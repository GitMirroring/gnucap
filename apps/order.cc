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
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "ap.h"
#include "globals.h"
#include "u_node_order.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class ORDER_REVERSE : public ORDERING{
public:
  explicit ORDER_REVERSE(std::string name="reverse") : _name(name) {}
private: //override
  ORDERING* clone()const{return new ORDER_REVERSE(*this);}
  std::string name() const{return _name;}
  void init(unsigned total_nodes, NODE_ORDER& no){
    std::vector<unsigned>& n=nm(no);

    n.resize(total_nodes+1);
    n[0] = 0;
    for (unsigned node = 1; node<=total_nodes; ++node) {
      n[node] = unsigned(total_nodes - node) + 1;
    }
  }
private:
  std::string _name;
}ro;
DISPATCHER<CMD>::INSTALL roi(&order_dispatcher, "reverse|default", &ro);
ORDER_REVERSE ro2("auto");
DISPATCHER<CMD>::INSTALL roi2(&order_dispatcher, "auto", &ro2);
/*--------------------------------------------------------------------------*/
struct set_default_order{
  set_default_order(){
    OPT::order=&ro2;
  }
}d;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class ORDER_FORWARD : public ORDERING{
private: //override
  ORDERING* clone()const{return new ORDER_FORWARD(*this);}
  std::string name() const{return "backward";}
  void init(unsigned total_nodes, NODE_ORDER& no){ untested();
    std::vector<unsigned>& n=nm(no);

    n.resize(total_nodes+1);
    for (unsigned node = 0; node<=total_nodes; ++node) { untested();
      n[node] = node;
    }
  }
}of;
DISPATCHER<CMD>::INSTALL ofi(&order_dispatcher, "forward", &of);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
