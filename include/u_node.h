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
 * user nodes
 */
#ifndef U_NODE_H
#define U_NODE_H
#include "e_node.h" // declare NODE_P
#include <set> // todo: use some linked list
/*--------------------------------------------------------------------------*/
// a node in u_nodemap and/or at top level
// almost a NODE_P, but need extra stuff from NODE vtable at top level
class USER_NODE : public NODE {
protected:
  NODE_P _conn{NULL};
  std::set<int> _types; // WIP. disciplines go here.
protected:
  USER_NODE(USER_NODE const& n) = delete;
public:
  explicit USER_NODE(CARD const* n, int u);
  explicit USER_NODE(USER_NODE const* n);

  NODE_P& link() {
    bool was_ground = is_grounded();
    _conn.set_node(this);
    if(was_ground){
      // incomplete();
      _conn.set_ground();
    }else{
    }
    return _conn;
  }
  void expand(CARD* owner) override;
  NODE* deflate(NODE_P* p, CARD* owner)override;
  bool is_short_to(NODE_P const& n)const override;
public: // NODE
  int matrix_number()const override;
  double tr_probe_num(const std::string& x)const override;

public: // conn
  void set_ground();
  void set_type(int type); // int?
  bool is_grounded()const; // { return _is_ground; }
  int type()const;
public:
  NODE& data() {
    return _conn.data();
  }
  NODE const& data()const {
    return _conn.data();
  }
  NODE const* node()const {
    NODE_P const& r = _conn.root();
    assert(r.is_node());
    return r.node();
  }

  void req_type(int i);
  NODE* select(int t);
private: // NODE_P supplements (mainly top level)
 // void merge(NODE_P*p);
  friend class NODE_P;
  NODE& merge(NODE_P* o);
  static void merge(USER_NODE*, USER_NODE*);
}; // USER_NODE
/*--------------------------------------------------------------------------*/
// a node in a module (e.g. subcircit or modelgen device)
class MODULE_NODE : public USER_NODE {
public:
  explicit MODULE_NODE(USER_NODE const* n, CARD const* owner) : USER_NODE(n) {
    assert(n);
    set_owner(owner);
    trace1("MODULE_NODE::MODULE_NODE", n->long_label());
  }
public:
  NODE* deflate(NODE_P* p, CARD* owner)override;
};
/*--------------------------------------------------------------------------*/
// user_node??
class GROUND_NODE : public USER_NODE {
public:
  explicit GROUND_NODE(USER_NODE const* n)
   : USER_NODE(n){}
  explicit GROUND_NODE(CARD const* n, int u)
   : USER_NODE(n, u){}
  bool is_grounded()const override {return true;} // needed for probes.
  int matrix_number()const override { return 0;}
  void connect(NODE_P*)override;
  double tr_probe_num(const std::string& x)const override;
};
/*--------------------------------------------------------------------------*/
// extern GROUND_NODE ground_node;
/*--------------------------------------------------------------------------*/
#if 0 // later
// PORT_NODE: responsible for sckt port node expansion
class PORT_NODE : public CONNECT_NODE {
public:
  explicit PORT_NODE(std::string s, int u): CONNECT_NODE(s, u){}
  explicit PORT_NODE(NODE_P* n);
  ~PORT_NODE() { untested(); }
  void connect(NODE_P*)override;
  NODE* deflate(NODE_P* p, CARD* owner)override;
};
#endif
/*--------------------------------------------------------------------------*/
inline int USER_NODE::matrix_number() const
{
  if(_conn.is_node()){
    assert(_conn.is_node());
    if(_conn.n_() == this){
      unreachable();
      incomplete();
      return -1;
    }else{
      return(_conn.n_()->matrix_number());
    }
  }else if(_conn.is_link()){
    // CONST_NODE_PS a(&_conn);
    // assert(a.is_node());
    return _conn->matrix_number();
  }else if(_conn.is_none()){
    unreachable();
  }else{
    unreachable();
  }
  return -1;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif // guard
// vim:ts=8:sw=2:noet:
