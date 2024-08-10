/*                 -*- C++ -*-
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
 * user nodes and elaboration
 */
/*--------------------------------------------------------------------------*/
#include "u_node.h"
#include "e_logicnode.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
// same as CARD,  but bump user node counters.
class TOPLEVEL : public CARD {
public:
  explicit TOPLEVEL() : CARD(){
    set_label("$root");
  }

public:
  LOGIC_NODE* new_logic_node(NODE const* proto)override {
    NODE* l = CARD::new_logic_node(proto);
    CKT_BASE::_sim->newnode_user(); // bump user node counter.
    return prechecked_cast<LOGIC_NODE*>(l);
  }
  NODE* new_matrix_node(NODE const* proto)override {
    NODE* l = CARD::new_matrix_node(proto);
    CKT_BASE::_sim->newnode_user(); // bump user node counter.
    return l;
  }
private:
  CARD*	 clone()const override {unreachable(); return NULL;}
  std::string value_name()const override {unreachable(); return ""; }

}owner_;
/*--------------------------------------------------------------------------*/
// NODEMAP::new_node
USER_NODE::USER_NODE(CARD const* n, int u) : NODE(n, u)
{
  _conn.set_node(this);
  trace1("USER_NODE::USER_NODE 1", this);
}
/*--------------------------------------------------------------------------*/
// various clone
USER_NODE::USER_NODE(USER_NODE const* proto) : NODE(proto)
{
  _conn.set_node(this);
  assert(proto);
  if(proto){
    if(proto->is_grounded()){
      set_ground();
    }else{
    }
    set_type(proto->type());
  }else{ untested();
  }
  assert(!_conn.is_none());
  trace1("USER_NODE::USER_NODE 2", this);
}
/*--------------------------------------------------------------------------*/
// deflate user node (expand, deflate? allocate? elaborate?)
// _pb will point to the actual node instance, owned by _pb.
// return pointer to _pb.n_()
NODE* USER_NODE::deflate(NODE_P*n, CARD* owner)
{
  assert(n);
  NODE_P& conn = n->root(); // const?
  if(n == &_conn){
  }else{
    assert(owner);
  }
  bool gnd = conn.is_grounded() || _conn.is_grounded()
         || is_grounded();

  int type = _conn.type();

  NODE* to = NULL;
  if(!owner) { // top level
    assert(!n->type()); // !
    trace3("USER_NODE::deflate top level1", short_label(), gnd, type);
    trace2("USER_NODE::deflate top level2", short_label(), _types.size());
    if(_types.count(12345)){
      type = 12345;
    }else{
    }
    trace3("USER_NODE::deflate top level3", short_label(), gnd, type);

    if(gnd){
      assert(is_grounded());
      GROUND_NODE* g = new GROUND_NODE(this);
      to = g;
      assert(is_grounded());
    }else if(type == 12345) {
      LOGIC_NODE* new_node = owner_.new_logic_node(this);
      assert(new_node);
      new_node->set_user_number(user_number()); // HACK
      assert(new_node->user_number() == user_number());
      to = new_node;
    }else if(type == 0){
      trace1("USER_NODE::deflate matrix", short_label());
      NODE* new_node = owner_.new_matrix_node(this);
      // assert(new_node->user_number() == user_number()); not yet
      to = new_node;
    }else{ // split mode
      trace1("USER_NODE::deflate split", short_label());
	   // place connect nodes on a split node.
	   // multiple types
      incomplete();
      return this;
    }
  }else{ // not top level
    if(_types.count(12345)){
      type = 12345;
    }else if(n->type() == 12345){
      type = 12345;
    }else{
    }
    trace3("USER_NODE::deflate0", owner->long_label(), n->type(), type);
    assert(n);

    if(1) {
      // if(p->n_()->more()){ untested();
      //   incomplete();
      // }else
      if(gnd || n->is_grounded()){
	to = new GROUND_NODE(this);
	// assert(to->short_label() == short_label());
      }else if(type == 12345){
	// getting here when a node is connected to a DEV_LOGIC or to a top level node
	NODE* l = owner->new_logic_node(this);
	to = l;
	l->set_owner(owner);
	assert(to->short_label() == short_label());
      }else{
	trace3("USER_NODE::deflate3", owner->long_label(), n->is_link(), n->is_connected());
	// the others are plain matrix nodes.
	NODE* l = owner->new_matrix_node(this);
	l->set_owner(owner);
	to = l;
	// assert(to->short_label() == short_label());
      }
      assert(to->user_number() == user_number());
      if(to->long_label() == owner->long_label()+"."+short_label()){
      }else{
	// incomplete();
      }
      // assert(user_number()>=0);
      trace3("USER_NODE::deflate", short_label(), owner->long_label(), to->flat_number());

    }else{ // split mode more incomplete.
	   // place connect nodes on a split node.
      incomplete();
      unreachable();
      return this;
    }
  } // not top level

  assert(!_conn.is_none());
  return to;
}
/*--------------------------------------------------------------------------*/
NODE* MODULE_NODE::deflate(NODE_P*n, CARD* owner)
{
  assert(owner);
  // trace1("MODULE_NODE::deflate", n->short_label());
  NODE* nn = USER_NODE::deflate(n, owner);

  return nn;
}
/*--------------------------------------------------------------------------*/
void USER_NODE::expand(CARD* owner)
{
 // trace3("USER_NODE::expand", short_label(), is_grounded(), _types.size());
  assert(!_conn.is_none());

  if(owner){
    trace2("USER_NODE::expand no top level", short_label(), is_grounded());
  }else if(!_conn.is_node()){
    trace2("USER_NODE::expand no node", short_label(), is_grounded());
    assert(_conn.is_link());

  }else if(_conn.n_() == this){
    // _conn.expand(NULL);
    _conn.set_node(deflate(&_conn, NULL));
    trace3("USER_NODE::expand", short_label(), is_grounded(), has_owner());
    assert(_conn.is_node());
  }else{
    trace2("USER_NODE::expand skip", short_label(), is_grounded());
    // nothing to do.
    // unreachable(); // re-run bug?
  }
  // unreachable();

#if 0
  NODE_PS r(&_conn);
  if(r.is_node()){ untested();
    trace1("USER_NODE::expand", long_label());
    if(dynamic_cast<USER_NODE*>(r.node())) { untested();
    }else if(dynamic_cast<MATRIX_NODE*>(r.node())) { untested();
    }else{ untested();
      unreachable();
    }
    // already expanded?
  }else{ untested();
    NODE_PS conn(&_conn); // const?
    trace2("USER_NODE::expand2", long_label(), conn.is_grounded());
   // map[i] = proto[i]->deflate(&map[i], owner);
   r = deflate(&_conn, NULL);

  }
#endif
}
/*--------------------------------------------------------------------------*/
void USER_NODE::merge(USER_NODE* a, USER_NODE* b)
{
  assert(b != a);
  // trace2("USER_NODE::merge2", a->short_label(), a->_conn.n_() == a);
  // trace2("USER_NODE::merge2", b->short_label(), b->_conn.n_() == b);
//  assert(a->_conn.is_node() || a->_conn.next()->is_node());
//  assert(b->_conn.is_node() || b->_conn.next()->is_node());

  USER_NODE* A = prechecked_cast<USER_NODE*>(&a->data());
  USER_NODE* B = prechecked_cast<USER_NODE*>(&b->data());

  // NODE_P::merge(b->_conn, _conn);
  NODE_P& new_root = NODE_P::merge(a->_conn, b->_conn);

  assert(a->_conn.operator->() == b->_conn.operator->());
  assert(a->_conn.root().is_node());
  //assert(a->_conn.is_node());
  //assert(b->_conn.is_node());
  //assert(a->_conn.n_() == b->_conn.n_());

  trace1("USER_NODE::merge2", a->_conn.operator->() == a);
  trace1("USER_NODE::merge2", b->_conn.operator->() == b);

  if(new_root.is_node()){
    NODE* n = new_root.n_();
    auto un = prechecked_cast<USER_NODE*>(n);

    if(A != n){
      A->_conn.set_io_link();
      A->_conn.set_next(&un->_conn);
    }else{
    }
    if(B != n){
      B->_conn.set_io_link();
      B->_conn.set_next(&un->_conn);
    }else{
    }

    trace1("USER_NODE::merge3", a->short_label());
    trace1("USER_NODE::merge3", b->short_label());
    trace1("USER_NODE::merge3", n->short_label());
  }else{
    unreachable(); // later?
  }
}
/*--------------------------------------------------------------------------*/
NODE& USER_NODE::merge(NODE_P*p)
{
  trace1("USER_NODE::merge1", short_label());
  p = &p->root();
  // *p = p->root();

  assert(p);
  trace2("USER_NODE::merge1", p->is_link(), p->is_node());
  if(p->is_link()){
    trace2("USER_NODE::merge1", p->next(), p);
  }else{
  }
  assert(p->is_root());
 // _conn.set_next(NODE_P::merge(_conn, *p));
  //return *this;

  if(&_conn == p){ untested();
    incomplete(); //?
    unreachable();
  }else{
  }

  // assert(_conn.is_node());
  //assert(_conn.n_() == this);

  if(p->is_node()){
    auto un = prechecked_cast<USER_NODE*>(p->n_());
    assert(un);
    if(un != this){
      merge(this, un);
    }else{ untested();
    }
    // return *this;
  }else if(p->is_link()) {
    trace2("merge4", p==p->next(), p->next());
    trace2("merge4", p->next()->is_node(), p->next()->is_link());
    trace1("merge4", p->next()->is_root());
    if(_conn.is_link()){ untested();
       unreachable();
      _conn = NODE_P::merge(_conn, *p);
    }else{
      trace1("merge5", _conn.is_node());
      auto un = prechecked_cast<USER_NODE*>(_conn.n_());
      assert(un == this);
      if(un == this){
	// un -> merge(p);
	_conn = NODE_P::merge(_conn, *p);
	p->set_io_link();
	p->set_next(&_conn);
      }else{ untested();
      }
      // unreachable(); // leafnet
     // p->merge(&_conn);
      //_conn = NODE_P::merge(_conn, *p);
    }
  }else{ untested();
    unreachable();
  }

 // CONST_NODE_PS c(&_conn);
  if(_conn.root().is_grounded()) {
    set_ground();
  }else{
  }
  assert(!_conn.is_none());

  return *this;
}
/*--------------------------------------------------------------------------*/
double USER_NODE::tr_probe_num(const std::string& x) const
{
 // if(_pb.is_node()){ untested();
 //   return _pb->tr_probe_num(x);
  if (_conn.is_link()){
    return _conn->tr_probe_num(x);
  }else if (Umatch(x, "l{ogic} |la{stchange} |fi{naltime} |di{ter} |ai{ter} |count ")) {
    if(auto l = dynamic_cast<LOGIC_NODE const*>(node())){
      return l->tr_probe_num(x);
    }else{
      return 0.;
    }
  }else{
    return NODE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
// -> .h?
bool USER_NODE::is_grounded() const
{
  if(_conn.is_link()){
    if(1|| _conn.operator->() != this){
      //CONST_NODE_PS c(&_conn);
      return _conn.root().is_grounded();
    }else{
      return _conn.is_ground();
    }
  }else if(_conn.n_() == this) {
    assert(_conn.is_node());
    return _conn.is_ground();
  }else{
    assert(_conn.is_node());
    // assert(_conn.is_ground == _conn.n_()->is_grounded()); // ?
    return _conn.n_()->is_grounded();
  }
}
/*--------------------------------------------------------------------------*/
int USER_NODE::type() const
{
 // CONST_NODE_PS c(&_conn);
  return _conn.root().type();
}
/*--------------------------------------------------------------------------*/
void USER_NODE::set_ground()
{
  trace2("USER_NODE::set_ground", this, short_label());
  assert(!_conn.is_none());
  if(_conn.is_none()){ untested();
    unreachable();
    incomplete();
  }else if(_conn.is_link()){ untested();
    //DE_PS(&_conn).set_to_ground(NULL);
    //_root->set_to_ground(d);
    _conn.root().set_to_ground(NULL);
  }else if(_conn.is_node()){
    _conn.set_ground();
  }else{ untested();
    _conn.set_io_link(); // really?
    _conn.set_to_ground(NULL);
    unreachable();
    incomplete();
  }
  assert(is_grounded());
  assert(!_conn.is_none());
}
/*--------------------------------------------------------------------------*/
void USER_NODE::set_type(int t)
{
  if(_conn.is_none()){ untested();
    incomplete();
  }else if(_conn.is_link()){ untested();
    incomplete();
   // NODE_PS(&_conn).set_type(t);
  }else if(_conn.is_node()){
    _conn.set_type(t);
    _types.insert(t);
  }else{ untested();
    _conn.set_type(t);
  }
  assert(!_conn.is_none());
}
/*--------------------------------------------------------------------------*/
double GROUND_NODE::tr_probe_num(const std::string& x) const
{
  if (Umatch(x, "v ")) {
    return 0.;
  }else{ untested();
    return NODE::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
// add is_grounded to NODE vtable? only needed for top level.
bool is_grounded(NODE const* n)
{
  auto un = prechecked_cast<USER_NODE const*>(n);
  assert(un);
  return un->is_grounded();
}
/*--------------------------------------------------------------------------*/
bool USER_NODE::is_short_to(NODE_P const& n) const
{
  if(is_grounded() && n.is_grounded()){
    // too many cases. fold?
    return true;
  }else if(!_conn.is_node()) { untested();
    assert(_conn.operator->() != this);
    assert(_conn.root().is_node());
    return _conn->is_short_to(n);
  }else if(_conn.is_link()) { untested();
    return _conn.root() == n.root();//
   // return CONST_NODE_PS(&_conn) == CONST_NODE_PS(&n);
  }else if(n.is_node()) {
    return _conn.root() == n.root();
    //CONST_NODE_PS(&_conn) == CONST_NODE_PS(&n);
  }else{ untested();
    unreachable();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
// return reference to sub node when splitting nodes.
// (not yet)
NODE* USER_NODE::select(int /*t*/)
{
  if (_conn.is_node()){
    return _conn.n_();
  }else{
    return _conn.operator->();
    return this;
  }
}
/*--------------------------------------------------------------------------*/
void USER_NODE::req_type(int t)
{
  _types.insert(t);
  incomplete();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
