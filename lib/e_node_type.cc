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
#include "globals.h"
/*--------------------------------------------------------------------------*/
NODE_TYPE::NODE_TYPE(std::string const& name)
  : NODE(name)
{ untested();
  set_label(name);
  static int k;
  _type_number = k++;
}
/*--------------------------------------------------------------------------*/
NODE_TYPE::~NODE_TYPE()
{
}
/*--------------------------------------------------------------------------*/
class ELECTRICAL : public NODE_TYPE {
public:
  explicit ELECTRICAL() : NODE_TYPE("electrical") {
    set_continuous();
    // set_potential("Voltage");
    // set_flow("Current");
  }
}electrical;
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class HYBRID : public NODE_TYPE {
public:
  explicit HYBRID() : NODE_TYPE("hybrid") { untested();
    OPT::default_logic = this;
    set_mixed();
    // set_potential("Voltage");
    // set_flow("Current");
  }
}hybrid;
/*--------------------------------------------------------------------------*/
class LOGIC : public NODE_TYPE {
public:
  explicit LOGIC() : NODE_TYPE("logic") {
    set_discrete();
  }
}logic;
/*--------------------------------------------------------------------------*/
class CONNECTRULES : public CARD {
  mutable node_t _n[9];
public:
  explicit CONNECTRULES() : CARD() { untested();

    assert(electrical.type_number()==0);
    assert(hybrid.type_number()==1);
    assert(logic.type_number()==2);

    OPT::connect_rules = this;
    _n[0+0*3] = &electrical;
    _n[0+1*3] = &hybrid;
    _n[0+2*3] = &hybrid;
    _n[1+0*3] = &hybrid;
    _n[1+1*3] = &hybrid;
    _n[1+2*3] = &hybrid;
    _n[2+0*3] = &hybrid;
    _n[2+1*3] = &hybrid;
    _n[2+2*3] = &logic;

    OPT::connect_rules = this;
  }
  CARD* clone()const override { untested();unreachable(); return nullptr;}
  int net_nodes()const override { untested();return 3;}
  node_t& n_(int i)const override { untested();assert(i<3); return _n[i*3];}
} p3;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
