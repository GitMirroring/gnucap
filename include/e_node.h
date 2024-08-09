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
#include "e_base.h"
#include "e_bits.h" // declare NODE_P_BITS
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
class NODE_P;
class NODE_MAP;
class NODE : public CKT_BASE /*CARD?*/ {
private:
  CARD const* _owner{NULL};
  int _user_number{-1}; // unneeded? //
			// owner->nodes[u] == this ...
  mutable int _probes{0};
protected:
  explicit NODE(CARD const*, int u);
private:
  explicit NODE(const NODE& p); // u_nodemap.cc:49 (deep copy)
protected:
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  // explicit NODE(const std::string& s);
public:
  ~NODE();

public: // label
  std::string const& short_label()const final override;
  std::string long_label()const final override;
  void set_user_number(int i) {_user_number = i;}
  void set_label(const std::string& s)final {assert(short_label() == s);}
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

public: // probes
  void  inc_probes()const override {++_probes;}
  void  dec_probes()const override {assert(_probes>0); --_probes;}
  bool  has_probes()const override {untested();return _probes > 0;}
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
    : NODE_P_BITS(n){ incomplete(); } // needed?
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
      }else{
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
      //set_next(n); // .next());
    }else if(n.is_none()){
      assert(is_none());
    }else{
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

  NODE_P& set_none() { NODE_P_BITS::set_none(); return *this; }
  NODE_P clone()const { untested();
    return NODE_P(*this);
  }
  void expand(CARD* owner);
  NODE_P& map();

private:
public: // hmm
  void req_type(int);
protected:

  void set_input() { set_direction(d_input); }
  void set_output() { set_direction(d_output); }
public:
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
    assert(!direction());
    if(is_node()){
      // assert((_int << 16 >> 16) == _int);
      assert(direction() == 0);
      return NODE_P_BITS::node_safe();
    }else{
      return NULL;
    }
  }
#if 0
  NODE* __node_ptr() { untested();
    if(is_node()){ untested();
      // uintptr_t mask = uintptr_t(-1) >> 16;
      // return (NODE*)(_uint & mask & ~uintptr_t(4));
      assert(!type()); // really?
      assert(!is_inout());
      return NODE_P_BITS::node();
    }else{ untested();
      return NULL;
    }
  }
#endif
public: // BUG. internal
  // NODE_P& set_ref(NODE* p){ untested();
  //   _ptr = p;
  //   assert(!(_uint & 7));
  //   assert(p == n_());
  //   return *this;
  // }
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
    if(is_link()){ untested();
      assert(p.is_link());
      return next() == p.next();
    }else if(is_node()){
      return node() == p.node();
    }else{ untested();
      unreachable();
    }
    return false;
  }
 //  void set_link(int dir){
 //    assert(dir);
 //    assert(dir<=3);
 //     assert(is_inout()); // why?
 //    return NODE_P_BITS::set_link(dir);
 //  }
  bool is_link()const {
    return NODE_P_BITS::is_link();
  }
  bool is_number()const {
    return NODE_P_BITS::is_number();
  }
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
 // operator bool()const {return _ptr;}
  void map_subckt_node(NODE_P* map, CARD const*);
  // void connect(NODE_P* node);
  // bool links_to(NODE_P const& p)const;
  NODE_P& merge(NODE_P* p);
 // void	new_node(const std::string&, CARD*);
 // void	new_node(const std::string&, NODE_MAP*);

  // NODE_P& reset(int i, CARD*);
  // NODE_P& new_node(const std::string&, CARD_LIST*);
  void new_node(const std::string&, CARD const*);
  NODE_P& new_model_node(const std::string& n, CARD*);
  NODE_P& set_to(NODE_P& n, CARD*);

  void set_to_ground(CARD const*);
  void set_next(NODE_P* const& p) {
#if 0
    assert(p);
    assert(is_link() || is_none());
    // assert(p->is_link());
    if(is_link()){
    }else if(is_node()){ untested();
    }else if(is_none()){ untested();
    }
    assert(is_link() || is_none());
    // assert(!_nnn || _int & 3);
    // assert(!_nnn || is_link());
    uintptr_t P = uintptr_t(p);

    assert(! (P & (uintptr_t(-1) << 48)));
    assert(! (P & uintptr_t(7)));
    assert(P == uintptr_t(p));
#endif

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
  void set_type(int t) {
    assert(t<16000);
    NODE_P_BITS::set_type(t);
  }
  int type()const { return NODE_P_BITS::type(); }
 //  int flat_number()const {
 //    incomplete();
 //    return -1;
 //   // return _nnn.flat_number();
 //  }
  // int size()const;

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
  //NODE_P& root();
  static NODE_P& merge(NODE_P& a, NODE_P& b);
}; // NODE_P
/*--------------------------------------------------------------------------*/
// embed into NODE_P? maybe a little messy
//    (maybe not.)
class CONST_NODE_PS {
protected:
  NODE_P* _root{NULL};
protected:
  explicit CONST_NODE_PS() {}
public:
  explicit CONST_NODE_PS(NODE_P const* node);
  bool operator==(CONST_NODE_PS const& p)const {
    return _root == p._root;
  }
  bool intersects(CONST_NODE_PS const& p)const {
    return _root == p._root;
  }
  bool is_node()const {
    assert(_root);
    return _root->is_node();
  }
  NODE const* operator->()const {
    assert(_root);
    assert(is_node());
    return _root->n_();
  }
  NODE_P const* node_p()const { untested();
    return _root;
  }
  NODE const* node()const {
    assert(_root);
    assert(_root->is_node());
    return _root->n_();
  }
public: // USER_NODE
  int type()const;
  bool is_grounded()const;
protected:
  void set_root(NODE_P* r) { _root = r; }
}; // CONST_NODE_PS
/*--------------------------------------------------------------------------*/
class NODE_PS : public CONST_NODE_PS {
public:
  explicit NODE_PS(NODE_P* node);
public:
  NODE_P& root(){assert(_root); return *_root;}
public: // modify
 // void merge(NODE_PS& b);
  NODE_PS& add(NODE_P* b);
  NODE* node() {
    assert(_root);
    assert(_root->is_node());
    return _root->n_();
  }
  void set_to_ground(CARD const*d) { untested();
    assert(_root);
    _root->set_to_ground(d);
  }

  NODE_PS& operator=(NODE* n) {
    assert(_root);
    if(_root->is_link()){
    }else if(_root->is_node()){
    }else{
    }
    _root->set_node(n);
    return *this;
  }

  NODE* operator->() {
    assert(_root);
    assert(is_node());
    return _root->n_();
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline std::string NODE_P::short_label() const
{
  if (is_node()){
    return ::short_label(n_());
  }else if(is_link()){
    CONST_NODE_PS nn(this);
    return nn->short_label();
  }else{
    return "?????";
  }
}
/*--------------------------------------------------------------------------*/
#if 0
inline NODE_P& NODE_P::clear()
{
  incomplete();
  // unreachable();

  NODE_P_BITS::clear();
  return *this;
}
#endif
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

  //int	      t_()const

  bool	is_grounded()const {return false;}

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
#if 0
  COMPLEX&    iac() {untested();
    assert(n_());
    assert(n_()->m_() == m_());
    assert(n_()->iac() == NODE::_ac[m_()]);
    //return n_()->iac();
    return NODE::_sim->_ac[m_()];
  }
#endif

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
  }else{ untested();
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

  if(a.is_none()) { untested();
    new_root = &b;
    sub_root = &a;
  }else if(b.is_none()) { untested();
    new_root = &a;
    sub_root = &b;
  }else if(a.is_node()) {
    // assert(!dynamic_cast<USER_NODE const*>(a->n_()));
    // assert(!b->is_node());

    new_root = &a;
    sub_root = &b;
  }else if(b.is_node()) {
    // assert(!dynamic_cast<USER_NODE const*>(b->n_()));
    // assert(!a->is_node());

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
  {
    assert(sub_root);
    if(new_root->is_node()){
    }else if(new_root->is_none()){ untested();
    }else{
    }
    // if(sub_root->is_node()){
    //   sub_root->set_io_link();
    // }else if(sub_root->is_none()){ untested();
    //   incomplete();
    //   sub_root->set_io_link();
    // }else{
    // }
    *sub_root = *new_root;

    // sub_root->set_next(new_root); //
    // if(sub_root->type()){ untested();
    //   new_root->set_type(sub_root->type());
    // }else{
    // }
    // if(sub_root->is_grounded()){ untested();
    //   new_root->set_ground();
    // }else{
    // }

    // if(new_root->is_link()){
    // }else if(new_root->is_node()){
    // }else{
    //   unreachable();
    // }
  }
  return *new_root;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
