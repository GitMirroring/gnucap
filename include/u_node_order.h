/*                                  -*- C++ -*-
 * Copyright (C) 2018 Felix Salfelder
 * Author: Felix Salfelder <felix@salfelder.org>
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
 * node orderings
 */
#ifndef U_NODE_ORDER_H
#define U_NODE_ORDER_H
#include "m_matrix.h"
#include "u_node_order.h"
#include "c_comand.h"
#include "io_trace.h"
#include "m_matrix.h"
/*--------------------------------------------------------------------------*/
class NODE_ORDER;
/*--------------------------------------------------------------------------*/
class ORDERING : public CMD{
public: // construct
  explicit ORDERING() : CMD() {}
  virtual ORDERING* clone() const=0;
  virtual ~ORDERING(){}
protected:
  ORDERING( const ORDERING& o) : CMD(o) {}
public: // ordering stuff
  // create initial ordering (incidence not known yet)
  virtual void init(unsigned total_nodes, NODE_ORDER&)=0;
  virtual std::string name() const=0;
  virtual void iwant(unsigned a, unsigned b, BSMATRIX_LAYOUT& m){
    // plugins could collect it on their own ..
    // (legacy: just bump spikes).
    m.iwant(a, b);
  }
  virtual void remap(NODE_ORDER&){ }
  virtual void bump(BSMATRIX_LAYOUT&, NODE_ORDER const&){
    // compute a new ordering, apply to m.
    // (legacy: spikes already bumped)
  }
private: // overrides
  virtual void do_it(CS&, CARD_LIST*){ untested();
    incomplete();
  }
protected: // friend access
  std::vector<unsigned>& nm(NODE_ORDER& x) const;
};
/*--------------------------------------------------------------------------*/
bool Get(CS&, const std::string& key, ORDERING const** val);
OMSTREAM& operator<<(OMSTREAM& o, ORDERING const* x);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class NODE_ORDER{
private: // types
    typedef std::vector<unsigned> vector_t;
public: // construct
  explicit NODE_ORDER() : _order(NULL) { }
public:
  void uninit(){
    _nm.resize(0);
  }
  size_t size() const{ untested();
    return _nm.size()-1;
  }
  bool empty() const{
    return _nm.empty();
  }

public: // forward to virtual functions
  void reinit(unsigned total_nodes); // u_node_order.cc
  void iwant(unsigned a, unsigned b, BSMATRIX_LAYOUT& aa){
    assert(_order);
    _order->iwant(a, b, aa);
  }
  void remap();
  virtual void bump(BSMATRIX_LAYOUT& m);

public:
  unsigned const& operator[](unsigned x) const{ return _nm[x]; }
  // don't use
  unsigned const& operator[](int x) const{ return _nm[size_t(x)]; }
private:
  vector_t _nm;
  ORDERING* _order;

friend class ORDERING;
}; // NODE_ORDER
/*--------------------------------------------------------------------------*/
inline std::vector<unsigned>& ORDERING::nm(NODE_ORDER& x) const{
  return x._nm;
}
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
