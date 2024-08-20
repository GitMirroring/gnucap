/*$Id: u_nodemap.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
 * Copyright (C) 2002 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
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
 * node name to number mapping -- for named nodes
 */
// #include "e_node.h"
#include "u_node.h"
#include "u_nodemap.h"
#include "e_cardlist.h"
#include "e_card.h" // BUG. scope stuff
/*--------------------------------------------------------------------------*/
NODE_MAP::NODE_MAP()
{
  _node_map = new map_t;
}
/*--------------------------------------------------------------------------*/
/* copy constructor: deep copy
 * The std::map copy constructor does a shallow copy,
 * then replace second with a deep copy.
 * but no map.
 */
NODE_MAP::NODE_MAP(const NODE_MAP& p)
  : _parent(&p)
{
  // build_map(p._node_map)?
  //assert(p._node_map); // for now.
  _nodes.resize(p.how_many());
  assert(p._node_map);
  for(map_value_t& nn : *p._node_map){
    auto u = prechecked_cast<USER_NODE*>(nn.second.n_());
    assert(u);
    int idx = u->user_number();
    assert(nn.second.user_number() == idx);

    // _nodes[idx] = p._nodes[idx].clone(); // type?
    assert(!_nodes[idx].is_link());
    assert(!_nodes[idx].is_node());

    {
     // _nodes[idx] = nn.second;
      _nodes[idx].set_io_link();
     //  _nodes[idx].set_next(NULL);

      _nodes[idx].set_type(nn.second.type());

      if(nn.second.is_ground()){ untested();
	_nodes[idx].set_ground();
      }else if(u->is_grounded()){
	_nodes[idx].set_ground();
      }else{
      }
      // assert(_nodes[idx].size() == 1);
    }

    // trace3("node clone", idx, _nodes[idx].is_ground(), _nodes[idx].short_label());
    trace2("node clone", idx, u->is_grounded());
    // trace2("node clone", idx, nn.first);
  }

  for(auto const& nn : _nodes){
    trace2("node clone", nn.type(), nn.is_ground());
    assert(nn.is_link());
  }
}
/*--------------------------------------------------------------------------*/
NODE_MAP::~NODE_MAP()
{
  if(_parent){
  }else{
    delete _node_map;
    _node_map = NULL;
  }
}
/*--------------------------------------------------------------------------*/
/* return node given a string
 */
NODE_P const& NODE_MAP::operator[](std::string const& s) const
{
  static NODE_P invalid_node;
  assert(!invalid_node.is_node());
  if(_node_map){
    trace2("NODE_MAP::operator[]", s, _node_map->size());
    map_t::const_iterator i = _node_map->find(s);
    if (i != _node_map->end()) {
    }else if (OPT::case_insensitive) {
      std::string s_(s);
      notstd::to_lower(&s_);
      i = _node_map->find(s_);
      if (i == _node_map->end()) {
	return invalid_node;
      }else{
      }
    }else{
      return invalid_node;
    }
    return (*i).second;
  }else if(_parent){
    NODE_P const& pn = (*_parent)[s];
    int u = -1;
    if(pn.is_node()){
      u = pn.user_number();
    }else{
    }
    if(u < 0) {
    }else if(pn.user_number() < int(_nodes.size())){
      return _nodes[u];
    }else{ untested();
    }
    return invalid_node;
  }else{ untested();
    return invalid_node;
  }
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * creates a new one if it isn't already there.
 */
USER_NODE* NODE_MAP::new_node(std::string s, CARD const* d)
{
  assert(_node_map); // TODO: new_model_node
  if(_parent){
    // _parent = NULL;
  }else{
  }

  assert(d->scope());
  assert(d->scope()->nodes());
  assert(d->scope()->nodes() == this);

  trace1("NODE_MAP::new_node", s);
  if (OPT::case_insensitive) {
    notstd::to_lower(&s);
  }else{
  }

  CARD const* node_owner;
  // if(d->makes_own_scope())// ??
  if(d->scope() == d->subckt()){
    node_owner = d;
  }else{
    node_owner = d->owner();
  }

  assert(_node_map);
  NODE_P& node = (*_node_map)[s];

  if (!node.n_()){
    // increment how_many() when lookup fails (new s)
    trace2("new_node, push", s, how_many());
    USER_NODE* nn = new USER_NODE(node_owner, how_many()-1);
    node.set_own(nn); // link nn->_nodes[idx]??
    assert(nn == node.n_());
    _names.push_back(s);
    _nodes.push_back(NODE_P(nn));

    assert(label(how_many()-1) == s);
    // assert(nn->short_label() == s); // nope
    // trace2("NODE_MAP::new_node1", node->short_label(), s);
    assert( node.n_() );
  }else{
    trace2("new_node, already there", s, how_many());
  }

  assert(dynamic_cast<USER_NODE*>(node.n_()));
  return prechecked_cast<USER_NODE*>(node.n_());
}
/*--------------------------------------------------------------------------*/
int NODE_MAP::how_many()const
{
  if(_parent && !_node_map){
    return static_cast<int>(_nodes.size());
  }else{
    return static_cast<int>(_node_map->size());
  }
 //  + _nodes.size());
}
/*--------------------------------------------------------------------------*/
NODE const* NODE_MAP::node(int i) const
{
  assert(i<int(_nodes.size()));
  assert(_nodes[i].n_());
  return _nodes[i].n_();
}
/*--------------------------------------------------------------------------*/
// reset top level node map
// substitutes "clone"
void NODE_MAP::build_map()
{
  trace1("NODE_MAP::build_map", how_many());

  if(_parent){
    for(auto&n : _nodes){
      if(n.is_node()){
	n.set_own(false); // memory leak?
      }else{
      }
    }

    assert(_parent == this);
    assert(int(_nodes.size()) <= how_many());
  }else if(_nodes.size()){
    // top level, adding more after a run
  }else{
  }

  _nodes.resize(0);

  assert(_node_map);
  _nodes.resize(_node_map->size());

  for(auto& n : _nodes){
    // trace0("consistency");
    assert(!n.is_node());
    assert(!n.is_link());
  }

  for(map_value_t& nn : *_node_map){
    int idx = nn.second.user_number();
    // trace3("buildmap", nn.first, idx, _parent);
    assert(idx < int(_nodes.size()));

    assert(_nodes[idx].is_none());
    auto u = prechecked_cast<USER_NODE*>(nn.second.n_());
    assert(u);

    u->link();
    _nodes[idx].set_node(u);
  }

  _parent = this;
}
/*--------------------------------------------------------------------------*/
std::string const& NODE_MAP::label(int i) const
{
  if(i < int(_names.size())){
    return _names[i];
 //  if(_names.size()) {
  }else if(_parent == this){
    // incomplete();
  }else if(_parent){
    return _parent->label(i);
  }else{
    incomplete();
    // return "????" + to_string(i) + "/" + to_string(int(_names.size())) + "????";
  }
  static std::string unreachable="unreachable" + to_string(i);
  return unreachable;
}
/*--------------------------------------------------------------------------*/
void NODE_MAP::map_nodes()
{
  assert(_parent);
  for(auto&n : _nodes){
    n.map();
    assert(n.is_none() || n.is_node());
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
