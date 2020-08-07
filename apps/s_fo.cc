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
#include "declare.h"	/* plclose, plclear, fft */
#include "u_prblst.h"
#include "s_tr.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD_FFT : public OUTPUT_CMD {
private:
  int _stepno;		// count of visible (saved) steps
  COMPLEX** _fdata;	/* storage to allow postprocessing */
  int    _timesteps;	/* number of time steps in tran analysis, incl 0 */
  double _fstart;	/* user start frequency */
  double _fstop;	/* user stop frequency */
  double _fstep;	/* fft frequecncy step */
private:
  explicit OUTPUT_CMD_FFT(const OUTPUT_CMD_FFT&p) : OUTPUT_CMD(p) {}
public:
  OUTPUT_CMD_FFT() :
    OUTPUT_CMD(),
    _stepno(0),
    _fdata(NULL),
    _timesteps(0),
    _fstart(NOT_VALID),
    _fstop(NOT_VALID),
    _fstep(NOT_VALID)
  {
    set_label("fft");
  }
  virtual ~OUTPUT_CMD_FFT()		{/*assert(!_fdata);*/}
public: // OUTPUT_CMD
  OUTPUT_CMD* clone() const		{return new OUTPUT_CMD_FFT(*this);}
public: // OUTPUT
  void init(int Level, const std::string& Label);
  void head(double start, double stop, const std::string&);
  void t_head(double start, double stop, const std::string&);
  void commit(double XX, int Level);
  void flush();
private:
  void	fftunallocate();
  void	foout();
  void	fohead(const PROBE&);
  void	foprint(COMPLEX*);
}; // OUTPUT_CMD_FFT
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class FOURIER : public TRANSIENT {
public:
  void	do_it(CS&, CARD_LIST*);
  explicit FOURIER():
    TRANSIENT(),
    _fstart(0.),
    _fstop(0.),
    _fstep(0.),
    _timesteps(0)
  {}
  ~FOURIER() {}
private:
  explicit FOURIER(const FOURIER&): TRANSIENT() {unreachable(); incomplete();}
  std::string status()const {untested();return "";}
  void	setup(CS&);	/* s_fo_set.cc */
#if 0
  void	fftallocate();
  void	fftunallocate();
  void	foout();	/* s_fo_out.cc */
  void	fohead(const PROBE&);
  void	foprint(COMPLEX*);
  void	store_results(double); // override virtual
#endif
private:
  PARAMETER<double> _fstart;	/* user start frequency */
  PARAMETER<double> _fstop;	/* user stop frequency */
  PARAMETER<double> _fstep;	/* fft frequecncy step */
  int    _timesteps;	/* number of time steps in tran analysis, incl 0 */
};
/*--------------------------------------------------------------------------*/
static	int	to_pow_of_2(double);
static  int	stepnum(double,double,double);
static	COMPLEX	find_max(COMPLEX*,int,int);
static	double  db(COMPLEX);
/*--------------------------------------------------------------------------*/
void FOURIER::do_it(CS& Cmd, CARD_LIST* Scope)
{
  _scope = Scope;
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
    ////fftallocate();
    ::status.set_up.stop();

    switch (ENV::run_mode) {
    case rPRE_MAIN:	unreachable();		break;
    case rBATCH:
      // fall through
    case rINTERACTIVE:  itested();
      // fall through
    case rSCRIPT:	sweep(); /*////foout();*/	break;
    case rPRESET:	untested(); /*nothing*/ break;
    }
  }catch (Exception& e) {untested();
    error(bDANGER, e.message() + '\n');
  }
  ////fftunallocate();
  _sim->unalloc_vectors();
  _sim->_lu.unallocate();
  _sim->_aa.unallocate();

  _sim->_has_op = s_FOURIER;
  _scope = NULL;

  ::status.four.stop();
  ::status.total.stop();
  out_flush();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* allocate:  allocate space for fft
 */
//void FOURIER::fftallocate()
void OUTPUT_CMD_FFT::init(int, const std::string& /*Label*/)
{
  _timesteps = 1000;
  int probes = probelist().size();
  _fdata = new COMPLEX*[probes];
  for (int ii = 0;  ii < probes;  ++ii) {
    _fdata[ii] = new COMPLEX[_timesteps+100];
  }
  _stepno = 0;
}
/*--------------------------------------------------------------------------*/
void OUTPUT_CMD_FFT::head(double Tstart, double Tstop, const std::string&)
{
  _fstep = 1 / (Tstop - Tstart);
}
/*--------------------------------------------------------------------------*/
void OUTPUT_CMD_FFT::t_head(double Fstart, double Fstop, const std::string& Xvar)
{
  if (Xvar == "Freq") {
    _fstart = Fstart;
    _fstop = Fstop;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* store: stash time domain data in preparation for Fourier Transform
 */
void OUTPUT_CMD_FFT::commit(double, int Level)
{
  if (Level < dl_STROBE) {
    // only look at dl_STROBE or better.
  }else{
    int ii = 0;
    trace2("fft::commit", _stepno, _timesteps);
    for (PROBELIST::const_iterator
	   p=probelist().begin();  p!=probelist().end();  ++p) {
      //assert(_stepno < _timesteps);
      assert(_stepno < 1000);
      _fdata[ii][_stepno] = p->value();
      ++ii;
    }
    ++_stepno;
  }
}
/*--------------------------------------------------------------------------*/
/* foout:  print out the results of the transform
 */
void OUTPUT_CMD_FFT::flush()
{
  _timesteps = _stepno;
  int ii = 0;
  for (PROBELIST::const_iterator
	 p=probelist().begin();  p!=probelist().end();  ++p) {
    fohead(*p);
    fft(_fdata[ii], _timesteps-1,  0);
    foprint(_fdata[ii]);
    ++ii;
  }
}
/*--------------------------------------------------------------------------*/
/* fo_head: print output header
 * arg is index into probe array, to select probe name
 */
void OUTPUT_CMD_FFT::fohead(const PROBE& Prob)
{
  out().form("# %-10s", Prob.label().c_str())
    << "--------- actual ---------  -------- relative --------\n"
    << "#freq       "
    << "value        dB      phase  value        dB      phase\n";
}
/*--------------------------------------------------------------------------*/
/* fo_print: print results of fourier analysis
 * for all points at single probe
 */
void OUTPUT_CMD_FFT::foprint(COMPLEX *Data)
{
  int startstep = stepnum(0., _fstep, _fstart);
  assert(startstep >= 0);

  int stopstep  = stepnum(0., _fstep, _fstop );
  if (stopstep == 0) {
    stopstep = _timesteps/2 - 1;
  }else{
  }
  assert(stopstep < _timesteps/2);

  COMPLEX maxvalue = find_max(Data, std::max(1,startstep), stopstep);
  if (maxvalue == 0.) {
    maxvalue = 1.;
  }else{
  }
  Data[0] /= 2;
  for (int ii = startstep;  ii <= stopstep;  ++ii) {
    double frequency = _fstep * ii;
    assert(ii >= 0);
    assert(ii < _timesteps);
    COMPLEX unscaled = Data[ii];
    COMPLEX scaled = unscaled / maxvalue;
    unscaled *= 2;
    out().form("%s%s%7.2f %8.3f %s%7.2f %8.3f\n",
	     ftos(frequency,    11,5,out().format()),
        ftos(std::abs(unscaled),11,5,out().format()),
	     db(unscaled),
	     phase(unscaled*COMPLEX(0.,1)),
        ftos(std::abs(scaled),  11,5,out().format()),
	     db(scaled),
	     phase(scaled) ) ;
  }
}
/*--------------------------------------------------------------------------*/
/* stepnum: return step number given its frequency or time
 */
static int stepnum(double Start, double Step, double Here)
{
  if (Here != NOT_VALID && Step != NOT_VALID && Start != NOT_VALID) {
    return int((Here-Start)/Step + .5);
  }else{
    return 0;
  }
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
  _tstrobe = 1. / _fstep / (_timesteps-1);
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
  out_init(_trace);
  //out_head(_tstart, _tstop, "Time"); // in sweep()
  out_t_head(_fstart, _fstop, "Freq");
}
/*--------------------------------------------------------------------------*/
/* unallocate:  unallocate space for fft
 */
void OUTPUT_CMD_FFT::fftunallocate()
{
  if (_fdata) {
    for (int ii = 0;  ii < probelist().size();  ++ii) {
      delete [] _fdata[ii];
    }
    delete [] _fdata;
    _fdata = NULL;
  }else{untested();
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
FOURIER p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "fourier", &p3);
OUTPUT_CMD_FFT p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "fft", &p0);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
