/*$Id: s_fo.cc 2016/09/26 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2020 Felix Salfelder
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * tran and fourier commands -- top
 * performs transient analysis, silently, then fft.
 * outputs results of fft
 */

/// the actual bug... the (old) fourier command did not work with store (and
/// breaks gnucap-python) as it only prints the output. I have postponed it,
/// as it is only tangential to output plugins -- i could use a modified plugin
/// where required.

//testing=script 2014.07.04
#include "globals.h"
#include "u_sim_data.h"
#include "u_status.h"
#include "declare.h"	/* fft */
#include "u_prblst.h"
#include "s_tr.h"
#include "u_out.h"
#include "trace_on.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class FOURIER_PROBE : public PROBE_BASE {
public:
  ~FOURIER_PROBE();
private:
  FOURIER_PROBE();
  FOURIER_PROBE(FOURIER_PROBE const&) : PROBE_BASE(){unreachable();}
public:
  explicit FOURIER_PROBE(PROBE_BASE const*Brh)
    : PROBE_BASE("", Brh){
      set_label(Brh->long_label());
      _probes.insert(this);
  }
public:
private: // PROBE_BASE
  bool is_complex_type() const{untested(); return _idx!=-1;}
  PROBE_VALUE store() {
    if(_idx==-1){ untested();
      PROBE_BASE const* b=prechecked_cast<PROBE_BASE const*>(brh());
      assert(b);
      double v = b->value();
      _data.push_back(v);
      return v;
    }else{
      assert(_idx<int(_data.size()));
      return PROBE_VALUE(_data[_idx]);
    }
  }
  PROBE_VALUE value() const{
    if(_idx==-1){ untested();
      PROBE_BASE const* b=prechecked_cast<PROBE_BASE const*>(brh());
      assert(b);
      double v = b->value();
      return v;
    }else{
      assert(_idx<int(_data.size()));
      return PROBE_VALUE(_data[_idx]);
    }
  }
public:
  static void reset();
  static void eval();
  static void fft(int timesteps);
public:
  static int _idx;
private:
  typedef std::set<FOURIER_PROBE*> bag;
  static bag _probes;
  std::vector<COMPLEX> _data;
};
FOURIER_PROBE::bag FOURIER_PROBE::_probes;
int FOURIER_PROBE::_idx;
/*--------------------------------------------------------------------------*/
FOURIER_PROBE::~FOURIER_PROBE()
{
  _probes.erase(this);
}
void FOURIER_PROBE::reset()
{
  trace1("reset", _probes.size());
  for(bag::iterator i=_probes.begin(); i!=_probes.end(); ++i){
    (*i)->_data.resize(0);
  }
}
void FOURIER_PROBE::eval()
{
  for(bag::iterator i=_probes.begin(); i!=_probes.end(); ++i){
    (*i)->store();
  }
}
void FOURIER_PROBE::fft(int timesteps)
{ untested();
  FOURIER_PROBE::_idx = 0;
  for(bag::iterator i=_probes.begin(); i!=_probes.end(); ++i){
    assert(timesteps<=int((*i)->_data.size()));
    ::fft((*i)->_data.data(), timesteps,  0);
    (*i)->_data[0] /= 2;
    trace2("fft", (*i)->long_label(), (*i)->_data[0]);
  }
}
/*--------------------------------------------------------------------------*/
class transient_data_tap : public OUTPUT{
public:
  transient_data_tap(TRANSIENT* t)
    : _t(t) // , _outprobes(op), _timesteps(ts), _stepno(0) 
  {
    _t->attach_output(this);
  }
  ~transient_data_tap(){
    _t->detach_output(this);
  }
private: // pure, unused
  OUTPUT* clone()const { unreachable(); return NULL; }
private:
  void init(int, const std::string&){}
  void head(double, double, const std::string&){untested();}

  // here we receive data from TRANSIENT
  void commit(double /*X*/, int Level){ untested();
    if (Level >= dl_STROBE) {
      FOURIER_PROBE::eval();
    }else{
    }
  }

  void flush(){untested();}
private:
  SIM* _t;
  PROBELIST const* _outprobes;
  size_t _timesteps;
  unsigned _stepno;
};
/*--------------------------------------------------------------------------*/
class FOURIER : public TRANSIENT {
public:
  void	do_it(CS&, CARD_LIST*);
  explicit FOURIER():
    TRANSIENT(),
    _fstart(0.),
    _fstop(0.),
    _fstep(0.),
    _timesteps(0),
    _fdata(NULL)
  {}
  ~FOURIER() {}
private:
  explicit FOURIER(const FOURIER&): TRANSIENT() {unreachable(); incomplete();}
  std::string status()const {untested();return "";}
  void	setup(CS&);
  void	foout();
  void	fohead(const PROBE_BASE&);
  void	foprint(COMPLEX*);
  PROBE_BASE const* tap_probe(PROBE_BASE const*) const;
  void tr_sweep();
public: // tap
  size_t timesteps() const{ return _timesteps; }
private:
  PARAMETER<double> _fstart;	/* user start frequency */
  PARAMETER<double> _fstop;	/* user stop frequency */
  PARAMETER<double> _fstep;	/* fft frequecncy step */
  int    _timesteps;	/* number of time steps in tran analysis, incl 0 */
public:
  std::vector<COMPLEX>* _fdata;	/* storage to allow postprocessing */
private:
  OMSTREAM _out;
};
/*--------------------------------------------------------------------------*/
static	int	to_pow_of_2(double);
static  int	stepnum(double,double,double);
/*--------------------------------------------------------------------------*/
void FOURIER::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
  _sim->_label = "fourier";
  _sim->set_command_fourier();
  reset_timers();
  ::status.four.reset().start();

  try {
    setup(Cmd);
    _sim->init();
    CARD_LIST::card_list.precalc_last();
    _sim->alloc_vectors();
    _sim->_aa.reallocate();
    _sim->_aa.dezero(OPT::gmin);
    _sim->_aa.set_min_pivot(OPT::pivtol);
    _sim->_lu.reallocate();
    _sim->_lu.dezero(OPT::gmin);
    _sim->_lu.set_min_pivot(OPT::pivtol);


    FOURIER_PROBE::reset();
    ::status.set_up.stop();


    switch (ENV::run_mode) {
    case rPRE_MAIN:	unreachable();		break;
    case rBATCH:
      // fall through
    case rINTERACTIVE:  itested();
      // fall through
    case rSCRIPT:	tr_sweep(); foout();	break;
    case rPRESET:	untested(); /*nothing*/ break;
    }
  }catch (Exception& e) {untested();
    error(bDANGER, e.message() + '\n');
  }

  FOURIER_PROBE::reset(); // unallocate scratch space.
  _sim->unalloc_vectors();
  _sim->_lu.unallocate();
  _sim->_aa.unallocate();

  _sim->_has_op = s_FOURIER;
  _scope = NULL;

  ::status.four.stop();
  ::status.total.stop();
  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* foout:  send the results of the transform
 */
void FOURIER::foout()
{ untested();
  out_init(tNONE);
  FOURIER_PROBE::fft(_timesteps-1);
  out_head(_fstart, _fstop, "Freq");

  int startstep = stepnum(0., _fstep, _fstart);
  int stopstep = stepnum(0., _fstep, _fstop );
  assert(stopstep < int(_timesteps));
  int& ii = FOURIER_PROBE::_idx;

  trace2("foout", startstep, stopstep);
  for (ii=startstep; ii<=stopstep; ++ii) { untested();
    double frequency = _fstep * ii;
    out_commit(frequency, dl_STROBE);
  }
  out_flush();
}
/*--------------------------------------------------------------------------*/
void FOURIER::tr_sweep()
{
  transient_data_tap tdg(this);
  FOURIER_PROBE::_idx = -1;
  sweep();
}
/*--------------------------------------------------------------------------*/
/* stepnum: return step number given its frequency or time
 */
static int stepnum(double Start, double Step, double Here)
{
  return int((Here-Start)/Step + .5);
}
/*--------------------------------------------------------------------------*/
/* fo_setup: fourier analysis: parse command string and set options
 * 	(options set by call to TRANSIENT::options)
 */
void FOURIER::setup(CS& Cmd)
{
  _cont = true;

  if (Cmd.match1("'\"({") || Cmd.is_pfloat()) {
    PARAMETER<double> arg1, arg2, arg3;
    Cmd >> arg1;
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg2;
    }else{untested();
    }
    if (Cmd.match1("'\"({") || Cmd.is_float()) {
      Cmd >> arg3;
    }else{untested();
    }
    
    if (arg3.has_hard_value()) {	    /* 3 args: all */
      assert(arg2.has_hard_value());
      assert(arg1.has_hard_value());
      _fstart = arg1;
      _fstop  = arg2;
      _fstep  = arg3;
    }else if (arg2.has_hard_value()) {untested(); /* 2 args: start = 0 */
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      arg2.e_val(0.,_scope);
      if (arg1 >= arg2) {untested();	    /* 2 args: stop, step */
	_fstart = "NA";		    	    /* 	(stop > step) */
	_fstop  = arg1;
	_fstep  = arg2;
      }else{untested(); /* arg1 < arg2 */    /* 2 args: step, stop */
	_fstart = "NA";
	_fstop  = arg2;
	_fstep  = arg1;
      }
    }else{untested();
      assert(arg1.has_hard_value());
      arg1.e_val(0.,_scope);
      if (arg1 == 0.) {untested();	    /* 1 arg: start */
	_fstart = 0.;
	/* _fstop unchanged */
	/* _fstep unchanged */
      }else{untested();			    /* 1 arg: step */
	_fstart = "NA";
	_fstop  = "NA";
	_fstep  = arg1;
      }
    }
  }else{untested();
    /* else (no args) : no change */
  }

  options(Cmd);
  _out = IO::mstdout; // tmp hack

  _fstart.e_val(0., _scope);
  _fstep.e_val(0., _scope);
  _fstop.e_val(OPT::harmonics * _fstep, _scope);
  
  if (_fstep == 0.) {untested();
    throw Exception("frequency step = 0");
  }else{
  }
  if (_fstop == 0.) {untested();
    _fstop = OPT::harmonics * _fstep;
  }else{
  }

  _timesteps = to_pow_of_2(_fstop*2 / _fstep) + 1;


  if (_cold  ||  _sim->_last_time <= 0.) {
    _cont = false;
    _tstart = 0.;
  }else{
    _cont = true;
    _tstart = _sim->_last_time;
  }

  _tstop = _tstart + 1. / _fstep;
  _tstrobe = 1. / (_fstep * double(_timesteps-1));
  _time1 = _sim->_time0 = _tstart;

  _sim->_freq = _fstep;

  _dtmax = std::min(double(_dtmax_in), _tstrobe / double(_skip_in));
  if (_dtmin_in.has_hard_value()) {untested();
    _sim->_dtmin = _dtmin_in;
  }else if (_dtratio_in.has_hard_value()) {untested();
    _sim->_dtmin = _dtmax / _dtratio_in;
  }else{
    // use smaller of soft values
    _sim->_dtmin = std::min(double(_dtmin_in), _dtmax/_dtratio_in);
  }


  out_reset();
  out_set(Cmd);
}
/*--------------------------------------------------------------------------*/
PROBE_BASE const* FOURIER::tap_probe(PROBE_BASE const* n) const
{
  return new FOURIER_PROBE(n);
  return n;
}
/*--------------------------------------------------------------------------*/
/* to_pow_of_2: round up to nearest power of 2
 * example: z=92 returns 128
 */
static int to_pow_of_2(double Z)
{
  int x = static_cast<int>(floor(Z));
  int y;
  for (y = 1; x > 0; x >>= 1) {
    y <<= 1;
  }
  return y;
}   
/*--------------------------------------------------------------------------*/
static FOURIER p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "fourier", &p3);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
