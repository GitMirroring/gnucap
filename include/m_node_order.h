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
 * nodemapper
 */
#ifndef M_NODE_PERM_H
#define M_NODE_PERM_H
#include "m_matrix.h"
#include "u_node_order.h"

// template<class T>
// class BSMATRIX;
// class SIM_DATA;
class ORDERING;
/*--------------------------------------------------------------------------*/
class NODE_ORDER{
public: // construct
  explicit NODE_ORDER(BSMATRIX<double>& tr, BSMATRIX<COMPLEX>& ac)
    : _aa(tr), _acx(ac), _order(NULL)
  {
  }
public:
  void uninit(){ untested();
    _nm.resize(0);
  }
  void reinit(unsigned total_nodes);

  size_t size() const{ untested();
    return _nm.size()-1;
  }
  bool empty() const{
    return _nm.empty();
  }
  void tr_iwant(unsigned a, unsigned b){ untested();
    _aa.iwant(a, b);
    assert(_order);
    _order->tr_iwant(a, b);
  }
  void ac_iwant(unsigned a, unsigned b){ untested();
    _acx.iwant(a, b);
    assert(_order);
    _order->tr_iwant(a, b);
  }

public:
  unsigned const& operator[](int x) const{ return _nm[size_t(x)]; }
  unsigned const& operator[](unsigned x) const{ return _nm[x]; }

private:
  BSMATRIX<double>& _aa;
  BSMATRIX<COMPLEX>& _acx;
  std::vector<unsigned> _nm; // nm 0...total_nodes
  ORDERING* _order;

friend class ORDERING;
}; // NODE_ORDER
/*--------------------------------------------------------------------------*/
inline std::vector<unsigned>& ORDERING::nm(NODE_ORDER& x) const{ untested();
  return x._nm;
}
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
