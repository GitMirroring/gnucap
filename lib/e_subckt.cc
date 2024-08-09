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
#include "u_node.h" // connect node
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::setup_nodes()
{
  int num_nodes = subckt()->nodes()->how_many();
  CARD_LIST* s = subckt();
  for(int i=0; i < net_nodes(); ++i) {
    assert(node(i).is_connected());
    if(0&&node(i).is_link()){
      incomplete(); // done?
    }else if(i < num_nodes){
      trace2("BASE_SUBCKT::setup node", long_label(), i);
     // NODE_P& n = s->nodes()->nodes()[i];
     // n = NULL; // clear();
      // n.set_io_link();
     // assert(n.is_link());
    }else{
      incomplete();
    }
  }

  for(int i=net_nodes(); i < net_nodes()+int_nodes(); ++i) {
    if(i < num_nodes){
      NODE_P& n = s->nodes()->nodes()[i];
      trace2("BASE_SUBCKT::setup node internal", long_label(), i);


      if(n.is_link()){
//	n.clear();
	n.set_none();
//	unreachable();
	incomplete();
      }else{
	// n.set_io_link();
      }

      // modelgen uses _n for internal node_t's
      // "new_model_node" activates them
      //
      // why use number, could link node(i) -> n ?
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
// these ports never require connect modules.
void BASE_SUBCKT::expand_ports_first()
{
  CARD_LIST* s = subckt();
  assert(s);
  int num_nodes = s->nodes()->how_many();
  NODE_P* lower = s->nodes()->map();

  trace3("BASE_SUBCKT::expand_ports_first", long_label(), num_nodes, net_nodes());
  for(int i=0; i < net_nodes(); ++i) {
    trace3("BASE_SUBCKT::expand_ports_first1", i, node(i).type(), net_nodes());
    trace2("BASE_SUBCKT::expand_ports_first3", node(i).is_link(), node(i).is_node());
    if(node(i).is_node()){
      trace2("BASE_SUBCKT::expand_ports_first2", i, node(i)->short_label());
    }
    if(node(i).is_link()){
    }else if(node(i).is_node()){
      // top level
    }else{
      unreachable();
    }
    //  node(i): upper node, outward port
    //  lower[i]:  lower node
    if(i >= num_nodes) {
    }else if(node(i).type() == lower[i].type()) {
      // assert(type);
      // TODO: allocate split nodes,
      //       place connect modules as needed
      // todo:: tag, passed to caller? something else?
      // lower[i].set_next(&node(i)); // pass to caller.
      trace4("BASE_SUBCKT::epf pass", long_label(), i, node(i).is_link(), node(i).type());
      lower[i] = node(i); // pass to caller.
    }else if(node(i).is_node()){
      // all nodes are USER_NODE at this stage.
      auto un = prechecked_cast<USER_NODE*>(node(i).n_());
      assert(un);
      trace4("BASE_SUBCKT::epf req", long_label(), i, lower[i].type(), node(i).type());
      if(node(i).type()){
      }else if(lower[i].type() == 12345){
        node(i).set_type(lower[i].type());
        un->req_type(lower[i].type());
	lower[i] = node(i); // pass to caller.
      }else{
      }

      // un->req_type(lower[i].type());
   // }else if(node(i)->is_node()){
   //   incomplete(); // same as is_node?
    }else if(!node(i).type()){

      trace5("BASE_SUBCKT::epf2", long_label(), i, node(i).is_link(), node(i).type(), lower[i].type());

      if(lower[i].type() == 12345){
        node(i).req_type(lower[i].type());
      //  un->req_type(lower[i].type());
	lower[i] = node(i); // pass to caller.
      }else{
      }


    }else{
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
      incomplete();
      // hmm
    }else if(!node(i).is_link()){
      unreachable();

    }else if(i < num_nodes){
      // TODO: allocate split nodes,
      //       place connect modules as needed
           //  incomplete();
	    // node(i).merge(&s->nodes()->nodes()[i]); // pass to caller.
    }else{
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
    }else if(pi.is_link() && pi.next() != &pi){
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
    }else if(pi.n_()){
      trace3("alloc sckt node: exists", long_label(), pi.short_label(), pi.user_number());
      incomplete();
      unreachable();
      // something with split nodes?
      pi.expand(this);
    }else{
      unreachable();
    }
  }
} // expand_nodes
/*--------------------------------------------------------------------------*/
// used in modelgen models.
// supposedly the same as expand_nodes, but need cleanup
void BASE_SUBCKT::expand_model_nodes()
{
  int num_nodes = subckt()->nodes()->how_many();
  int len = int(subckt()->nodes()->length());
  assert(len<=num_nodes);
  assert(net_nodes()<=len);

  int i=num_nodes;
  for(; i>len; ) {
    --i;
    incomplete();
  }
  for( assert(i==len); i>net_nodes(); ) {
    --i;
    trace3("BASE_SUBCKT::expand_model_nodes internal", i, node(i).is_number(), &node(i));
  }

  return expand_nodes();

  expand_ports();

  trace1("BASE_SUBCKT::expand_model_nodes internal", num_nodes);

  // matrix numbers allocated here, need reverse order
  trace3("alloc modelgen node", long_label(), num_nodes, net_nodes());

  i=num_nodes;
  for(; i>len; ) {
    --i;
    incomplete();
  }
  for( assert(i==len); i>net_nodes(); ) {
    --i;
    NODE_P& pi = subckt()->nodes()->nodes()[i];
   // if(node(i).size()<=1)
    if(pi.is_none()){
      trace2("unused internal model node...", long_label(), i);
    }else{
      trace2("used internal model node...", long_label(), i);
      USER_NODE cn(subckt()->nodes()->proto(i));

      assert(pi.is_link());
      pi.set_node(cn.deflate(&pi, this));
      pi.set_own();
      trace3("alloc model node", long_label(), pi.short_label(), pi.user_number());
      //trace3("allocd model node", long_label(), pi.short_label(), pi.n_()->matrix_number());
      assert(pi.n_());
    }
  }
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::map_nodes()
{
  COMPONENT::map_nodes();
  if(!is_device()){ untested();
  }else if(subckt()) {
    if(subckt()->nodes()){
      subckt()->nodes()->map_nodes();
    }else{
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
