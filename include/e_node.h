/*                -*- C++ -*-
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
 * circuit node class
 */
#ifndef E_NODE_H
#define E_NODE_H
#include "u_sim_data.h"
#include "e_card.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
enum {
  OUT1 = 0,
  OUT2 = 1,
  IN1 = 2,
  IN2 = 3,
  NODES_PER_BRANCH = 4,
  INVALID_NODE = -1
};
/*--------------------------------------------------------------------------*/
class NODE : public CARD {
private:
  // meets short,int,bool (7 bytes)
  int	_user_number;
  //int	_flat_number;
  //int	_matrix_number;
protected:
  explicit NODE(CARD const*, int u);
private:
  explicit NODE(const NODE& p); // u_nodemap.cc:49 (deep copy)
protected:
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  // explicit NODE(const std::string& s);
public:
  ~NODE() {}

  CARD* clone()const override	{untested(); return new NODE(*this);}

public: // label
  std::string const& short_label()const final override;
  std::string const  long_label()const final override;
  void set_user_number(int i) {_user_number = i;}
  void set_owner(CARD const* c) {
    assert(!_owner || _owner == c || !c);
    _owner = c;
  }
  bool has_owner()const {return _owner;}
public: // elaboration, USER_NODE
  virtual void connect(NODE_P*) {unreachable();}
  virtual NODE* deflate(NODE_P*, CARD*);
  virtual void expand(CARD*) { unreachable(); }
  virtual CARD_LIST* scope() {unreachable(); return NULL;}
  virtual bool is_grounded()const {return false;} // needed for probe selection
  virtual int type()const {unreachable(); return 0;}
  virtual bool is_short_to(NODE_P const& n)const;
public: // raw data access (rvalues)
  virtual int	user_number()const	{return _user_number;}
  virtual int	flat_number()const	{return INVALID_NODE;} // nodes w/o matrix have get here.
public: // simple calculated data access (rvalues)
  virtual int	matrix_number()const	{assert(0); unreachable(); return 0;} // _sim->_nm[_user_number];
  int	m_()const		{return matrix_number();}
public: // virtuals
  double	tr_probe_num(const std::string&)const override;
  XPROBE	ac_probe_ext(const std::string&)const override;

  double      v0()const	{
    assert(m_() >= 0);
    assert(m_() <= _sim->matrix_nodes());
    return _sim->_v0[m_()];
  }
  double      vt1()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->matrix_nodes());
    return _sim->_vt1[m_()];
  }
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->matrix_nodes());
    return _sim->_ac[m_()];
  }
  //double      vdc()const		{untested();return _vdc[m_()];}

  //double&     i()	{untested();return _i[m_()];}  /* lvalues */
  COMPLEX&    iac() {
    assert(m_() >= 0);
    assert(m_() <= _sim->matrix_nodes());
    return _sim->_ac[m_()];
  }
}; // NODE
/*--------------------------------------------------------------------------*/
class MATRIX_NODE : public NODE {
  int _flat_number{-1};
public:
  explicit MATRIX_NODE(NODE const* s, int f) : NODE(s), _flat_number(f) {}
  explicit MATRIX_NODE(CARD* s, int f) : NODE(s, f), _flat_number(f) {}
  int	matrix_number()const override;
  void set_flat_number(int u) {
    _flat_number = u;
  }
 // int user_number()const override;
  int flat_number()const override;
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class NODE;
class CARD;
class CARD_LIST;
/*--------------------------------------------------------------------------*/
int user_number(NODE const*);
std::string const& short_label(NODE const*);
/*--------------------------------------------------------------------------*/
class NODE_P : private NODE_P_BITS {
  enum dir_t{
    d_none = 0,
    d_output = 1,
    d_input = 2,
    d_inout = 3
  };

public: // implicit copy, used for set_parameters
  NODE_P(NODE_P const& n) : NODE_P_BITS(n) {
    if(n.is_node()) {
      assert(is_node());
      NODE_P_BITS::set_own(false);
    }else if(n.is_link()) {
    }
  }

public:
  explicit NODE_P(NODE* p) : NODE_P_BITS(p) { }
  explicit NODE_P() : NODE_P_BITS() {
    static_assert(alignof(NODE_P)>7);
  }
  explicit NODE_P(NODE_P&& n) : NODE_P_BITS(n) {
    n.set_none();
  };
  explicit NODE_P(NODE_P const& n, int)
    : NODE_P_BITS(n){ } // needed?
  ~NODE_P();

  NODE_P& operator=(NODE_P const& n) {
    if(is_node()){
      assert(!is_own_node());
    }else{
    }
    if(n.is_node()) {
      set_node(n.node());
      set_type(n.type());
      if(n.is_ground()){
	set_ground();
      }else{
      }

      NODE_P_BITS::set_own(false);
      assert(n_() == n.n_());
    }else if(n.is_link()) {
      /// used in internal node collapse
      // incomplete();
      if(!is_connected()){
      }else if(is_none()){untested();
      }else if(!is_link()){untested();
      }else if( next() == this){
      }else{ untested();
      }
      if(is_none()){
	set_io_link(); // BUG
      }else if(is_number()){
	set_io_link(); // BUG
      }else{
      }
      assert(n.next());
      set_next(n.next()); // const??
      assert(is_link());
    }else if(n.is_none()){
      assert(is_none());
    }else{ untested();
      unreachable();
    }
    return *this;
  }
  NODE_P& operator=(NODE_P&& n) {
    assert(n.is_node());
    set_node(n.node());
    if(n.is_ground()){ untested();
      set_ground();
    }else{
    }
    if(n.is_own_node()){ untested();
      NODE_P_BITS::set_own(true);
    }else{
    }
    n.set_none();
    return *this;
  }

  // NODE_P clone()const { untested();
  //   return NODE_P(*this);
  // }
  void expand(CARD* owner);
  NODE_P& map();

private:
public: // hmm
  void req_type(int);
protected:

  void set_input() { set_direction(d_input); }
  void set_output() { set_direction(d_output); }
public:
  NODE_P& set_none() { NODE_P_BITS::set_none(); return *this; }
  void set_inout() { set_direction(d_inout); }
  void set_user_number(int u) {
    assert(!is_link());
    assert(!is_node() || !n_() );
    NODE_P_BITS::set_number(u);
  }

  bool is_none()const {return NODE_P_BITS::is_none(); }
  bool is_node()const {return NODE_P_BITS::is_node(); }
  bool is_own_node()const {return NODE_P_BITS::is_own(); }
  void set_node(NODE* n) {assert(n); NODE_P_BITS::set_node(n);}
  // [..]

private:
  NODE const* node_ptr() const{
    return const_cast<NODE_P*>(this)->node_ptr();
  }
  bool is_inout()const { return direction() & d_inout; }
  bool is_input()const { return direction() & d_input; }
  bool is_output()const { return direction() & d_output; }
protected:
  NODE* node_ptr() {
    // assert(!direction());
    if(is_node()){
      // assert((_int << 16 >> 16) == _int);
      return NODE_P_BITS::node_safe();
    }else{
      return NULL;
    }
  }
public: // BUG. internal
  NODE_P& set_own(bool x=true){
    assert(is_node());
    NODE_P_BITS::set_own(x);
    return *this;
  }
  NODE_P& set_own(NODE* p){
    set_node(p);
    assert(p == node_ptr());
    NODE_P_BITS::set_own(true);
    assert(p == node_ptr());
    return *this;
  }
public:
  bool operator==(NODE_P const& p )const {
    //return NODE_P_BITS::operator==(p)?
    if(is_link() != p.is_link()){
	return false;
    }else if(is_link()){
      return next() == p.next();
    }else if(is_node()){
      return node() == p.node();
    }else{ untested();
      unreachable();
    }
    return false;
  }
  bool is_link()const { return NODE_P_BITS::is_link(); }
  bool is_number()const { return NODE_P_BITS::is_number(); }
  void set_io_link() {
    assert(is_node() || is_none() || is_number());
    set_inout();
    assert(direction()==3);
    NODE_P_BITS::set_link();
    set_next(this);
  }
  bool is_connected()const{
    if(is_node()){
      return true;
    }else if(is_number()){
      return false;
    }else if(is_grounded()){
      return true;
    }else if(is_none()){
      return false;
    }else if(is_link()){
      if(next() == NULL){ untested();
	return false;
      }else if(next() == this){
	// self connection indicates use
	return true;
      }else{ //
	// return false; no.
	return true;
      }
    }else if(is_inout()){ untested();
      incomplete();
      return true;
      // return _ptr; // yikes.
      return false;
    }else if(n_()) {
      return n_();
    }else{ untested();
      return false;
    }
  }
  NODE_P* next() {
    assert(is_link());
    return NODE_P_BITS::next();
  }
  NODE_P /*const??*/ * next()const {
    assert(is_link());
    return NODE_P_BITS::next();
  }
  void map_subckt_node(NODE_P* map, CARD const*);
  NODE_P& merge(NODE_P* p);

  void new_node(const std::string&, CARD const*);
  NODE_P& new_model_node(const std::string& n, CARD*);
  NODE_P& set_to(NODE_P& n, CARD*);

  void set_to_ground(CARD const*);
  void set_next(NODE_P* const& p) {
    NODE_P_BITS::set_next(p);
    assert(!p || !is_none());
  }
  void deflate(CARD*);
  int user_number()const {
    if(is_number()){
      assert(!is_link());
      return NODE_P_BITS::number();
    }else if(is_node()) {
      return ::user_number(n_());
    }else if(is_link()){ untested();
      incomplete(); // use ->, data?
      return -1;
    }else{
      return -1;
    }
  }
  void set_type(int t) { NODE_P_BITS::set_type(t); }
  int type()const { return NODE_P_BITS::type(); }

  void set_ground();
public:
  bool is_ground()const{
    return NODE_P_BITS::is_ground();
  }
public: // old names, node_t..
  bool is_short_to(NODE_P const& n)const;
  bool is_grounded()const; // e_node.cc
  const NODE* n_()const {return node_ptr();}
  NODE*	      n_()	{return node_ptr();}
  std::string  short_label()const;

private: // internal
  void set_direction(int dir) {
    assert(is_node() || is_none() || is_number());
    assert(dir);
    assert(dir<4);
    NODE_P_BITS::set_direction(dir);
  }

private: // raw data access (lvalues)
  NODE const& data()const;
  NODE&	data();

public: // same as in node_t, but NODE*
  const NODE* operator->()const	{return &data();}
  NODE*	    operator->()	{return &data();}

private:
  NODE_P const& root()const;
  NODE_P& root();
  bool is_root()const;
  friend class USER_NODE;
  static NODE_P& merge(NODE_P& a, NODE_P& b);
}; // NODE_P
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline std::string NODE_P::short_label() const
{
  if (is_node()){
    return ::short_label(n_());
  }else if(is_link()){
    NODE_P const& r = root();
    assert(r.is_node());
    assert(r.n_());
    return r.n_()->short_label();
  }else{
    return "?????";
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class LOGIC_NODE;
class INTERFACE node_t : public NODE_P {
private:
  int _m;	// cached. for speed;
  // 32 bits unused here.
private:
  static bool node_is_valid(int i) {
    if (i == INVALID_NODE) {
    }else if (i < 0) { untested();
      unreachable();
    }else if (i > NODE::_sim->total_nodes()) { untested();
      unreachable();
    }else{
    }
    return i>=0 && i<=NODE::_sim->total_nodes();
  }
public:
  int	      m_()const	{
    assert(n_());
    assert(n_()->matrix_number() == _m);
    return _m;
  }

  const std::string  short_label()const {return ((n_()) ? (n_()->short_label()) : "?????");}
  void	set_to_ground(CARD* Owner)	{new_node("0", Owner);}
  void	new_node(const std::string&, const CARD*);
  void	new_model_node(const std::string& n, CARD* d);
  void	map_subckt_node(int* map_array, const CARD* d);
  bool	is_grounded()const { return false;}
  bool	is_connected()const {return (e_() != INVALID_NODE);}
  bool	is_short_to(node_t const& n)const {return n_() == n.n_();} // BUG. doesn't work

  node_t&     map();

  explicit    node_t();
	      node_t(const node_t&);
  explicit    node_t(NODE*);
	      ~node_t() {}

public:

  node_t& operator=(node_t const& n);
  node_t& operator=(const NODE_P& p);

public:
  double      v0()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->total_nodes());
    assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->v0() == NODE::_sim->_v0[m_()]);
    return NODE::_sim->_v0[m_()];
  }
  
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->total_nodes());
    assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->vac() == NODE::_ac[m_()]);
    return NODE::_sim->_ac[m_()];
  }
  
  double&     i() {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->total_nodes());
    return NODE::_sim->_i[m_()];
  }

  // top level kludge.
  // n_() may be a USER_NODE, shortened to n.n_(), but n_() != n.n_()
  // fastest way is ... compare matrix number,
  // but doesn't work during elaboration
  bool is_short_to(node_t const& n)const{
    if(_m == INVALID_NODE){
      return NODE_P::is_short_to(n);
    }else{
      return _m == n._m;
    }
  }
};
/*--------------------------------------------------------------------------*/
inline bool NODE_P::is_root() const
{
  if(is_node()){
    return true;
  }else if(is_link()) {
    return next() == this;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
inline NODE_P const& NODE_P::root() const
{
  NODE_P const* r = this;
  while (r->is_link() && r->next() != r) {
    r = r->next();
  }
  return *r;
}
/*--------------------------------------------------------------------------*/
inline NODE_P& NODE_P::root()
{
  NODE_P* r = this;
  NODE_P* prev = NULL;
  while (r->is_link() && r->next() != r) {
    prev = r;
    r = r->next();
#if 1
    if(r->is_link()){
      prev->set_next(r->next());
    }else{
    }
#endif
  }
  assert(r->is_root());
  return *r;
}
/*--------------------------------------------------------------------------*/
inline NODE_P& NODE_P::merge(NODE_P& A, NODE_P& B)
{
  NODE_P& a = A.root();
  NODE_P& b = B.root();
  if(a.is_root()){
  }else{ untested();
  }
  if(b.is_root()){
  }else{ untested();
  }
  trace2("merge", a.is_root(), b.is_root());
  trace2("merge", a.is_node(), b.is_node());
  trace2("merge", a.is_link(), b.is_link());

  NODE_P* new_root = NULL;
  NODE_P* sub_root = NULL;

  assert(!a.is_none());
  assert(!b.is_none());

  if(a.is_node()) {
    new_root = &a;
    sub_root = &b;
  }else if(b.is_node()) {
    new_root = &b;
    sub_root = &a;
  }else{
    assert(a.is_link());
    assert(b.is_link());
    if(a.next() == &a){
      new_root = &b;
      sub_root = &a;
    }else if(b.next() == &b){ untested();
      new_root = &a;
      sub_root = &b;
    }else{ untested();
      incomplete();
      // new_root = &a;
      // sub_root = &b;
    }
  }

  assert(new_root);
  assert(sub_root);
  *sub_root = *new_root;

  return *new_root;
}
/*--------------------------------------------------------------------------*/
inline NODE const& NODE_P::data() const
{
  NODE_P const& r = root();
  assert(r.is_node());
  return *r.node();
}
/*--------------------------------------------------------------------------*/
inline NODE& NODE_P::data()
{
  NODE_P& r = root();
  assert(r.is_node());
  return *r.node();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
