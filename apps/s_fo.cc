/*$Id: s_fo.cc 2016/09/26 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
//testing=script 2014.07.04
#include "globals.h"
#include "u_sim_data.h"
#include "u_status.h"
#include "m_phase.h"
#include "declare.h"	/* fft */
#include "u_prblst.h"
#include "s_tr.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
static std::string default_transient_command="tran";
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
// maybe inherit from OUTPUT_CMD. merge with tap below
// better send output to _output (how?)
class FOURIER : public SIM {
public:
  void	do_it(CS&, CARD_LIST*);
  explicit FOURIER():
    SIM(),
    _fstart(0.),
    _fstop(0.),
    _fstep(0.),
    _timesteps(0),
    _fdata(NULL)
  {
  }
  ~FOURIER() {}
private:
  explicit FOURIER(const FOURIER&): SIM() {unreachable(); incomplete();}
  std::string status()const {untested();return "";}
  void	setup(CS&);
  void	fftallocate();
  void	fftunallocate();
  void	foout();
  void	fohead(const PROBE_BASE&);
  void	foprint(COMPLEX*);
  void tr_sweep();
private: // pure in SIM
  void	sweep()	{unreachable();}
public: // tap
//  int stepno() const{ return _tr->*&FTRAN::_stepno; }
  size_t timesteps() const{ return _timesteps; }
  int step_cause()const{ return _tr->step_cause(); }
private:
  PARAMETER<double> _fstart;	/* user start frequency */
  PARAMETER<double> _fstop;	/* user stop frequency */
  PARAMETER<double> _fstep;	/* fft frequecncy step */
  size_t    _timesteps;	/* number of time steps in tran analysis, incl 0 */
  double _tstart;
  double _tstop;
  double _tstrobe;
  bool _cold;
public:
  std::vector<COMPLEX>* _fdata;	/* storage to allow postprocessing */
private:
  OMSTREAM _out;
  TRANSIENT* _tr;
};
/*--------------------------------------------------------------------------*/
static	int	to_pow_of_2(double);
static  int	stepnum(double,double,double);
static	COMPLEX	find_max(COMPLEX*,int,int);
static	double  db(COMPLEX);
/*--------------------------------------------------------------------------*/
class transient_data_tap : public OUTPUT{
public:
  transient_data_tap(TRANSIENT* t, PROBELIST const* op, size_t ts,
      std::vector<COMPLEX>* fd)
    : _t(t), _outprobes(op), _timesteps(ts), _fdata(fd), _stepno(0) {
    _t->attach_output(this);
  }
  ~transient_data_tap(){
    _t->detach_output(this);
  }
private: // pure, unused
  void do_it(CS&, CARD_LIST*){ unreachable(); }
private:
  void head_(){
  }
  void commit(double /*X*/, int Flags){
    // _zap->commit(sel, Flags);
    store(Flags);
  }
  void store(int);
private:
  TRANSIENT* _t;
  PROBELIST const* _outprobes;
  size_t _timesteps;
  std::vector<COMPLEX>* _fdata;
  unsigned _stepno;
};
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

    fftallocate();
    ::status.set_up.stop();

    CS tropt(CS::_STRING, std::string()
	+ "mode=fourier trace=n"
	+ " " + Cmd.tail()
        + " start=" + to_string(_tstart)
        + " stop=" + to_string(_tstop)
        + " strobe=" + to_string(_tstrobe));

    {
      transient_data_tap tdg(_tr, outprobes(), _timesteps, _fdata);
      _tr->do_it(tropt, _scope);
    }


    switch (ENV::run_mode) {
    case rPRE_MAIN:	unreachable();		break;
    case rBATCH:
      // fall through
    case rINTERACTIVE:  itested();
      // fall through
    case rSCRIPT:	foout();	break;
    case rPRESET:	untested(); /*nothing*/ break;
    }
    fftunallocate();
  }catch (Exception& e) {untested();
    error(bDANGER, e.message() + '\n');
  }

  _sim->_has_op = s_FOURIER;
  _scope = NULL;

  ::status.four.stop();
  ::status.total.stop();
  
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* store: stash time domain data in preparation for Fourier Transform
 */
void transient_data_tap::store(int flag)
{
  if (! (flag & ofPRINT)){
  }else if (_outprobes){ itested();
    int ii = 0;
    for (PROBELIST::const_iterator p=_outprobes->begin();
         p!=_outprobes->end(); ++p) {
      trace2("tapstore", _stepno, _timesteps);
      assert(_stepno < unsigned(_timesteps));
      PROBE_BASE const* P=dynamic_cast<PROBE_BASE const*>(*p);
      if(!P){ untested();
	incomplete();
	continue;
      }else{
	_fdata[ii][_stepno] = P->value();
	++ii;
      }
    }
    ++_stepno;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* foout:  print out the results of the transform
 */
void FOURIER::foout()
{
  //plclose();
  //plclear();
  int ii = 0;
  if (PROBELIST const* pl=outprobes()){
    for (PROBELIST::const_iterator p=pl->begin(); p!=pl->end(); ++p) {
      PROBE_BASE const* P=dynamic_cast<PROBE_BASE const*>(*p);
      if(!P){ untested();
	incomplete();
	continue;
      }
      fohead(*P);
      fft(_fdata[ii].data(), int(_timesteps)-1,  0);
      foprint(_fdata[ii].data());
      ++ii;
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* fo_head: print output header
 * arg is index into probe array, to select probe name
 */
void FOURIER::fohead(const PROBE_BASE& Prob)
{
  _out.form("# %-10s", Prob.label().c_str())
    << "--------- actual ---------  -------- relative --------\n"
    << "#freq       "
    << "value        dB      phase  value        dB      phase\n";
}
/*--------------------------------------------------------------------------*/
/* fo_print: print results of fourier analysis
 * for all points at single probe
 */
void FOURIER::foprint(COMPLEX *Data)
{
  int startstep = stepnum(0., _fstep, _fstart);
  assert(startstep >= 0);
  int stopstep  = stepnum(0., _fstep, _fstop );
  assert(stopstep < int(_timesteps));
  COMPLEX maxvalue = find_max(Data, std::max(1,startstep), stopstep);
  if (maxvalue == 0.) {
    maxvalue = 1.;
  }else{
  }
  Data[0] /= 2;
  for (int ii=startstep; ii<=stopstep; ++ii) {
    double frequency = _fstep * ii;
    assert(ii < int(_timesteps));
    COMPLEX unscaled = Data[ii];
    COMPLEX scaled = unscaled / maxvalue;
    unscaled *= 2;
    _out.form("%s%s%7.2f %8.3f %s%7.2f %8.3f\n",
	     ftos(frequency,    11,5,_out.format()),
        ftos(std::abs(unscaled),11,5,_out.format()),
	     db(unscaled),
	     phase(unscaled*COMPLEX(0.,1)),
        ftos(std::abs(scaled),  11,5,_out.format()),
	     db(scaled),
	     phase(scaled) ) ;
  }
}
/*--------------------------------------------------------------------------*/
/* stepnum: return step number given its frequency or time
 */
static int stepnum(double Start, double Step, double Here)
{
  return int((Here-Start)/Step + .5);
}
/*--------------------------------------------------------------------------*/
/* find_max: find the max magnitude in a COMPLEX array
 */
static COMPLEX find_max(COMPLEX *Data, int Start, int Stop)
{
  COMPLEX maxvalue = 0.;
  for (int ii = Start;  ii <= Stop;  ++ii) {
    if (std::abs(Data[ii]) > std::abs(maxvalue)) {
      maxvalue = Data[ii];
    }else{
    }
  }
  return maxvalue;
}
/*--------------------------------------------------------------------------*/
static double db(COMPLEX Value)
{
  return  20. * log10(std::max(std::abs(Value),VOLTMIN));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* fo_setup: fourier analysis: parse command string and set options
 * 	(options set by call to TRANSIENT::options)
 */
void FOURIER::setup(CS& Cmd)
{
  std::string trcmd=default_transient_command;
  CMD* x=command_dispatcher[trcmd];
  if(!x){ untested();
    throw Exception_Cant_Find("fourier", trcmd);
  }else{
  }
  _tr = dynamic_cast<TRANSIENT*>(x);
  assert(_tr); // for now

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

  {
    unsigned here = Cmd.cursor();
    do{
      Get(Cmd, "c{old}",	   &_cold)
	;
    }while (Cmd.more() && !Cmd.stuck(&here));

    Cmd.reset(here);
  }

  _timesteps = size_t(to_pow_of_2(_fstop*2 / _fstep) + 1);

  if (_cold  ||  _sim->_last_time <= 0.) {
    _tstart = 0.;
  }else{
   _tstart = _sim->_last_time;
  }

  _tstop = _tstart + 1. / _fstep;
  _tstrobe = 1. / (_fstep * double(_timesteps-1));
  _sim->_freq = _fstep;

}
/*--------------------------------------------------------------------------*/
/* allocate:  allocate space for fft
 */
void FOURIER::fftallocate()
{
  assert(!_fdata);
  if(!outprobes()){
  }else if (PROBELIST const* pl=outprobes()){
    int probes = pl->size();
    _fdata = new std::vector<COMPLEX>[probes];
    for (int ii = 0;  ii < probes;  ++ii) {
      _fdata[ii].resize(_timesteps+100);
    }
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
/* unallocate:  unallocate space for fft
 */
void FOURIER::fftunallocate()
{
  if(!outprobes()){
  }else if (PROBELIST const* pl=outprobes()){
    assert (_fdata || !pl->size());
    delete [] _fdata;
    _fdata = NULL;
  }else{unreachable();
  }
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
