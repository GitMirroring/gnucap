/*$Id: e_subckt.cc                  -*- C++ -*-
 * Copyright (C) 2001 Albert Davis,
 *               2019 Felix Salfelder
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
 * Base class for subcircuits in the circuit description file
 */
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::new_subckt()
{
  assert(!_subckt);
  delete _subckt;
  _subckt = NULL;
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::new_subckt(const CARD* Model, PARAM_LIST* Params)
{
  delete _subckt;
  _subckt = NULL;
  _subckt = new CARD_LIST(Model, this, scope(), Params);
  _subckt->map_subckt_nodes(Model, this);
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::renew_subckt(const CARD* Model, PARAM_LIST* Params)
{
  if (_sim->is_first_expand()) {
    new_subckt(Model, Params);
  }else{untested();
    assert(subckt());
    subckt()->attach_params(Params, scope());
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST const* CARD::subckt() const
{
  unreachable(); // this is a compatibility hack, used in some spice devices
  BASE_SUBCKT const* s=dynamic_cast<BASE_SUBCKT const*>(this);
  if(s){
    return s->subckt();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::subckt()
{
  unreachable(); // this is a compatibility hack
  BASE_SUBCKT* s=dynamic_cast<BASE_SUBCKT*>(this);
  if(s){
    return s->subckt();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::set_port_by_index(int Index, std::string& Value){ untested();
  incomplete(); // ports are in subckt->map
  return;
  assert(_ports.size()==size_t(Index)); // sequential assignment only
  node_t n;
  if(Value=="0"){
    n = new /*PORT_*/NODE(Value, 0);
  }else{
    n = new /*PORT_*/NODE(Value, Index+1);
  }
  _ports.push_back(n);
}
/*--------------------------------------------------------------------------*/
COMMON_SUBCKT::COMMON_SUBCKT(int c)
  : COMMON_PARAMLIST(c),
    _subckt(NULL)
{untested();
  trace1("COMMON_SUBCKT", this);
//  _subckt = std::make_shared<const CARD_LIST>();
}
/*--------------------------------------------------------------------------*/
std::string COMMON_SUBCKT::port_name(int i) const
{
  if (i<net_nodes()){
    NODE const* nn = prechecked_cast<NODE const*>(_ports[i].n_());
    assert(nn);
    return nn->short_label();
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
int COMMON_SUBCKT::net_nodes() const
{
  return int(_ports.size());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
