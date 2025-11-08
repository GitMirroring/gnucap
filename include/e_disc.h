/*                -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 * discipline and nature
 */
#ifndef E_DISC_H
#define E_DISC_H
#include "globals.h"
#include "e_base.h"
#include "e_node.h"
#include "u_nodemap.h"
#include "e_usernode.h"
#include "d_dot.h"
/*--------------------------------------------------------------------------*/
enum domain_type {
  dom_default = 0,
  dom_continuous = 1,
  dom_discrete = 2
};
/*--------------------------------------------------------------------------*/
class NATURE : public CKT_BASE {
  std::list<std::pair<std::string, std::string> > _attribs;
  double abstol{0.};
};
/*--------------------------------------------------------------------------*/
class DISCIPLINES : public NODE_MAP {
};
/*--------------------------------------------------------------------------*/
class DISCIPLINE : public NODE {
  typedef DISPATCHER<DISCIPLINE>::INSTALL inst;
  domain_type _domain{dom_default};
  int _user_number{-1};
  std::string _potential;
  std::string _flow;
  inst* _installer{nullptr};
public:
  explicit DISCIPLINE(std::string const& name, std::string potential="", std::string flow="");
  ~DISCIPLINE();
  CARD* clone_instance()const override {
    return new NODE_DECL(short_label());
  }
  void set_continuous() {_domain = dom_continuous;}
  int param_count()const override { return 3; }
  using NODE::param_name;
  std::string param_name(int i)const override {
    switch(i){
    case 0: return "domain";
    case 1: return "potential";
    case 2: return "flow";
    default: unreachable(); return "???";
    }
  }
  std::string param_value(int i)const override {
    static std::string d[3] = {"", "continuous", "discrete"};
    switch(i){
    case 0: return d[_domain];
    case 1: return _potential;
    case 2: return _flow;
    default: unreachable(); return "???";
    }
  }
  bool param_is_printable(int i)const override {
    switch(i){
    case 0: return _domain;
    case 1: return _potential.size();
    case 2: return _flow.size();
    default: unreachable(); return false;
    }
  }
  int set_param_by_name(std::string n, std::string v)override {
    if(n=="domain"){
      if(v=="discrete"){
	_domain = dom_discrete;
      }else if(v=="continuous"){
	_domain = dom_continuous;
      }else{ untested();
	error(bWARNING, "invalid domain " + v);
	_domain = dom_default;
      }
      return 0;
    }else if(n=="potential"){
      _potential = v;
      return 1;
    }else if(n=="flow"){
      _flow = v;
      return 2;
    }else{
      return NODE::set_param_by_name(n, v) + 3;
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
