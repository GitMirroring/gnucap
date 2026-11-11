/*$Id: bm_generator.cc 2015.01.08 al $ -*- C++ -*-
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
 * behavioral modeling simple value
 * used with tc, etc, and conditionals
 */
//testing=script 2015.01.08
//BUG// no step control, except delay
//BUG// encapsulation violation: _sim->_genout
#include "globals.h"
#include "e_elemnt.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
const int genparam_count = 12;
double default_freq = 0.;
double default_ampl = 1.;
double default_phaz = 0.;
double default_maxv = 1.;
double default_minv = 0.;
double default_offset = 0.;
double default_init_ = 0.;
double default_rise = 1e-12;
double default_fall = 1e-12;
double default_delay = 0.;
double default_width = 0.;
double default_period = 0.;
/*--------------------------------------------------------------------------*/
class EVAL_BM_GENERATOR : public EVAL_BM_ACTION_BASE {
  static PARAMETER<double> _freq;
  static PARAMETER<double> _ampl;
  static PARAMETER<double> _phaz;
  static PARAMETER<double> _maxv;
  static PARAMETER<double> _minv;
  static PARAMETER<double> _offset;
  static PARAMETER<double> _init_;
  static PARAMETER<double> _rise;
  static PARAMETER<double> _fall;
  static PARAMETER<double> _delay;
  static PARAMETER<double> _width;
  static PARAMETER<double> _period;
  static double _g;
private:
  explicit	EVAL_BM_GENERATOR(const EVAL_BM_GENERATOR& p);
public:
  explicit      EVAL_BM_GENERATOR(int c=0);
		~EVAL_BM_GENERATOR()	{}
private: // override virtual
  bool		operator==(const COMMON_COMPONENT&)const override;
  COMMON_COMPONENT* clone()const override {return new EVAL_BM_GENERATOR(*this);}
  int		param_count()const override {return genparam_count;}
  std::string	param_name(int i)const override;
  std::string	param_name(int i, int j)const override;
  void		set_param_by_index(int I, std::string& V, int Offset)override;
  bool		has_parse_params_obsolete_callback()const override { return false;}
  void		print_common_obsolete_callback(OMSTREAM&, LANGUAGE*)const override;
  void		precalc_last(PARAM_LIST const* Scope)override;
  void		tr_eval(ELEMENT*)const override;
  void		ac_eval(ELEMENT*)const override;
  std::string	name()const override		{return "generator";}
  bool		ac_too()const override		{return true;}
  bool		parse_numlist(CS&)override;
private:
  double gen()const;
  double value()const override { return _g;}
};
/*--------------------------------------------------------------------------*/
PARAMETER<double> EVAL_BM_GENERATOR::_freq;
PARAMETER<double> EVAL_BM_GENERATOR::_ampl;
PARAMETER<double> EVAL_BM_GENERATOR::_phaz;
PARAMETER<double> EVAL_BM_GENERATOR::_maxv;
PARAMETER<double> EVAL_BM_GENERATOR::_minv;
PARAMETER<double> EVAL_BM_GENERATOR::_offset;
PARAMETER<double> EVAL_BM_GENERATOR::_init_;
PARAMETER<double> EVAL_BM_GENERATOR::_rise;
PARAMETER<double> EVAL_BM_GENERATOR::_fall;
PARAMETER<double> EVAL_BM_GENERATOR::_delay;
PARAMETER<double> EVAL_BM_GENERATOR::_width;
PARAMETER<double> EVAL_BM_GENERATOR::_period;
double EVAL_BM_GENERATOR::_g = 0.;
/*--------------------------------------------------------------------------*/
EVAL_BM_GENERATOR::EVAL_BM_GENERATOR(int c)
  :EVAL_BM_ACTION_BASE(c)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_GENERATOR::EVAL_BM_GENERATOR(const EVAL_BM_GENERATOR& p)
  :EVAL_BM_ACTION_BASE(p)
{
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_GENERATOR::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_GENERATOR* p = dynamic_cast<const EVAL_BM_GENERATOR*>(&x);
  if (p) {
  }else{untested();
  }
  bool rv = p && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
  }else{ untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  o << name();
  EVAL_BM_ACTION_BASE::print_common_obsolete_callback(o, lang);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::precalc_last(PARAM_LIST const*)
{
  // BUG: lost Scope
  PARAM_LIST Scope;
  _freq.e_val(default_freq, &Scope);
  _ampl.e_val(default_ampl, &Scope);
  _phaz.e_val(default_phaz, &Scope);
  _maxv.e_val(default_maxv, &Scope);
  _minv.e_val(default_minv, &Scope);
  _offset.e_val(default_offset, &Scope);
  _init_.e_val(default_init_, &Scope);
  _rise.e_val(default_rise, &Scope);
  _fall.e_val(default_fall, &Scope);
  _delay.e_val(default_delay, &Scope);
  _width.e_val(default_width, &Scope);
  _period.e_val(default_period, &Scope);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::tr_eval(ELEMENT* d)const
{
  if(_sim->command_is_op()) {
    _g = 0.;
  }else{
    _g = gen();
  }
  double dtmin = d->_sim->_dtmin;
  if(!d->_sim->_time0){
    if(_delay > 2.01*dtmin){
      d->_sim->new_event(_delay, d);
      d->_sim->new_event(_delay+dtmin, d);
    }else{
    }
    if(_rise > 2.01*dtmin){
      d->_sim->new_event(_delay+_rise, d);
      d->_sim->new_event(_delay+_rise+dtmin, d);
    }else{
    }
  }else{
  }
  tr_finish_tdv(d, _g);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::ac_eval(ELEMENT* d)const
{
  d->_ev = 1;
  ac_final_adjust_with_temp(&(d->_ev));
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_GENERATOR::parse_numlist(CS& cmd)
{
  size_t here = cmd.cursor();
  PARAMETER<double> new_value(NOT_VALID);
  cmd >> new_value;
  if (cmd.gotit(here)) {
    _scale = new_value;
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string EVAL_BM_GENERATOR::param_name(int I) const
{
  static std::string names[genparam_count] { //
    "freq",
    "ampl",
    "phase",
    "max",
    "min",
    "offset",
    "init",
    "rise",
    "fall",
    "delay",
    "width",
    "period"
  };
  assert(I>=0);
  if(I<genparam_count){
    return names[I];
  }else{ untested();
    return EVAL_BM_ACTION_BASE::param_name(I-genparam_count);
  }
}
/*--------------------------------------------------------------------------*/
std::string EVAL_BM_GENERATOR::param_name(int i, int j) const
{
  assert(i < EVAL_BM_GENERATOR::param_count());
  if (j == 0) {
    return param_name(i);
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_GENERATOR::set_param_by_index(int I, std::string& V, int Offset)
{
  switch(I) {
  case 0:
    _freq = V;
    break;
  case 1:
    _ampl = V;
    break;
  case 2:
    _phase = V;
    break;
  case 3:
    _maxv = V;
    break;
  case 4:
    _minv = V;
    break;
  case 5:
    _offset = V;
    break;
  case 6:
    _init_ = V;
    break;
  case 7:
    _rise = V;
    break;
  case 8:
    _fall = V;
    break;
  case 9:
    _delay = V;
    break;
  case 10:
    _width = V;
    break;
  case 11:
    _period = V;
    break;
  default: untested();
    return EVAL_BM_ACTION_BASE::set_param_by_index(I-genparam_count, V, Offset+genparam_count);
  }
}
/*--------------------------------------------------------------------------*/
double EVAL_BM_GENERATOR::gen() const
{
  if (CKT_BASE::_sim->_time0 <= _delay) {
    return _init_;
  }else{
  }
  double loctime = CKT_BASE::_sim->_time0 - _delay;
  if (_period > 0.) {
    loctime = fmod(loctime, _period);
  }else{
    // not periodic
  }

  double level;
  if (CKT_BASE::_sim->_time0 <= _delay + _rise) {	/* initial rise */
    level = (_maxv - 0) * (loctime/_rise) + 0;
  }else if (loctime <= _rise) {				/* rising       */
    level = (_maxv - _minv) * (loctime/_rise) + _minv;
  }else if (_width==0.  ||  (loctime-=_rise) <= _width) {/* pulse on     */
    level = _maxv;
  }else if ((loctime-=_width) <= _fall) {		/* falling      */
    level = (_minv - _maxv) * (loctime/_fall) + _maxv;
  }else{						/* pulse off    */
    level = _minv;
  }
  level *= (_freq == 0.)
    ? _ampl
    : _ampl * sin(M_TWO_PI * _freq*(CKT_BASE::_sim->_time0-_delay) + _phaz * DTOR);
  return (CKT_BASE::_sim->_time0 <= _delay + _rise)
    ? level + (_offset - _init_) * (loctime/_rise) + _init_
    : level + _offset;
}
/*--------------------------------------------------------------------------*/
EVAL_BM_GENERATOR p1(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL d1(&bm_dispatcher, "gen|generator", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
