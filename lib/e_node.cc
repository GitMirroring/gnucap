/*$Id: e_node.cc 2018/05/27  al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis,
 *               2020 Felix Salfelder
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
 * node probes
 */
//testing=script,sparse 2006.07.11
#include "u_nodemap.h"
#include "d_logic.h"
#include "e_aux.h"
#include "u_xprobe.h"
/*--------------------------------------------------------------------------*/
/* default constructor : unconnected, don't use
 */
NODE::NODE()
  :CARD(), NODE_DATA(INVALID_NODE),
   _flat_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
/* copy constructor : user data only
 */
NODE::NODE(const NODE& p)
  :CARD(p), NODE_DATA(p),
   _flat_number(p._flat_number)
{
}
/*--------------------------------------------------------------------------*/
CARD* NODE::clone() const{ unreachable(); return NULL;}
CARD* NODE::new_card()
{
  LOGIC_NODE* n = new LOGIC_NODE();
  trace2("new card", _flat_number, user_number());
  n->set_flat_number(flat_number());
  n->set_user_number(user_number());
  n->_n[0] = this;
  assert(n->_n[0].n_());
  assert(n->_n[0].n_()==this);
  _nnn = n;
  return n;
}
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 * used in NODE_MAP::clone
 */
NODE::NODE(const NODE* p)
  :CARD(*p), NODE_DATA(*p),
   _flat_number(p->_flat_number)
{
  trace3("node from ptr", p->long_label(), p->user_number(), _flat_number);
}
/*--------------------------------------------------------------------------*/
/* usual initializing constructor : name and index
 */
NODE::NODE(const std::string& s, int n)
  :CARD(), NODE_DATA(n),
   _flat_number(NOT_VALID)
{
  if(n){
    unreachable();
  }else{
    set_flat_number(0);
  }
  set_label(s);
}
/*--------------------------------------------------------------------------*/
node_t::node_t()
  :_nnn(0),
   _m(INVALID_NODE)
{
}
node_t::node_t(const node_t& p)
  :_nnn(p._nnn),
   _m(p._m)
{
}
node_t::node_t(NODE* n)
  :_nnn(n),
   _m(to_internal(n->flat_number()))
{
}
node_t& node_t::operator=(NODE* p)
{
  assert(!_nnn);
  assert(p);
  _nnn = p;
  return *this;
}
node_t& node_t::operator=(const node_t& p)
{
  if (p._nnn) {
  }else{
    assert(p._m   == INVALID_NODE);
  }
  _nnn   = p._nnn;
  _m   = p._m;
  return *this;
}
/*--------------------------------------------------------------------------*/
bool node_t::operator==(node_t const& p) const
{
  incomplete();
  if(!_nnn || !p._nnn){ untested();
    return false;
//  }else if(_nnn->flat_number() !=  p._nnn->flat_number()){ untested();
//    return false;
  }else if(_m!=p._m){ untested();
    return false;
  }else{
    return true;
  }
}
/*--------------------------------------------------------------------------*/
NODE_DATA& node_t::data()const
{
  assert(_nnn);
  return(*_nnn);
}
/*--------------------------------------------------------------------------*/
double NODE_CARD::tr_probe_num(const std::string& x)const
{ untested();
  if (Umatch(x, "v ")) { untested();
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_lu, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) {
    assert(_nnn);
    return _nnn->tr_probe_num(x);
  }else if (Umatch(x, "mdy ")) {
    // matrix diagonal admittance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return aaa.d(m_(),m_());
  }else if (Umatch(x, "mdz ")) {
    // matrix diagonal impedance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return 1/aaa.d(m_(),m_());
  }else if (Umatch(x, "zero ")) {
    // fake probe: 0.0
    return 0.0;
  }else if (Umatch(x, "pdz ")) {
    // fake probe 1/0 .. positive divide by zero = Infinity
    double z1 = tr_probe_num("zero ");
    return 1.0/z1;
  }else if (Umatch(x, "ndz ")) {
    // fake probe -1/0 .. negative divide by zero = -Infinity
    double z1 = tr_probe_num("zero ");
    return -1.0/z1;
  }else if (Umatch(x, "nan ")) {
    // fake probe 0/0 = NaN
    double z1 = tr_probe_num("zero ");
    double z2 = tr_probe_num("zero ");
    return z1/z2;
  }else{untested();
    return CKT_BASE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
XPROBE NODE::ac_probe_ext(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    return XPROBE(vac());
  }else if (Umatch(x, "z ")) {
    return XPROBE(port_impedance(node_t(const_cast<NODE*>(this)),
				 node_t(&ground_node), _sim->_acx, COMPLEX(0.)));
  }else{untested();
    return CKT_BASE::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
void node_t::set_to_ground(CARD* d)
{
  if(_nnn){
    incomplete(); //BUG// fails on MUTUAL_L::expand after clone
  }else{
  }
  assert(d);

  assert(d->scope());
  NODE_MAP* Map = d->scope()->nodes();
  assert(Map);
  NODE* nn = (*Map)["0"];
  _nnn = nn;
  assert(_nnn);
  assert(nn->flat_number()==0);
  // assert(nn->user_number()==0);
}
/*--------------------------------------------------------------------------*/
/* new_node: a raw new node, as when a netlist is parsed
 */
void node_t::new_node(const std::string& node_name, const CARD* d)
{
  //  assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone
  //                         and in lang_spectre.error.4.gc
  //
  assert(d);

  trace1("new_node", d->long_label());
  assert(d->scope());
  NODE_MAP* Map = d->scope()->nodes();
  assert(Map);
  _nnn = Map->new_node(node_name);
  assert(_nnn);
}
/*--------------------------------------------------------------------------*/
/* new_model_node: a mapped new node, produced through model expansion.
 * Not really a model_node, but a node in the subckt that is made
 * in model expansion.
 * Supposedly equivalent to new_node() then map_subckt_node()
 * but it does it without building a map
 */
void node_t::new_model_node(const std::string& node_name, CARD* d)
{
  // do this in NODE::expand?
  new_node(node_name, d);
  int ttt = CKT_BASE::_sim->newnode_model();
  assert(_nnn);
  NODE* nn = prechecked_cast<NODE*>(_nnn);
  assert(nn);
  trace2("new_model_node", d->long_label(), ttt);
  nn->set_flat_number(ttt);
}
/*--------------------------------------------------------------------------*/
bool node_t::node_is_valid(NODE const* i)
{
  if(i == &ground_node){ untested();
    return true;
  }else if(!i){
    return false;
  }else if (node_is_valid(i->flat_number())) {
    return true;
  }else{ untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
// after shallow copying a subckt in a subckt component
// connect sub device port to node in subckt d
void node_t::map_subckt_node(NODE** m, const CARD* d)
{
  assert(m);
  if (e_() != INVALID_NODE) {
    if (node_is_valid(m[e_()])) {
      _nnn = m[e_()];
    }else{
      trace3("not valid?", e_(), d->long_label(), m[e_()]->long_label());
      throw Exception(d->long_label() + ": need more nodes");
    }
  }else{untested();
    throw Exception(d->long_label() + ": invalid nodes");
  }
  assert(_nnn);
//  _nnn->set_flat_number(_ttt);
  NODE* nn = prechecked_cast<NODE*>(_nnn);
  assert(node_is_valid(nn));
}
/*--------------------------------------------------------------------------*/
// tmp hack
NODE_CARD gln("ground", 0);
NODE_DATA* NODE::data()
{ 
  gln.set_flat_number(0);
  if(_nnn==NULL){ untested();
    incomplete();
    return &gln;
  }else if(this==&ground_node){ untested();
    return &gln;
  }else{ untested();
  }
  assert(_nnn); return _nnn;
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
