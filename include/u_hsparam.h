/*                   -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * hierarchical system parameters
 */
/*--------------------------------------------------------------------------*/
#ifndef HS_PARAM_H
#define HS_PARAM_H
/*--------------------------------------------------------------------------*/
#include "u_parameter.h"
#include "io_trace.h"
#include "e_paramlist.h"
/*--------------------------------------------------------------------------*/
static const int sysparams_count = 8;
/*--------------------------------------------------------------------------*/
class HS_PARAM : public COMMON_PARAMLIST {
  PARAMETER<double> _mfactor;
  PARAMETER<double> _xposition;
  PARAMETER<double> _yposition;
  PARAMETER<double> _zposition;
  PARAMETER<double> _hflip;
  PARAMETER<double> _vflip;
  PARAMETER<double> _zflip;
  PARAMETER<double> _angle;

private: // fixed values. combining hierarchical and specified.
  double _mfactor_fixed{1.};
  // method_t _method_fixed{meUNKNOWN};

  explicit HS_PARAM(HS_PARAM const& p) : COMMON_PARAMLIST(p),
    _mfactor(p._mfactor),
    _xposition(p._xposition),
    _yposition(p._yposition),
    _zposition(p._zposition),
    _hflip(p._hflip),
    _vflip(p._vflip),
    _zflip(p._zflip),
    _angle(p._angle),
    _mfactor_fixed(p._mfactor_fixed){
    }
public:
  explicit HS_PARAM() : COMMON_PARAMLIST() {
    _mfactor.set_default(1.);
    _xposition.set_default(0.);
    _yposition.set_default(0.);
    _zposition.set_default(0.);
    _hflip.set_default(1);
    _vflip.set_default(1);
    _zflip.set_default(1);
    _angle.set_default(0.);
  }
  ~HS_PARAM() {}
  bool operator==(const COMMON_COMPONENT& x)const override {
    auto* p = dynamic_cast<HS_PARAM const*>(&x);
    return (p
      && _mfactor == p->_mfactor
      && _xposition == p->_xposition
      && _yposition == p->_yposition
      && _zposition == p->_zposition
      && _hflip == p->_hflip
      && _vflip == p->_vflip
      && _zflip == p->_zflip
      && _angle == p->_angle
      && _mfactor_fixed == p->_mfactor_fixed // needed? must be in device anyway
      && COMMON_COMPONENT::operator==(x));
  }
  HS_PARAM* clone()const override { return new HS_PARAM(*this); }

  bool param_is_printable(int i)const override {
    switch(HS_PARAM::param_count() - 1 - i) {
    case 0:
      return _mfactor.has_hard_value();
    case 1:
      return _xposition.has_hard_value();
    case 2:
      return _yposition.has_hard_value();
    case 3:
      return _zposition.has_hard_value();
    case 4:
      return _hflip.has_hard_value();
    case 5:
      return _vflip.has_hard_value();
    case 6:
      return _zflip.has_hard_value();
    case 7:
      return _angle.has_hard_value();
    default:
      unreachable();
      return false;
    }
  }
  int param_count()const override {
    return sysparams_count;
  }
  void set_param_by_index(int i, std::string& v, int)override {
    switch(HS_PARAM::param_count() - 1 - i) {
    case 0:
      _mfactor = v;
      break;
    case 1:
      _xposition = v;
      break;
    case 2:
      _yposition = v;
      break;
    case 3: untested();
      _zposition = v;
      break;
    case 4: untested();
      _hflip = v;
      break;
    case 5: untested();
      _vflip = v;
      break;
    case 6: untested();
      _zflip = v;
      break;
    case 7: untested();
      _angle = v;
      break;
    default: untested();
      throw Exception_Too_Many(i, HS_PARAM::param_count(), 0);
    }
  }
  // use base class, once it's been cleaned up.
  int set_param_by_name(std::string Name, std::string Value) override {
    int which = -1;
    for(int i = 0; i<sysparams_count; ++i){
      if( Name == param_name(HS_PARAM::param_count() - 1 - i) ){
	which = HS_PARAM::param_count() - 1 - i;
	break;
      }else{
      }
    }
    if(which>=0){
      set_param_by_index(which, Value, 0);
      return which;
    }else{
      trace2("hsp::spbn base", Name, Value);
      return COMMON_COMPONENT::set_param_by_name(Name, Value);
    }
  }

  std::string param_name(int i, int j)const override {
    assert(i < HS_PARAM::param_count());
    if (j == 0) {
      return param_name(i);
    }else{
      return "";
    }
  }
  std::string param_name(int i)const override {
    static std::string hspname[sysparams_count] { //
      "$mfactor",
      "$xposition",
      "$yposition",
      "$zposition",
      "$hflip",
      "$vflip",
      "$zflip",
      "$angle"
    };
    int I = HS_PARAM::param_count() - 1 - i;
    assert(I>=0);
    if(I<sysparams_count){
      return hspname[I];
    }else{ untested();
      unreachable();
      return "";
    }
  }
  std::string param_value(int i)const override {
   // return "pv"+to_string(i);
    switch(HS_PARAM::param_count() - 1 - i) {
    case 0:
      return _mfactor.string();
    case 1:
      return _xposition.string();
    case 2:
      return _yposition.string();
    case 3: untested();
      return _zposition.string();
    case 4: untested();
      return _hflip.string();
    case 5: untested();
      return _vflip.string();
    case 6: untested();
      return _zflip.string();
    case 7: untested();
      return _angle.string();
    default: untested();
	     unreachable();
	     return "???";
    }
  }
  double mfactor()const {
    assert(_mfactor_fixed);
    return _mfactor_fixed;
  }
#ifndef NDEBUG
  double mfactor_specified()const { untested();
    return _mfactor;
  }
#endif

  void precalc_first(CARD_LIST const* )override {
    // incomplete();
  }

  void precalc(CARD const*) { untested();
    incomplete();
  }

  void expand(COMPONENT const* c) override {
    assert(c);
    CARD const* owner = c->owner();
    CARD_LIST const* scope;
    double mfactor_hier;
    if(auto comp = dynamic_cast<COMPONENT const*>(owner)){
      scope = owner->scope();
      if(comp->hsparam()){
	mfactor_hier = comp->hsparam()->mfactor();
      }else{
	mfactor_hier = 1.; // assert?
      }
    }else{
      scope = &CARD_LIST::card_list;
      mfactor_hier = 1.;
    }

    _mfactor.e_val(1., scope);
    _mfactor_fixed = _mfactor * mfactor_hier;
    trace4("hsp expand", c->long_label(), scope, _mfactor, _mfactor_fixed);

    _xposition.e_val(0., scope);
    _yposition.e_val(0., scope);
    _zposition.e_val(0., scope);
    _hflip.e_val(1, scope);
    _vflip.e_val(1, scope);
    _zflip.e_val(1, scope);
    _angle.e_val(0., scope);
  }

  HS_PARAM* hsparam()override { return this; }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
