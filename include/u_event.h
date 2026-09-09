/*                     -*- C++ -*-
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
 * events
 */
#ifndef U_EVENT_H
#define U_EVENT_H
/*--------------------------------------------------------------------------*/
// external
class WAVE;
class CARD;
class CARD_LIST;
class LOGIC_NODE;
class CKT_BASE;
/*--------------------------------------------------------------------------*/
class EVENT {
  double    _time  {NEVER};
  CARD* _owner {nullptr};
  EVENT() = delete;
public:
  EVENT(double Time, CARD* Owner)
    : _time(Time), _owner(Owner) {}
  EVENT(const EVENT& E)
    : _time(E._time), _owner(E._owner) {}
  ~EVENT() {}
  operator double() const {return _time;}
  double time() const {untested(); return _time;}
  CARD* owner() const {untested(); assert(_owner); return _owner;}
  bool operator<(EVENT const& o)const {
    if(_time < o._time) {
      return true;
    }else if(_time == o._time) {
     return _owner < o._owner;
    }else{
      return false;
    }
  }
  bool operator>=(double const& t)const { untested();
    return _time >= t;
  }
  struct greater{
    bool operator()(EVENT const& a, EVENT const& b)const {
      return b < a;
    }
  };
};
/*--------------------------------------------------------------------------*/
class EVENT_QUEUE {
  std::priority_queue<EVENT, std::deque<EVENT>, EVENT::greater > _eq;
public:
  explicit EVENT_QUEUE() {}
  ~EVENT_QUEUE() {}
public:
  void push(double Time, CARD* Owner) { _eq.push(EVENT(Time, Owner)); }
  bool empty()const {return _eq.empty();}
  EVENT const& top() {return _eq.top();}
  void pop() {_eq.pop();}
  void clear() {while(!_eq.empty()){_eq.pop();}}
public:
  void tr_advance_recursive();
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
