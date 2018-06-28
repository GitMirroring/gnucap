/*$Id: d_logicmod.cc,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx vdd out in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
//testing=script 2006.07.17
#include "d_logic.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
BUILTIN_LOGIC::BUILTIN_LOGIC(int a)
  : COMMON_LOGIC(a)
{ untested();
  ++_count;
}
/*--------------------------------------------------------------------------*/
BUILTIN_LOGIC::BUILTIN_LOGIC(const BUILTIN_LOGIC& p) : COMMON_LOGIC(p)
{ untested();
}
/*--------------------------------------------------------------------------*/
COMMON_LOGIC::COMMON_LOGIC(const COMMON_LOGIC& p)
  :COMMON_COMPONENT(p),
   delay  (p.delay),
   vmax   (p.vmax),
   vmin	  (p.vmin),
   unknown(p.unknown),
   rise   (p.rise),
   fall   (p.fall),
   rs     (p.rs),
   rw     (p.rw),
   th1    (p.th1),
   th0    (p.th0),
   mr     (p.mr),
   mf     (p.mf),
   over   (p.over),
   range  (p.range),
   _deflated  (p._deflated)
{ untested();
 // ++_count;
}
/*--------------------------------------------------------------------------*/
void BUILTIN_LOGIC::precalc_first(CARD_LIST const* par_scope)
{ untested();
  COMMON_LOGIC::precalc_first(par_scope);

//  const CARD_LIST* par_scope = scope();
  assert(par_scope);

  delay.e_val(1e-9, par_scope);
  vmax.e_val(5., par_scope);
  vmin.e_val(0., par_scope);
  unknown.e_val((vmax+vmin)/2, par_scope);
  rise.e_val(delay / 2, par_scope);
  fall.e_val(delay / 2, par_scope);
  rs.e_val(100., par_scope);
  rw.e_val(1e9, par_scope);
  th1.e_val(.75, par_scope);
  th0.e_val(.25, par_scope);
  mr.e_val(5., par_scope);
  mf.e_val(5., par_scope);
  over.e_val(.1, par_scope);

  range = vmax - vmin;
}
/*--------------------------------------------------------------------------*/
void BUILTIN_LOGIC::set_param_by_index(int i, std::string& value, int offset)
{ untested();
  switch (BUILTIN_LOGIC::param_count() - 1 - i) {
  case 0: delay = value; break;
  case 1: vmax = value; break;
  case 2: vmin = value; break;
  case 3: unknown = value; break;
  case 4: rise = value; break;
  case 5: fall = value; break;
  case 6: rs = value; break;
  case 7: rw = value; break;
  case 8: th1 = value; break;
  case 9: th0 = value; break;
  case 10: mr = value; break;
  case 11: mf = value; break;
  case 12: over = value; break;
  default: COMMON_LOGIC::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool BUILTIN_LOGIC::param_is_printable(int i)const
{ untested();
  switch (param_count() - 1 - i) {
  case 0: 
  case 1: 
  case 2: 
  case 3: 
  case 4: 
  case 5: 
  case 6: 
  case 7: 
  case 8: 
  case 9: 
  case 10:
  case 11:
  case 12: return true;
  default: return COMMON_LOGIC::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string BUILTIN_LOGIC::param_name(int i)const
{ untested();
  switch (BUILTIN_LOGIC::param_count() - 1 - i) {
  case 0: return "delay";
  case 1: return "vmax";
  case 2: return "vmin";
  case 3: return "unknown";
  case 4: return "rise";
  case 5: return "fall";
  case 6: return "rs";
  case 7: return "rw";
  case 8: return "thh";
  case 9: return "thl";
  case 10: return "mr";
  case 11: return "mf";
  case 12: return "over";
  default: return COMMON_LOGIC::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string BUILTIN_LOGIC::param_name(int i, int j)const
{ untested();
  if (j == 0) { untested();
    return param_name(i);
  }else if (i >= COMMON_LOGIC::param_count()) { untested();
    return "";
  }else{ untested();
    return COMMON_LOGIC::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string BUILTIN_LOGIC::param_value(int i)const
{ untested();
  switch (param_count() - 1 - i) {
  case 0: return delay.string();
  case 1: return vmax.string();
  case 2: return vmin.string();
  case 3: return unknown.string();
  case 4: return rise.string();
  case 5: return fall.string();
  case 6: return rs.string();
  case 7: return rw.string();
  case 8: return th1.string();
  case 9: return th0.string();
  case 10: return mr.string();
  case 11: return mf.string();
  case 12: return over.string();
  default: return COMMON_LOGIC::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
namespace{
// spice stuff. wrap BUILTIN_LOGIC (former MODEL_LOGIC) into a MODEL_CARD
class MODEL_LOGIC : public MODEL_CARD {
public:
  MODEL_LOGIC(BUILTIN_LOGIC* l)
    : MODEL_CARD(NULL), _logic(NULL)
  { untested();
    COMMON_COMPONENT::attach_common(l, &_logic);
  }
  ~MODEL_LOGIC(){
    COMMON_COMPONENT::attach_common(NULL, &_logic);
  }
private:
  MODEL_LOGIC(MODEL_LOGIC const& l)
    : MODEL_CARD(l), _logic(NULL)
  { untested();
    COMMON_COMPONENT::attach_common(
	prechecked_cast<COMMON_COMPONENT*>(l._logic->clone()), &_logic);
  }
  virtual CARD*	 clone()const { itested();
    return new MODEL_LOGIC(*this);
  }
  std::string  dev_type()const         {return "logic";}
  int          param_count()const      {return ( _logic->param_count());}
  void set_param_by_name(std::string n, std::string v){
    assert(_logic);
    _logic->set_param_by_name(n, v);
  }
private: // actual logic here.
  COMMON_COMPONENT* _logic;
};
BUILTIN_LOGIC logic(CC_STATIC);
MODEL_LOGIC L(&logic);
static DISPATCHER<MODEL_CARD>::INSTALL d2(&model_dispatcher, "logic", &L);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
