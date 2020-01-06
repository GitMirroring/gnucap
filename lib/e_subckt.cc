/*$Id: e_subckt.cc                  -*- C++ -*-
 * Copyright (C) 2001 Albert Davis,
 *               2019, 2020 Felix Salfelder
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
 * Base class for subcircuits in the circuit description file
 */
#include "e_subckt.h"
#include "e_model.h"
#include "u_nodemap.h"
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::new_subckt()
{
  assert(!_subckt);
  delete _subckt;
  _subckt = NULL;
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::new_subckt(BASE_SUBCKT* owner, PARAM_LIST* Params) const
{
  owner->new_subckt();
  CARD_LIST* s = owner->subckt();
  s->attach_params(Params, owner->scope());
  s->shallow_copy(subckt());
  s->set_owner(owner);
  map_subckt_nodes(owner);
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::renew_subckt(const CARD* Model, PARAM_LIST* Params)
{
  unreachable();
  incomplete(); // forward to COMMON?
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::expand_last()
{
  // TODO: move/forward to COMMON?
  auto model = prechecked_cast<COMMON_SUBCKT const*>(common());
  int np = 0;
  if(model){
    np = model->net_nodes();
  }else{
  }
  if(subckt()){
    for(NODE_MAP::const_iterator ii = subckt()->nodes()->begin();
	ii!=subckt()->nodes()->end(); ++ii) {
	trace3("dry", ii->first, ii->second->user_number(),
	                         ii->second->flat_number());
    }
    trace3("placing nodes", long_label(), np, subckt()->nodes()->how_many());
    for(NODE_MAP::const_iterator ii = subckt()->nodes()->begin();
	ii!=subckt()->nodes()->end(); ++ii) {
      int f = ii->second->user_number();
      // assert(f == ii->second->flat_number());
      if(f>np){
	NODE* c = ii->second;
	NODE* nn = c->new_card();
	assert(nn);
	assert(c->data());
	nn->set_owner(this);
	nn->set_label(ii->first);
	trace2("placing node", nn->long_label(), f);
	subckt()->push_back(nn);
      }else{
	trace2("port", ii->first, f);
      }
    }
  }else{
    trace1("no sckt", long_label());
    incomplete();
    // DEV_LOGIC?
  }

}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::renew_subckt(BASE_SUBCKT* owner, PARAM_LIST* Params) const
{
  assert(owner);
  if (_sim->is_first_expand()) { untested();
    new_subckt(owner, Params);
  }else{untested();
    assert(subckt());
    owner->subckt()->attach_params(Params, owner->scope());
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST const* CARD::subckt() const
{
  unreachable(); // this is a compatibility hack, used in some spice devices
  BASE_SUBCKT const* s=dynamic_cast<BASE_SUBCKT const*>(this);
  if(s){
    return s->subckt();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::subckt()
{
  unreachable(); // this is a compatibility hack
  BASE_SUBCKT* s=dynamic_cast<BASE_SUBCKT*>(this);
  if(s){
    return s->subckt();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::set_port_by_index(int Index, std::string& Value){ untested();
  incomplete(); // ports are in subckt->map
  return;
  assert(_ports.size()==size_t(Index)); // sequential assignment only
  node_t n;
  if(Value=="0"){
    n = new /*PORT_*/NODE(Value, 0);
  }else{
    n = new /*PORT_*/NODE(Value, Index+1);
  }
  _ports.push_back(n);
}
/*--------------------------------------------------------------------------*/
COMMON_SUBCKT::COMMON_SUBCKT(int c)
  : COMMON_PARAMLIST(c),
    _subckt(NULL)
{untested();
  trace1("COMMON_SUBCKT", this);
}
/*--------------------------------------------------------------------------*/
std::string COMMON_SUBCKT::port_name(int i) const
{
  if (i<net_nodes()){
    NODE const* nn = prechecked_cast<NODE const*>(_ports[i].n_());
    assert(nn);
    return nn->short_label();
  }else{
    return "";
  }
}
/*--------------------------------------------------------------------------*/
int COMMON_SUBCKT::net_nodes() const
{
  return int(_ports.size());
}
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::map_subckt_nodes(BASE_SUBCKT* owner, CARD_LIST const* sckt_proto) const
{
  COMMON_SUBCKT const* model = this;
  int np = 0;
  if(sckt_proto){ untested();
    // root hack
  }else{ untested();
    assert(model->subckt());
    np = model->net_nodes();
    sckt_proto = model->subckt();
  }
  assert(model);
  assert(sckt_proto->nodes());
  assert(owner);
  assert(owner->subckt());
  trace0(model->long_label().c_str());
  trace0(owner->long_label().c_str());

  CARD_LIST* cl = owner->subckt();

  int num_nodes_in_subckt = sckt_proto->nodes()->how_many();
  trace2("",  model->net_nodes(),  num_nodes_in_subckt);
  assert(np <= num_nodes_in_subckt);
  std::vector<NODE*> map = std::vector<NODE*>(num_nodes_in_subckt+1);
  {
    map[0] = &ground_node;
    // self test: verify that port node numbering is correct
    trace1("ports", model->net_nodes());
    for (int port = 0; port < np; ++port) {
//      assert(model->n_(port).e_() <= num_nodes_in_subckt);
      //assert(model->n_(port).e_() == port+1);
 //     trace3("ports", port, model->n_(port).e_(), owner->n_(port).t_());
    }
    {
      // take care of the "port" nodes (external connections)
      // map them to what the calling circuit wants
      //
      int i=0;
      trace3("ports", owner->long_label(), model->net_nodes(), net_nodes());
      for (i=1; i <= np; ++i) {
	assert(i <= num_nodes_in_subckt);
	map[i] = owner->n_(i-1).n_();
	trace2("ports", i, owner->n_(i-1).t_());
      }

      // collecting ordered nodes from nodemap.
      // this is alphabetic order, presumably. need to assign
      // newnode_subckt() in order of appearance, below
      for(NODE_MAP::const_iterator ii = cl->nodes()->begin();
	    ii!=cl->nodes()->end(); ++ii) {
	int f = ii->second->user_number();
	trace2("collect", ii->first, f);
	assert(f == ii->second->flat_number());
	if(f>np){
	  NODE* c = ii->second;
	  CARD* nn = c; // c->new_card();
	  assert(nn);
	  NODE* nnn = prechecked_cast<NODE*>(nn);
	  map[f] = nnn;
	}else{
	}
      }
    
      // get new node numbers, and assign them to the remaining
      trace3("internal", owner->long_label(), model->net_nodes(), num_nodes_in_subckt);
      for (assert(i==np + 1); i <= num_nodes_in_subckt; ++i) {
	int f = CKT_BASE::_sim->newnode_subckt();
	NODE* nnn = map[i];
	assert(nnn);
	nnn->set_flat_number(f); // TODO: let NODE decide. NODE::expand?
	trace2("new flat", nnn->long_label(), f);
      }
    }
  }
  // "map" now contains pointers to nodes in newly created scope
  // Mapping is done in node_t.

  // scan the list, map the nodes
  for (CARD_LIST::iterator ci = cl->begin(); ci != cl->end(); ++ci) {
    // for each card in card_list
    if (!(*ci)->is_device()) {
      assert(dynamic_cast<MODEL_CARD*>(*ci)
           ||dynamic_cast<NODE*>(*ci));
    }else if (COMPONENT* c=dynamic_cast<COMPONENT*>(*ci) ) {
      for (int ii = 0;  ii < (**ci).net_nodes();  ++ii) {
	// for each connection node in card
	c->n_(ii).map_subckt_node(map.data(), owner);
      }
    }else{
      // component proto
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
