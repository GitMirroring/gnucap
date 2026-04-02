/*$Id: e_node.cc $ -*- C++ -*-
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
 * node probes
 */
//testing=script,sparse 2006.07.11
#include "m_union.h"
#include "u_nodemap.h"
#include "u_xprobe.h"
#include "e_cardlist.h"
#include "e_card.h"
#include "e_aux.h"
#include "e_logicnode.h"
#include "e_matrixnode.h"
#include "m_union.h"
#include "e_usernode.h" // BUG
#include "e_node_type.h" // DNDEBUG?
/*--------------------------------------------------------------------------*/
extern NODE electrical;
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 * supposedly not used, but used by a required function that is also not used
 */
NODE::NODE(const NODE* p)
  :CARD(*p)
{ untested();
  unreachable();
}
/*--------------------------------------------------------------------------*/
node_t::node_t(node_t& p)
  :_nnn(p._nnn),
   _link(p._link),
   _index(p._index),
   _m(p._m)
{
  if(_nnn){
    _nnn = nullptr;
    _link = &p;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
NODE::~NODE()
{
  if(subckt()){
    for(auto&i : *subckt()){
      i = nullptr;
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
node_t::node_t(const node_t& p)
  :_nnn(p._nnn),
   _link(p._link),
   _index(p._index),
   _m(p._m)
{
}
/*--------------------------------------------------------------------------*/
node_t::node_t(node_t&& p)
  :_nnn(p._nnn),
   _link(p._link),
   _index(p._index),
   _m(p._m)
{
  if(p._link == &p) { untested();
    _link = this;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
#ifndef NDEBUG
extern NODE ground_node;
#endif
node_t::node_t(NODE* n)
  :_nnn(n),
   _index(n->user_number()),
   _m(n->matrix_number())
{
  assert(n!=&ground_node || _m==0);
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(node_t& p)
{
  if(_own){
    delete _nnn;
  }else{
  }
  _nnn = nullptr;

  if(!p.n_()){
    _link = p._link;
  }else{
    _link = &p;
  }

  _index = p._index;// wrong scope ??
  _m   = p._m;
  _own = false;
  return *this;
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(const node_t& p)
{
  if(!p.n_()){
    return operator=(const_cast<node_t&>(p));
  }else{ untested();
    // assert(dynamic_cast<USER_NODE const*>(p.n_()));
    clear();
    // _link = nullptr;
    _dir = dir_none;
    _nnn = p._nnn;
    _own = false;
    assert(!p._link);
    return *this;
  }
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(node_t&& p)
{
  _nnn   = nullptr; // p._nnn;
  _link   = p._link;
  _index = p._index;
  _m   = p._m;
  _own = false;

  if(p._link == &p) { untested();
    _link = this;
  }else{
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
// ordinary pointer assignment
// but keep index (!)
node_t& node_t::operator=(NODE* n)
{
  assert(n);
  assert(!_link || _link == this);
  _dir = dir_none;
  // clear();
  if(!_nnn){
    _own = false;
  }else if(_own){ untested();
    _nnn->purge();
    delete _nnn;
    _own = false;
  }else{
  }
  _link = nullptr;
  _nnn = n;

  _index = n->user_number();
  return *this;
}
/*--------------------------------------------------------------------------*/
// take ownership
node_t& node_t::set_own(NODE* n)
{
  assert(n != &ground_node);
  operator=(n);
  _own = true; // take ownership.
  return *this;
}
/*--------------------------------------------------------------------------*/
extern NODE ground_node;
NODE& node_t::data()const
{
  if(_nnn) {
    return *_nnn;
  }else if(auto e = root()._nnn){
    return *e;
  }else{
    // why ground?
    return ground_node;
  }
  unreachable();
}
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(const std::string& x)const
{
  if (Umatch(x, "v ")) {
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_aa, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) {
    return NOT_VALID;
  }else if (Umatch(x, "mdy ")) {
    // matrix diagonal admittance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return aaa.solver()->d(m_());
  }else if (Umatch(x, "mdz ")) {
    // matrix diagonal impedance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return 1/aaa.d(m_());
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
  }else{itested();
    return CARD::tr_probe_num(x);
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
    return CARD::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
/* new_node: a raw new node, as when a netlist is parsed
 * It's only "new" if this is the first use in this scope.
 * If it is not the first use of this node, it makes a connection.
 */
void node_t::new_node(const std::string& node_name, const CARD* Owner)
{
#ifdef TRACE_UNTESTED
  if (dynamic_cast<NODE_TYPE const*>(_nnn)) { untested();
//  }else if (dynamic_cast<NODE_DECL const*>(_nnn)) { untested();
  }else if (dynamic_cast<USER_NODE const*>(_nnn)) { untested();
    // a port with a discipline associated with it.
  }else if (_nnn) {untested();
    // Repeat assign to this port, must be by name.  Probably an error.
    // Just clobber it.  Might be a leak but probably isn't.
  }else{//33312
    // proper first assign to this port.  The usual case.
  }
  if(_link){ untested();
  }else{
  }
#endif
  CARD_LIST const* scope; // the CARD_LIST that owns this device.
  if(Owner) {
    scope = Owner->scope();
  }else{ untested();
    scope = &CARD_LIST::card_list;
  }
  assert(scope);
  NODE_MAP* Map = scope->nodes();
  assert(Map);

  NODE* n = Map->new_node(node_name); // not neessarily "new"
  _index = n->user_number();
  assert(_index!=INVALID_NODE);

  n->set_owner(nullptr); // Owner?
}
/*--------------------------------------------------------------------------*/
// free memory. NODES are owned by roots in the union forest, but path
// contraction will make the others point directly to root()->_nnn
// only delete once.
node_t::~node_t()
{
  // clear();
  if(!_nnn){
    _own = false;
  }else if(_own){
    _nnn->purge();
    delete _nnn;
    _nnn = nullptr;
    _own = false;
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* new_model_node: a mapped new node, produced through model expansion.
 * Not really a model_node, but a node in the device that is made
 * in model expansion.
 * Supposedly equivalent to new_node() then map_subckt_node()
 * but it does it without building a map
 */
void node_t::new_model_node(const std::string& node_name, CARD* Owner)
{
  (void) node_name;
  (void) Owner;
  assert(!_nnn);
  // find_subset(this);
  _nnn = &electrical;
  _own = false;
  allocate(3);
}
/*--------------------------------------------------------------------------*/
/* (re)connect a port to an external node making use of index.
 * m: external nodes are in m. usually m == d->scope->nodes.
 * TODO: pass map, and range check..?
 */
void node_t::map_subckt_node(node_t* m, const CARD* d)
{
  if(!d){
    assert(m->n_() == &ground_node);
  }else{
  }
  assert(m);
  if (e_() != INVALID_NODE) {
    // clear(); // keep index.
    m[e_()].connect(*this);
    assert(_link);
    assert(!_nnn);
    if(!_nnn){
      _own = false;
    }else if(_own){ untested();
      delete _nnn;
      _own = false;
    }else{ untested();
    }
    _nnn = nullptr;
  }else{
    (void)d; // probably floating. handle elsewhere
  }

  if(!d){
    assert(m->n_() == &ground_node);
  }else{
  }
//  assert(_nnn || _link);
}
/*--------------------------------------------------------------------------*/
// nodes are all the same. only difference is counter
// 0: subckt, module, DEV_SUBCKT "subckt_node"
// 1: top level                  "user_node"
// 2: misc device internal nodes "model_node"
void node_t::allocate(int u /*, CARD* owner*/)
{
  assert(u!=2); // obsolete new_model_node supplementary call.

  if(_nnn == &ground_node){
  }else if(is_node() && CKT_BASE::_sim->is_first_expand()) {
    // repeat call.
  }else{
  }
  if(dynamic_cast<USER_NODE const*>(_nnn)) { untested();
    unreachable();
  }else if(auto dd = dynamic_cast<NODE_TYPE const*>(_nnn)) {
    int flat_number = INVALID_NODE;
    switch(u) {
    case 0:
      flat_number = CKT_BASE::_sim->newnode_subckt();
      break;
    case 1:
      flat_number = CKT_BASE::_sim->newnode_user();
      break;
    case 3:
      flat_number = CKT_BASE::_sim->newnode_model();
      break;
    default:
      unreachable();
    }
    CARD* ni = dd->allocate();
    auto nn = prechecked_cast<NODE*>(ni);
    assert(nn);
    nn->set_user_number(flat_number);
    nn->set_owner(nullptr);
    set_own(nn);
  }else if(is_node()) {
    // done.
    trace3("node_t::allocate is_node", this, &root(), _nnn->short_label());
  }else if(_link==this) { untested();
    unreachable();
  }else if(dynamic_cast<NODE_TYPE const*>(root().n_())){
 //    trace3("node_t::allocate disc allocate", _index, u, short_label());
 //    int flat_number = CKT_BASE::_sim->newnode_subckt();
 //    NODE* nn = new LOGIC_NODE(flat_number);
 //    clear();
 //    nn->set_owner(nullptr);
 //    set_own(nn);
  }else{
    trace3("node_t::allocate no allocate", _index, u, short_label());
  }
}
/*--------------------------------------------------------------------------*/
void node_t::set_to_ground(CARD* Owner)
{
  assert(!_link || _link == this || is_grounded());
  int idx = _index;
  clear();
  assert(!_nnn);
  if(Owner){
    assert(Owner->scope());
    assert(Owner->scope()->nodes());
    NODE_MAP& nodes = *Owner->scope()->nodes();
    if(nodes.size() && nodes[0].is_grounded()){itested();
      // use that, maybe a spice scope?
    }else{
      // there is no ground here. resort to global
      // (don't try to create one, makes no sense.)
      Owner = nullptr;
    }
  }else{
  }
  if(Owner){untested();
    // fallback
    // maybe reached from spice?
    assert(Owner->scope());
    assert(Owner->scope()->nodes());
    new_node("0", Owner);
    assert(_index==0);
    _m = 0;
  }else{
    NODE_MAP& nodes = *CARD_LIST::card_list.nodes();
    // kludge: "0" always exists at top level.
    _link = &nodes["0"]->n_(0);
    // must retain index. connection is in _link...
    assert(_index == idx);
  }
  _m = 0;
}
/*--------------------------------------------------------------------------*/
bool node_t::is_grounded() const
{
  if(_m==0){
    //assert(_nnn == &ground_node);
    return true;
  }else{
    //assert(_nnn != &ground_node);
  }

  if(&root()==this){
    return false;
  }else if(root().is_grounded()){
    return true;
  }else{itested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void node_t::clear()
{
  if(_nnn == &ground_node){
  }else{
  }
  if(!_nnn){
//  }else if(dynamic_cast<DISCIPLINE const*>(_nnn)){ untested();
//  }else if(_nnn == &ground_node){ untested();
  }else{
    if(_own){
      _nnn->purge();
      delete _nnn;
    }else{
    }
    _own = false;
    _nnn = nullptr;
  }

// _link = nullptr;
}
/*--------------------------------------------------------------------------*/
void node_t::connect_port(node_t& port)
{
  connect(port);
}
/*--------------------------------------------------------------------------*/
static NODE const* resolve_type(NODE const* upper, NODE const* lower)
{
  if(upper == lower){
    return upper;
  }else if(upper && lower && OPT::connect_rules){
    int u = upper->user_number();
    int l = lower->user_number();
    assert(u<OPT::connect_rules->net_nodes());
    assert(l<OPT::connect_rules->net_nodes());
    return (&OPT::connect_rules->n_(u))[l].n_();
  }else if(upper) { untested();
    return upper;
  }else{ untested();
    return lower;
  }
}
/*--------------------------------------------------------------------------*/
// make a connection to a node, usually further up the hierarchy.
// this will have to transport type information,
// negotiate with the target node, and flag it as used.
// next steps after connect
// - resolve target node type
// - expand/deflate target node
// - map to resulting structure
void node_t::connect(node_t& lower)
{
  assert(OPT::default_logic);
  node_t& target = lower;
  bool used = is_used() || target.is_used();

  if(!_nnn){
    // incomplete();
    set_type(&electrical);
  }else{
  }

  assert(root()._nnn == &ground_node
      || dynamic_cast<NODE_TYPE const*>(root()._nnn));

  node_t* old_root = &root();
  node_t* lower_root = &lower.root();

  NODE const* upper_type = old_root->n_();

  if(!old_root->_nnn){ untested();
    // too late?
    old_root->set_type(OPT::default_logic);
    assert(old_root->_nnn == OPT::default_logic);
    upper_type = OPT::default_logic;
  }else{
  }
  assert(!old_root->_link);

  if(upper_type == &ground_node){
    upper_type = &electrical; // HACK.
  }else{
  }

  assert(dynamic_cast<NODE_TYPE const*>(upper_type));

  if(!target.root()._nnn){
    // no discipline specified.
    // target.root().set_type(OPT::default_node);
  }else{
    assert(target.root()._nnn == &ground_node
	|| dynamic_cast<NODE_TYPE const*>(target.root()._nnn));
  }

  NODE const* lower_type = lower_root->n_();
  if(lower_type == &ground_node){
    // incomplete();
    lower_type = &electrical;
  }else if(lower_type){
  }else if(upper_type){
    lower_type = lower_root->set_type(upper_type);
  }else{ untested();
  }
  assert(lower_type);
  assert(dynamic_cast<NODE_TYPE const*>(lower_type));
  trace2("DBG connect", lower_type->short_label(), upper_type->short_label());

  assert(!_own);
  assert(!target._own);
  if(dynamic_cast<NODE_TYPE const*>(_nnn)){
    _nnn = nullptr;
    _link = this;
    _own = false;
  }else{
  }
  if(dynamic_cast<NODE_TYPE const*>(target._nnn)){
    target._nnn=nullptr;
    target._link = &target;
  }else{
  }
  node_t* u = build_union(&target, this);
  assert(u);
  node_t& r = *u;

  assert(_nnn || _link);
  assert(!_nnn || !_link);

  if(dynamic_cast<USER_NODE const*>(r._nnn)) { untested();
    // THIS IS WRONG.
    // replace by clone_instance?
    // r._link = nullptr;
    // r._nnn = &dummy;
  }else if(r._nnn && r._nnn == &ground_node){
    trace1("connect", typeid(*r._nnn).name());
  }else if(dynamic_cast<NODE_TYPE const*>(r._nnn)) {
  }else{
    assert(!r._nnn);
  }

  if(r._nnn == &ground_node){
    // HACK.
    r.set_used();
  }else if(used){
//  }else if(!r._nnn){ untested();
    assert(r._link == &root() || !r._link);
    r._link = nullptr;
    r.set_type(resolve_type(upper_type, lower_type));
    trace3("resolved", upper_type->short_label(), lower_type->short_label(), r.short_label());
    r.set_used();
  }else{
    r._link = nullptr;
  }

  assert(target._link);

  assert(!used
      || target.root()._nnn == &ground_node
      || dynamic_cast<NODE_TYPE const*>(target.root()._nnn));
}
/*--------------------------------------------------------------------------*/
NODE const* node_t::set_type(NODE const* d)
{
  _m = -1;
  assert(d);
  assert(!_own);
  if(_link == this){
    // incomplete(); // why?
    _link = nullptr;
  }else{
  }

  if(dynamic_cast<USER_NODE*>(_nnn)){ untested();
    _link = nullptr;
  }else{
  }

  if(_link){
    // unreachable();
    return nullptr;
  }else{
    _link = nullptr;
    _nnn = const_cast<NODE*>(d);
    assert(prechecked_cast<NODE_TYPE*>(_nnn));
    _dir = dir_none;
    _own = false;
    return _nnn;
  }
  return _nnn;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
