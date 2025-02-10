/*                 -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2024 Felix Salfelder
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
#include "e_cardlist.h"
#include "e_card.h"
#include "u_nodemap.h"
#include "e_aux.h"
#include "u_xprobe.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
/* default constructor : unconnected, don't use
 */
NODE::NODE()
  :CARD(),
   _user_number(INVALID_NODE)
   //_flat_number(INVALID_NODE)
   //_matrix_number(INVALID_NODE)
{
}
/*--------------------------------------------------------------------------*/
/* copy constructor : user data only
 */
NODE::NODE(const NODE& p)
  :CARD(p),
   _user_number(p._user_number)
   //_flat_number(p._flat_number)
   //_matrix_number(INVALID_NODE)
{ untested();
  unreachable();
}
/*--------------------------------------------------------------------------*/
/* constructor taking a pointer : it must be valid
 */
NODE::NODE(const NODE* p)
  :CARD(*p),
{
  assert(p);
  if(p){
    _user_number = p->_user_number;
  }else{ untested();
    // hmm anonymous matrix node.
    _user_number = -1;
  }
}
/*--------------------------------------------------------------------------*/
/* usual initializing constructor : name
 */
NODE::NODE(CARD const* n, int u) :
  CKT_BASE(), _owner(n), _user_number(u)
{
 // set_label(s);
}
/*--------------------------------------------------------------------------*/
node_t::node_t()
  :NODE_P(),
   // _ttt(INVALID_NODE),
   _m(INVALID_NODE)
{
}
node_t::node_t(const node_t& p)
  :NODE_P(p, 0),
   // _ttt(p._ttt),
   _m(p._m)
{ untested();
  // used when initialising node arrays for set_parameters.
  //assert(_ttt == _nnn->flat_number());
}
/*--------------------------------------------------------------------------*/
node_t::node_t(NODE* n)
  :NODE_P(n) , _m(n->matrix_number())
{
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(const node_t& p)
{
  if (p.is_node()) {
  }else{
    assert(p._m   == INVALID_NODE);
  }

  NODE_P::operator=(p);
  _m = p._m;
  return *this;
}
/*--------------------------------------------------------------------------*/
node_t& node_t::operator=(const NODE_P& p)
{
  assert(CARD::_sim->_nstat);
  return CARD::_sim->_nstat[m_()];
  if (p.is_node()) {
    //assert(p._ttt == p._nnn->flat_number());
    // needs nm? _m = p->matrix_number();
  }else{
//    assert(p._ttt == INVALID_NODE);
//    assert(p._m   == INVALID_NODE);
  }
  // assert (!p.is_link());
  NODE_P::operator=(p); // _nnn   = p._nnn;
  // incomplete(); // fetch from _nnn?
//  _ttt = p._ttt;
//  _m   = p._m;
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
double NODE::tr_probe_num(const std::string& x)const
{
  static GROUND_NODE ground_node(NULL, -1);
  if (Umatch(x, "v ")) {
    // return v0(); denoised
    return floor(v0()/OPT::vfloor + .5) * OPT::vfloor;
  }else if (Umatch(x, "z ")) {
    return port_impedance(node_t(const_cast<NODE*>(this)), node_t(&ground_node), _sim->_aa, 0.);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) { untested();
    incomplete();
//    assert(_sim->_nstat);
//    return _sim->_nstat[matrix_number()].tr_probe_num(x);
    return 0.;
  }else if (Umatch(x, "mdy ")) {
    // matrix diagonal admittance
    const BSMATRIX<double>&  aaa = _sim->_aa;
    return aaa.d(m_());
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
  static GROUND_NODE ground_node(NULL, -1);
  if (Umatch(x, "v ")) {
    return XPROBE(vac());
  }else if (Umatch(x, "z ")) { untested();
    return XPROBE(port_impedance(node_t(const_cast<NODE*>(this)),
				 node_t(&ground_node), _sim->_acx, COMPLEX(0.)));
  }else{untested();
    return CARD::ac_probe_ext(x);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
NODE_P::~NODE_P()
{
  // BUG, cant cast here.
//   if(_nnn == &ground_node) { untested();
//   }else if(dynamic_cast<USER_NODE const*>(_nnn)) { untested();
//     delete _nnn;
//   }else{ untested();
//   }
  if(is_own_node()){
    // trace1("delete node", n_()->long_label());

    assert(n_());
    delete n_();
  }else{
  }
  // set_node(NULL);
}
/*--------------------------------------------------------------------------*/
void NODE_P::set_ground()
{
  if(is_node()){
  }else{
  }
  NODE_P_BITS::set_ground();
}
/*--------------------------------------------------------------------------*/
void NODE_P::set_to_ground(CARD const* d)
{
  //assert(!_nnn); //BUG// fails on MUTUAL_L::expand after clone

  if(is_link()){ untested();
    root().set_to_ground(d);
    assert(is_grounded());
  }else if(is_none()){
    // naked ground. formerly taken from d->scope()->nodes();
    // but it's not there anymore.
    static GROUND_NODE ground_node(NULL, -1);
    set_node(&ground_node);
  }else if(!is_node()){ untested();
    set_ground();
    assert(is_grounded());
  }else if(auto n = dynamic_cast<USER_NODE*>(n_())){
    if(d){
    }else{ untested();
      // top level?
    }
    n->set_ground();
  }else if(n_()){ untested();
    unreachable();
  }else{ untested();
    // unreachable();
    // _nnn = &ground_node;
    set_ground();
  }
}
/*--------------------------------------------------------------------------*/
/* new_node: a mapped node, as when a netlist is parsed
 */
// former node_t::new_node
/* new_node: a raw new node, as when a netlist is parsed
 * It's only "new" if this is the first use in this scope.
 * If it is not the first use of this node, it makes a connection.
 */
void node_t::new_node(const std::string& node_name, const CARD* Owner)
{
  if (_nnn) {//206
    // Repeat assign to this port, must be by name.  Probably an error.
    // Just clobber it.  Might be a leak but probably isn't.
  }else{//33312
    // proper first assign to this port.  The usual case.
  }
  assert(Owner); // the device that owns this port.
  assert(Owner->scope()); // the CARD_LIST that owns this device.
  NODE_MAP* Map = Owner->scope()->nodes();
  assert(Map);
  // was
  //   _nnn = Map->new_node(node_name);
  //   _ttt = _nnn->user_number();
  USER_NODE* n = Map->new_node(node_name, d);
  assert(n->link().is_node());
  set_node(n);

  if(is_grounded()){
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* new_model_node: a mapped new node, produced through model expansion.
 * a node that is made in model expansion.
 * Supposedly equivalent to new_node() then map_subckt_node()
 * but it does it without building a map
 *
 * it's always electrical, immediately assign flat matrix number
 */
void node_t::new_model_node(const std::string& node_name, CARD* d)
{
  trace4("new_model node", node_name, d->short_label(), is_link(), this);
  CARD_LIST* cl = d->subckt();
 // if(is_none()) { untested();
 //   assert(!n_() || dynamic_cast<MATRIX_NODE const*>(n_()));
 //   delete n_();
 //   set_own(d->new_matrix_node(NULL));
 // }else
  if(cl) {
    if(is_none()) { untested();
      assert(!is_number());
      // create anonymous node
      incomplete();
    }else if(is_node()) { untested();
      assert(dynamic_cast<USER_NODE const*>(n_()));
    }else{
      incomplete();
      // create dynamic named mapped node..
    }

    trace3("new_model_node", short_label(), this, user_number());
#if 0
    set_io_link();
    set_next(this); // indicate that it's used?
#else
    assert(is_number());

    auto map = cl->nodes()->map();
    int u = user_number();
    set_io_link();
    set_next(&map[u]);
    if(map[u].is_link()){ untested();
      map[u].set_next(&map[u]);
    }else{
      map[u].set_io_link();
    }
#endif
  }else{
    // getting here when creating internal nodes in flat devices
#if 0
    // legacy: put a mapped node into parent scope
    new_node(node_name, d);
    _ttt = CKT_BASE::_sim->newnode_model();
#else
    // don't tell parent scope about it.
    assert(!n_() || dynamic_cast<MATRIX_NODE const*>(n_()));
    delete n_();
    set_own(d->new_matrix_node(NULL));
#endif
  }

  return *this;
}
/*--------------------------------------------------------------------------*/
NODE_P& NODE_P::set_to(NODE_P& n, CARD* c)
{
  trace4("set_to", c->short_label(), is_node(), is_link(), is_none());
  trace4("set_to", c->short_label(), n.is_node(), n.is_link(), n.is_none());

  operator=(n);
  return *this;

  if(is_none()) { untested();
    set_io_link();
  }else{ untested();
  }
  set_next(&n);
  return *this;
}
/*--------------------------------------------------------------------------*/
void NODE_P::req_type(int t)
{
  // simplify. always goto root?
  if(t == type()){
  }else if(is_node()){
    // probably top level?
    auto u = prechecked_cast<USER_NODE*>(n_());
    assert(u);
    set_type(t);
    u->req_type(t);
  }else if(is_link()){
    if(root().is_node()){ untested();
      auto u = prechecked_cast<USER_NODE*>(root().n_());
      assert(u);
      set_type(t);
      u->req_type(t);

    }else{
    }

    if(!type()){
      set_type(t);
    }else if(type() == t){ untested();
    }else{ untested();
      incomplete();

    }
  }else{ untested();
    unreachable();
  }
}
/*--------------------------------------------------------------------------*/
void NODE_P::map_subckt_node(NODE_P* m, CARD const* d)
{
  //assert(is_node()); // re_run?
  //assert(!is_own_node());
  int u = data().user_number();
  if(u>=0){
    trace3("msn", type(), u, m[u].type());
  }else{ untested();
    trace1("msn", type());
  }

  assert(d || m[u].is_node());
  m[u].req_type(type());

  if(!d && is_node()){
    // assert(n_() == m[u].n_());
    // re-run? clear type?
    set_node(m[u].n_());
  }else if(!d && is_link() && next()->is_node()) { untested();
    if(next()->n_() == m[u].n_()){ untested();
      incomplete(); // ?
    }else{ untested();
      incomplete(); // ?
      unreachable();
    }
    // top level
    incomplete(); // ?
    
  }else if(u < 0) { untested();
    unreachable();
    throw Exception((d?d->long_label()+": ":"") + "need more nodes");
  }else if (m[u].is_link()) {
    assert(u >= 0);
    assert(!is_link());
    set_io_link();
    assert(is_link());
    // *this = m[u] ?
    set_next(&m[u]);
  }else if(m[u].is_node()) { untested();
    auto un = prechecked_cast<USER_NODE*>(m[u].n_());
    assert(un);
    *this = m[u];
  }else if(n_()){ untested();
    assert(d);
    throw Exception(d->long_label() + ": need more nodes");
    incomplete(); // modelgen?
		  unreachable(); // d_subckt.error3.ckt?
  }else if(d){ untested();
    throw Exception(d->long_label() + ": need more nodes");
  }else{ untested();
    unreachable();
    // top level?
    incomplete();
    throw Exception("need more nodes");
  }
}
/*--------------------------------------------------------------------------*/
// just call n->expand(owner)?
void NODE_P::expand(CARD* owner)
{
  if(is_node()) {
 //   n_()->expand(); // (_conn, NULL);
  }else if(is_link()) { untested();
  }else{ untested();
    unreachable();
  }

  NODE_P& r = root();
  assert(r.is_node());
  r.n_()->expand(owner);
}
/*--------------------------------------------------------------------------*/
NODE_P& NODE_P::map()
{
  if(!is_link()){
  }else if(this == next()){ untested();
    incomplete();
  }else{
    trace2("NODE_P::map", this, next());
    NODE_P& t = root();
    if(t.is_node()){
      set_node(t.n_());
    }else{ untested();
      // assert(t.is_link());
     // assert(t.next() == t);?
    }
  }


  if(is_node()){
    auto un = prechecked_cast<USER_NODE*>(n_());
    if(un){
     //  int user_number = n_()->user_number();
      // incomplete();
      trace1("map", type());
      // set_node(un->select(type()));
     //  assert(user_number == n_()->user_number());
    }else{
    }
  }else{ untested();
  }

  return *this;
}
/*--------------------------------------------------------------------------*/
void NODE_P::deflate(CARD* owner)
{ untested();
  if(!owner){ untested();
    unreachable();
    assert(!is_own_node());
    // need to retain user node.
  }else{ untested();
    unreachable();
  }

  if(type()){ untested();
    assert(!type());
    incomplete();
    return;
  }else{ untested();
  }

  assert(n_());
  NODE* nn = n_()->deflate(this /*??*/, owner);

  if(!owner){ untested();
    auto un = prechecked_cast<USER_NODE*>(n_());
    assert(un);
    assert(nn!=un);
    unreachable();
    // un->set(nn);
  }else{ untested();
  }

  if(is_own_node()){ untested();
    if(!owner){ untested();
      assert(dynamic_cast<USER_NODE const*>(n_()));

    }else if(nn != n_()) { untested();
      // BUG. has references. why?!
      incomplete();
      delete n_();
      set_own(nn);
    }else{ untested();
    }

    assert(is_own_node());
  }else{ untested();
    incomplete();
    unreachable();
    // set_ref(nn);
  }

  assert(n_());
  trace2("deflated", n_()->flat_number(), n_()->short_label());
  trace1("deflated", n_()->is_grounded());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void GROUND_NODE::connect(NODE_P*to)
{ untested();
  USER_NODE::connect(to);

  if(auto n = dynamic_cast<USER_NODE*>(to->n_())){ untested();
    n->set_ground();
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
NODE_P& NODE_P::merge(NODE_P* o)
{ untested();
  assert(o);
  // trace2("merge top", short_label(), o->short_label());
  *this = root(); // needed to see if it's a USER_NODE
  if(is_node()){ untested();
    trace3("merge top node", short_label(), o->is_node(), o->is_link()?o->next():NULL);
    auto u = dynamic_cast<USER_NODE*>(n_());
    assert(u);

    // call USER_NODE::merge if needed.
    u->merge(o);
    if(u->is_grounded()){ untested();
    //  set_ground();
      assert(is_grounded());
    }else{ untested();
    }
  }else{ untested();
    trace1("merge top link", o->is_node());
    assert(is_link());
    assert(o);
    *o = o->root();

    if(o->is_node()){ untested();
      auto u = dynamic_cast<USER_NODE*>(o->n_());
      assert(u);
      u->merge(this);

    }else{ untested();
      merge(*this, *o);
    }

  }

  // trace2("merge top done", short_label(), o->short_label());
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
NODE* NODE::deflate(NODE_P*, CARD*)
{ untested();
  unreachable();
  // re-run.
  incomplete();
  return this;
}
/*--------------------------------------------------------------------------*/
std::string const& short_label(NODE const* n)
{
  assert(n);
  return n->short_label();
}
/*--------------------------------------------------------------------------*/
int user_number(NODE const* n)
{
  assert(n);
  return n->user_number();
}
/*--------------------------------------------------------------------------*/
node_t& node_t::map()
{
  NODE_P::map();
  if(is_node()) { // is connected?
    assert(n_());
    _m = n_()->matrix_number(); // leave it to NODE
  }else{ untested();
    // unreachable();
    // not used.
  }
			    
#if 0
  if (dynamic_cast<GROUND_NODE const*>(n_())){ // BUG.
    _m = 0;
  }else if (t_() != INVALID_NODE) { untested();
    assert(n_());
    _m=to_internal(t_());
  }else{ untested();
    assert(_m == INVALID_NODE);
  }
#endif
  return *this;
} // ELEMENT::map_nodes etc.
/*--------------------------------------------------------------------------*/
bool NODE::is_short_to(NODE_P const& n)const
{ untested();
  return n.operator->() == this;
}
/*--------------------------------------------------------------------------*/
bool NODE_P::is_short_to(NODE_P const& n) const
{
  // return find_root() == n.find_root()

  if(is_node()) {
    assert(n_());
    // basically n_() == n.n_(), but
    // at top level, *n_() may be a proxy
    auto un = prechecked_cast<USER_NODE const*>(n_());
    assert(un);
    return un->is_short_to(n);
    return n_()->is_short_to(n);
  }else if(is_grounded() && n.is_grounded()) {
    // multiple ground nodes...?
    return true;
  }else if(is_link() && n.is_link()) {
    return root() == n.root();
    //CONST_NODE_PS a(this);
    //CONST_NODE_PS b(&n);
    //return a == b;
  }else{ untested();
    incomplete(); // err on the safe sde.
    unreachable();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
std::string const& NODE::short_label()const
{
  if(_user_number<0) { untested();
    static std::string unknown = "noname";
    return unknown; // possible unnamed "model_node" in an ELEMENT
  }else if(_owner) {
    // prechecked_cast<SUBCKT_BASE const*>(_owner) ?
    // CARD_LIST const* cl = _owner->scope();
    CARD_LIST const* cl = _owner->subckt();
    if(cl){
    }else{ untested();
      cl = &CARD_LIST::card_list;
    }
    NODE_MAP const* nm = cl->nodes();
    assert(nm);
    return nm->label(_user_number);
  }else{
    // assume top level. for now.
    NODE_MAP const* nm = CARD_LIST::card_list.nodes();
    assert(nm);
    return nm->label(_user_number);
  }
  static std::string unknown = "???";
  unreachable();
  return unknown;
}
/*--------------------------------------------------------------------------*/
std::string NODE::long_label()const
{
  if(_owner) {
    return _owner->long_label() + "." + short_label();
  }else{
    return short_label();
    return "$root." + short_label();
  }
}
/*--------------------------------------------------------------------------*/
int MATRIX_NODE::flat_number() const
{
  return _flat_number;
}
/*--------------------------------------------------------------------------*/
int MATRIX_NODE::matrix_number() const
{
  assert(flat_number()>=0);
  trace3("matrix_number", flat_number(), user_number(), _sim->_nm[flat_number()]);
  assert(flat_number() < _sim->matrix_nodes());
  // assert(_sim->_nm[flat_number()] < _sim->matrix_nodes());
  return _sim->_nm[flat_number()];
}
/*--------------------------------------------------------------------------*/
bool is_grounded(NODE const*); // u_node.cc
/*--------------------------------------------------------------------------*/
bool NODE_P::is_grounded() const
{
  // node_t::is_grounded()const {return (e_() == 0);}
  if(is_ground()){
    return true;
  }else if(is_node()){
    // could be a USER_NODE..
    return n_()->is_grounded();
  }else if(!is_link()) {
    return false;
  }else if(next()!=this) {
    return root().is_grounded();
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
