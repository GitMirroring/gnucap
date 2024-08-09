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
 * NODE_P bits.
 */
#ifndef E_BITS_H
#define E_BITS_H
/*--------------------------------------------------------------------------*/
class NODE_P;
/*--------------------------------------------------------------------------*/
#if 1
#include <cstdint>

/* NODE_P
 * tag       desc
 * 000       ptr to NODE instance, port
 * 001       nodemap_ref output
 * 010       nodemap_ref input
 * 011       nodemap_ref i/o
 * 100       like 000, but owned
 * 101       NODE_P link output
 * 110       NODE_P link input
 * 111       NODE_P link i/o
 */

// 64 bit node bits
class NODE_P_BITS {
  union{
    void* _ptr{NULL};
    NODE* _nnn;
    intptr_t _int;
    uintptr_t _uint;
  };
  typedef enum{
    t_none = 0,
    t_ptr = 0,
    t_output = 1,
    t_input = 2,
    t_io = 3, // mid bits are user_number
    t_inout = 3, // mid bits are user_number
    t_link = 4,
    t_own = 4,
    t_stuff = 7
      // t_usr = 2
  }tag_t;
protected:
  explicit NODE_P_BITS() {
    static_assert(sizeof(NODE*) == 8);
    static_assert(sizeof(uint64_t) == 8);
    static_assert(alignof(NODE_P_BITS)>7);
  }
  explicit NODE_P_BITS(NODE_P_BITS const& n) :
    _ptr(n._ptr) {
  }
  explicit NODE_P_BITS(NODE* p) : _ptr(p) {
    assert(!(_uint >> 48)); // heap.
  }
protected: // modify
  void set_none() { _ptr = NULL; }
  void set_number(int u) {
    // assert(!(_uint & 4));
    // assert((_uint & 3));

    _uint &= ~( uintptr_t(uint32_t(-1)) << 16 );
    _uint |= uintptr_t(u) << 16;
  }
  void set_ground() {
    _uint |= (1l << 63); // set_io(3);
  }
  void set_node(NODE* n) {
    assert(!( uintptr_t(n) & ((uintptr_t(-1) << 48) )));

    uintptr_t mask = ~(uintptr_t(-1) >> 16 );
   // mask |= 7;
    _uint &= mask;

    _uint |= uintptr_t(n);
    assert(is_node());
  }
  void set_next(NODE_P* const& p) {
    assert(!( uintptr_t(p) & ((uintptr_t(-1) << 48) )));

    uintptr_t mask = ~(uintptr_t(-1) >> 16 );
    mask |= 7;
    _uint &= mask;

    _uint |= uintptr_t(p);
    assert(is_link());
  }

  void set_link() {
    assert(4 == intptr_t(t_link));
    _int = _int | intptr_t(t_link);
    assert(_int & 4);
    assert(is_link());
  }
  void set_link(int dir) {
    _int = _int | ( intptr_t(t_link) + dir );
  }
  void set_own(bool o=true) {
    if(o){
      _uint |= 4;
    }else{
      _uint &= ~7l;
    }
  }
  void set_tag(tag_t t) { untested();
   _uint &= ~3l;
   _uint |= t;
  }
  void set_type(int t) {
    assert(t<16000); // otherwise, plug in a NODE?
    _uint &= ~( uintptr_t(uint16_t(-1)) << 49 >> 1 );
    _uint |= uintptr_t(t) << 49 >> 1;
  }
  void set_direction(int dir) {
    _int = _int | intptr_t(dir); // really?
  }
public: // mode;
  bool is_none()const { return !_int; }
  bool is_link()const { return _int & intptr_t(4) && _int & intptr_t(3); }
  bool is_node()const { return _int && (tag()==t_ptr || tag()==t_own);}
  bool is_number()const { return !(_int & intptr_t(4)) && (_int & intptr_t(3)); }
  bool is_own()const { return (_uint & 7) == 4; }
protected: // NODE_P access
  bool is_ground()const{ return _uint & (1l << 63); }
  int tag()const { return tag_t(_int & 7); }
  NODE* node()const {
    return node_safe();
    assert(is_node() || is_none());
    assert((_int << 16 >> 16) == _int);
    assert(!(_int & 3));
    return _nnn;
  }
  NODE* node_safe()const {
    uintptr_t mask = uintptr_t(-1) >> 16;
    return (NODE*)(_uint & mask & ~uintptr_t(4));
  }
  NODE_P* next() {
    return reinterpret_cast<NODE_P*>(uintptr_t(ptr()) << 16 >> 16);
  }
  NODE_P /*const??*/ * next()const {
    return reinterpret_cast<NODE_P /*const*/ *>(uintptr_t(ptr()) << 16 >> 16);
  }

  uint16_t type()const{
    uint16_t t = uint16_t(_uint >> 48); // select upper bits
    t &= ~( 1 << 15 ); // drop ground bit
    return t;
  }
  int number()const {
    return uint32_t(_uint >> 16);
  }
  int direction()const { return _int & 3; }
private: // needed?
protected: // BUG
  void* ptr() {
    return reinterpret_cast<void*>(intptr_t(_ptr) & ~intptr_t(7));
  }
  void const* ptr()const {
    return reinterpret_cast<void const*>(intptr_t(_ptr) & ~intptr_t(7));
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#else
// generic node bits
class NODE_P_BITS {
private:
  // union {
  NODE* _nnn{NULL};
  NODE_P* _link{NULL};
  int _number{-1};
  // };
  enum mode_t {
    m_none = 0,
    m_node = 1,
    m_link = 2,
    m_number = 3
  } _mode;
  int _type{0};

  // in NODE_P
 //  enum dir_t {
 //    d_none = 0,
 //    d_output = 1,
 //    d_input = 2,
 //    d_inout = 3
 //  } _direction{d_none};

  int _direction{0};
  bool _owned{false};
  bool _grounded{false};


protected:
  NODE_P_BITS(NODE_P_BITS const& n)
    : _nnn(n._nnn), _link(n._link), _number(n._number),
      _owned(false), _grounded(n._grounded) {}
protected:
  NODE_P_BITS(NODE_P_BITS const& n, int)
    : _nnn(n._nnn){ incomplete(); } // direction? type?
public:
  explicit NODE_P_BITS(NODE* p) : _nnn(p){ };
  explicit NODE_P_BITS(){};
 // explicit NODE_P_BITS(NODE_P_BITS&& n):_nnn(n._nnn){ untested();
 //   n._nnn = NULL;
 // };
  ~NODE_P_BITS() {}

protected:
  bool is_none()const { return _mode == m_none; }
  bool is_node()const { return _mode == m_node; }
  bool is_link()const { return _mode == m_link; }
  bool is_number()const { return _mode == m_number; }
  bool is_ground()const { return _grounded; }
  bool is_inout()const;

  int number()const { untested(); assert(is_number()); return _number; }
  NODE* node()const { assert(is_node() || is_none()); return _nnn; }
  NODE* node_safe()const { return node(); }
  NODE_P* next() { untested(); assert(is_link()); return _link; }
  NODE_P* next() const { untested(); assert(is_link()); return _link; }
  int type()const { return _type; }
  int direction()const { return _direction; }
  bool is_own()const { return _owned; }

public: // modify
  void set_ground() { _grounded = true; }
 // void set_input()  { _direction = d_input; }
 // void set_output() { _direction = d_output; }
 // void set_inout()  { _direction = d_inout; }
  void set_type(int t) { _type = t; incomplete(); }
 // void set_link(int dir) { set_link(); set_direction(dir); }
  void set_none() { _mode = m_none; }
  void set_link() { _mode = m_link; }
  void set_node(NODE* n) { _mode = m_node; _nnn = n; }
  void set_number(int u) { _mode = m_number; _number = u; }

  void set_next(NODE_P* p) { untested();
    assert(!p || is_link());
    set_link();
    _link = p;
  }
  void set_own(bool o=true) { _owned = o; }

protected:
  void set_direction(int dir) {
    // incomplete();
    _direction = dir;
  }

public:
}; // NODE_P_BITS // generic.
/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif // guard
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
