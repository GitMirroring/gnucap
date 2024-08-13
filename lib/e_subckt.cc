/*                              -*- C++ -*-
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
 * subcircuit
 */
#include "u_nodemap.h"
#include "e_node.h"
// #include "u_node.h"
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::setup_nodes()
{
  int num_nodes = subckt()->nodes()->how_many();
  CARD_LIST* s = subckt();
  for(int i=0; i < net_nodes(); ++i) {
    assert(node(i).is_connected());
  }

  for(int i=net_nodes(); i < net_nodes()+int_nodes(); ++i) {
    if(i < num_nodes){
      NODE_P& n = s->nodes()->nodes()[i];
      trace2("BASE_SUBCKT::setup node internal", long_label(), i);

      if(n.is_link()){
	n.set_none();
	// incomplete();
      }else{
	// n.set_io_link();
      }

      // modelgen uses _n for internal node_t's
      // "new_model_node" activates them
      //
      // why number, could link node(i) -> n ?
      node(i).set_inout();
      node(i).set_user_number(i);

      // n.set_next(&node(i));
      // assert(n.is_none());
      trace3("BASE_SUBCKT::setup node internal", i, node(i).user_number(), &node(i));
      assert(i == node(i).user_number());
      assert(!node(i).is_none());
    }else{ untested();
      incomplete();
      // dynamic node..
    }
  }

  expand_ports_first();
}
/*--------------------------------------------------------------------------*/
// connect upper/lower in case they are of the same type.
// these ports will not need connect modules.
void BASE_SUBCKT::expand_ports_first()
{
  CARD_LIST* s = subckt();
  assert(s);
  int num_nodes = s->nodes()->how_many();
  NODE_P* lower = s->nodes()->map();

  for(int i=0; i < net_nodes(); ++i) {
    if(node(i).is_link()){
    }else if(node(i).is_node()){
      // top level
    }else{ untested();
      unreachable();
    }
    //  node(i): upper node, outward port
    //  lower[i]:  lower node
    if(i >= num_nodes) { untested();
    }else if(node(i).type() == lower[i].type()) {
      // assert(type);
      // TODO: allocate split nodes,
      //       place connect modules as needed
      // todo:: tag, passed to caller? something else?
      // lower[i].set_next(&node(i)); // pass to caller.
      trace4("BASE_SUBCKT::epf pass", long_label(), i, node(i).is_link(), node(i).type());
      lower[i] = node(i); // pass to caller.
    }else if(node(i).is_node()){
      auto un = prechecked_cast<USER_NODE*>(node(i).n_());
      assert(un);
      trace4("BASE_SUBCKT::epf req", long_label(), i, lower[i].type(), node(i).type());
      if(node(i).type()){ untested();
      }else if(lower[i].type() == 12345){
	// this is a hack to resolve hybrid nodes.
	// just make the whole net hybrid.
	// will be replaced by connect module placement
        node(i).set_type(lower[i].type());
        un->req_type(lower[i].type());
	lower[i] = node(i); // pass to caller.
      }else{ untested();
      }

    }else if(!node(i).type()){ untested();
      if(lower[i].type() == 12345){ untested();
        node(i).req_type(lower[i].type());
      //  un->req_type(lower[i].type());
	lower[i] = node(i); // pass to caller.
      }else{ untested();
      }


    }else{ untested();
      trace4("BASE_SUBCKT::epf incompatible", node(i).short_label(), i, lower[i].type(), node(i).type());
      trace4("BASE_SUBCKT::epf incompatible", node(i).short_label(), i, lower[i].is_node(), node(i).is_node());
      unreachable(); incomplete();
      // happens in modelgen models... presumably already connected.
    }
  }
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::expand_first()
{
  COMPONENT::expand_first();
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::expand_ports()
{
  int num_nodes = subckt()->nodes()->how_many();
  trace4("BASE_SUBCKT::expand_nodes ports", long_label(), num_nodes, net_nodes(), min_nodes());
  // CARD_LIST::expand doesnt know which ones are ports
  // ports need different treatment.
  for(int i=0; i < net_nodes(); ++i) {
    // assert(node(i));
    // CARD_LIST* s = subckt();
    // node(i) is upper port
    // s->node(i) is lower port
    // requested_discipline = _n[i].discipline();
    if(node(i).is_node()){
      // incomplete();
      // hmm
    }else if(!node(i).is_link()){ untested();
      unreachable();

    }else if(i < num_nodes){
      // TODO: allocate split nodes,
      //       place connect modules as needed
           //  incomplete();
	    // node(i).merge(&s->nodes()->nodes()[i]); // pass to caller.
    }else{ untested();
      // happens in modelgen models... presumably already connected.
    }
  }
}
/*--------------------------------------------------------------------------*/
// expand nodes >= net_nodes that are connected.
// normally internal nodes in a subckt instance
void BASE_SUBCKT::expand_nodes()
{
  assert(subckt());
  NODE_MAP& nm = *subckt()->nodes();
  // return subckt()->deflate_nodes(this); // not yet.
  int num_nodes = nm.how_many();
  int len = int(nm.length());
  trace3("alloc internal node", long_label(), num_nodes, len);
  assert(len==num_nodes); // incomplete();
  expand_ports();

  // TODO: what about nodes that are short to pass-through ports?
  // d_short.6.gc
  for(int i=len; i>net_nodes(); ) {
    --i;
    trace3("alloc internal node", long_label(), i, net_nodes());
    trace2("alloc internal node", nm.label(i), &nm);
    NODE_P& pi = nm.nodes()[i];
    trace2("alloc internal node", &pi, pi.is_connected());
    if(!pi.is_connected()){
      trace3("alloc internal not needed??", long_label(), i, pi.is_grounded());
      //trace3("alloc internal not needed", long_label(), i, pi.short_label());
      // not needed.
    }else if(pi.is_link() && pi.next() != &pi){ untested();
      // assert(!pi.is_root())?
      // controlled elsewhere
      trace3("alloc internal elsewhere??", long_label(), i, pi.is_grounded());
    }else if(pi.is_link()){
      // assert(pi.is_root())?
      assert(pi.next() == &pi); // connected??
      // clone/expand/deflate nm.proto(i).
      // cutting a corner, because clone is expensive

      trace4("alloc internal2", long_label(), i, pi.type(), pi.next() == &pi);
      MODULE_NODE cn(nm.proto(i), this);
      cn.set_owner(this);
     // cn.connect(pi); TODO

      assert(nm.proto(i)->short_label() == cn.short_label());
      assert(cn.user_number() == i);

      // pi.expand(); ... TODO
      pi.set_node(cn.deflate(&pi, this));
      assert(pi.is_node());
      pi.set_own();
      trace3("alloc sckt node link", long_label(), pi.n_()->long_label(), pi.user_number());
      assert(pi.n_());
    }else if(pi.n_()){ untested();
      trace3("alloc sckt node: exists", long_label(), pi.short_label(), pi.user_number());
      incomplete();
      unreachable();
      // something with split nodes?
      pi.expand(this);
    }else{ untested();
      unreachable();
    }
  }
} // expand_nodes
/*--------------------------------------------------------------------------*/
// used in modelgen models.
// supposedly the same as expand_nodes, forward.
void BASE_SUBCKT::expand_model_nodes()
{
  int num_nodes = subckt()->nodes()->how_many();
  int len = int(subckt()->nodes()->length());
  assert(len<=num_nodes);
  assert(net_nodes()<=len);

  int i=num_nodes;
  for(; i>len; ) { untested();
    --i;
    incomplete();
  }
  for( assert(i==len); i>net_nodes(); ) {
    --i;
    trace3("BASE_SUBCKT::expand_model_nodes internal", i, node(i).is_number(), &node(i));
  }

  return expand_nodes();
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::map_nodes()
{
  COMPONENT::map_nodes();
  if(!is_device()){ untested();
  }else if(subckt()) {
    if(subckt()->nodes()){
      subckt()->nodes()->map_nodes();
    }else{ untested();
    }
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
