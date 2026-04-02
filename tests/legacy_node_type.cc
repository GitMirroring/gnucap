/*                -*- C++ -*-
 * Copyright (C) 2026 Felix Salfelder
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
#include "e_node_type.h"
#include "legacy_logicnode.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
NODE_TYPE* electrical_(); // TODO
namespace{
/*--------------------------------------------------------------------------*/
class LEGACY : public NODE_TYPE {
public:
  explicit LEGACY() : NODE_TYPE("legacy") {
    OPT::default_logic = this;
  }
  NODE* allocate()const override {
    return new LOGIC_NODE;
  }
}legacy;
/*--------------------------------------------------------------------------*/
class CONNECTRULES : public CARD {
  static const int _count{4};
  mutable node_t _n[_count*_count];
public:
  explicit CONNECTRULES() : CARD() {
    NODE* electrical = electrical_(); // node_dispatcher["electrical"];
    assert(electrical);
    int e = electrical->flat_number();
    assert(e==0);
//    assert(hybrid.type_number()==1);
//    assert(logic.type_number()==2);
    assert(legacy.type_number()==3);

    _n[e+e*_count] = electrical;
    _n[e+1*_count] = &legacy;
    _n[e+2*_count] = &legacy;
    _n[e+3*_count] = &legacy;
    // [..]
    _n[3+e*_count] = &legacy;
    _n[3+1*_count] = &legacy;
    _n[3+2*_count] = &legacy;
    _n[3+3*_count] = &legacy;

    OPT::connect_rules = this;
  }
  CARD* clone()const override { untested();unreachable(); return nullptr;}
  int net_nodes()const override {return _count;}
  node_t& n_(int i)const override {assert(i<_count); return _n[i*_count];}
} p3;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
