/*                           -*- C++ -*-
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
 */
#ifndef U_NODE_ORDER_H
#define U_NODE_ORDER_H
/*--------------------------------------------------------------------------*/
#include "m_node_order.h"
#include "c_comand.h"
#include "io_trace.h"
/*--------------------------------------------------------------------------*/
class NODE_ORDER;
/*--------------------------------------------------------------------------*/
class ORDERING : public CMD{
public:
  virtual void init(unsigned total_nodes, NODE_ORDER&)=0;
  virtual ORDERING* clone() const=0;
  virtual std::string name() const=0;
private: // overrides
  virtual void do_it(CS&, CARD_LIST*){ untested();
    incomplete();
  }
protected: // friend access
  std::vector<unsigned>& nm(NODE_ORDER& x) const{ untested();
    return x._nm;
  }
};
/*--------------------------------------------------------------------------*/
bool Get(CS&, const std::string& key, ORDERING const** val);
OMSTREAM& operator<<(OMSTREAM& o, ORDERING const* x);
#endif
// vim:ts=8:sw=2:noet:
