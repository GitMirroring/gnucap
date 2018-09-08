/*$Id: e_model.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
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
 * base class for all models
 */
//testing=script 2006.07.12
#include "e_compon.h"
#include "e_model.h"
#include "e_paramlist.h"
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD(const COMPONENT* p)
  :CARD(),
   _component_proto(NULL)
{
  if (p) {
    _component_proto = p->clone();
  }else{
    assert(!_component_proto);
  }
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD(const MODEL_CARD& p)
  :CARD(p),
   _component_proto(NULL)
{
  if (p._component_proto) {
    _component_proto = p._component_proto->clone();
  }else{
    assert(!_component_proto);
  }
  if (_sim) {
    _sim->uninit();
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::~MODEL_CARD()
{
  if (_component_proto) {
    delete _component_proto;
    _component_proto = NULL;
  }else{
  }
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::set_param_by_index(int i, std::string& value, int offset)
{
  if (_component_proto) {
    _component_proto->set_param_by_index(i, value, offset);
  }else{untested();
    CARD::set_param_by_index(i, value, offset);
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_CARD::param_is_printable(int i)const
{
  if (_component_proto) {
    return _component_proto->param_is_printable(i);
  }else{untested();
    return CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_name(int i)const
{
  if (_component_proto) {
    return _component_proto->param_name(i);
  }else{untested();
    return CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_name(int i, int j)const
{
  if (_component_proto) {
    return _component_proto->param_name(i, j);
  }else{untested();
    return CARD::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_value(int i)const
{
  if (_component_proto) {
    return _component_proto->param_value(i);
  }else{untested();
    return CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::precalc_first()
{
  if (_component_proto) {
    _component_proto->precalc_first();
  }else{
    CARD::precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
class COMMON_PARAMSET : public COMMON_PARAMLIST{
public:
  explicit  COMMON_PARAMSET() : COMMON_PARAMLIST(){}
  COMMON_PARAMSET( const COMMON_PARAMSET& p) : COMMON_PARAMLIST(0){}
private:
  COMMON_PARAMLIST* clone()const {incomplete(); return new COMMON_PARAMSET(*this); }
  std::string name()const{incomplete(); return "";}

};
/*--------------------------------------------------------------------------*/
CARD* MODEL_CARD::clone_instance()const
{ untested();
  if (_component_proto) { untested();
    CARD* x = _component_proto->clone_instance();

    if(!subckt()){ untested();
    // }else if(!x->subckt()){ untested();
    }else if(COMPONENT* c=dynamic_cast<COMPONENT*>(x)){ untested();

      CARD* o=x->owner();
      CARD_LIST* s=&CARD_LIST::card_list;
      if(o){
	s=o->scope();
      }
      //x->subckt()->attach_params(subckt()->params(), s);

      COMMON_PARAMSET* foo=new COMMON_PARAMSET;
 //     foo->_params.eval_copy(*subckt()->params(), s);
      c->attach_common(foo); // intercept set_param_by_name on x
      // need to hack c->scope()
    }
    return x;
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
