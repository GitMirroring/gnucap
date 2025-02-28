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
//testing=script,complete 2006.07.14
#include "e_node.h"
#include "u_nodemap.h"
/*--------------------------------------------------------------------------*/
USER_NODE ground_node("0", 0);
/*--------------------------------------------------------------------------*/
NODE_MAP::NODE_MAP()
{
  _map = new map;
  (*_map)["0"] = 0;
  _nodes.resize(1);
  _nodes.back() = &ground_node; // BUG. ground is global.
}
/*--------------------------------------------------------------------------*/
/* copy constructor: deep copy
 * The std::map copy constructor does a shallow copy,
 * then replace second with a deep copy.
 */
NODE_MAP::NODE_MAP(const NODE_MAP& )
{ untested();
  unreachable();
#if 0
  for (iterator i = _map->begin(); i != _map->end(); ++i) { untested();
    if (i->first != "0") { untested();
      incomplete(); // not used yet.
      // assert(i->second);
      // i->second = new NODE(i->second);
    }else{ untested();
    }
  }
#endif
}
/*--------------------------------------------------------------------------*/
NODE_MAP::~NODE_MAP()
{
  delete _map;
  _map = nullptr;
//  for (iterator i = _node_map.begin(); i != _node_map.end(); ++i) {
//    if (i->first != "0") {
//      assert(i->second);
//      i.second->clear();
//    }else{
//    }
//  }  
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * returns nullptr pointer if no match
 */
NODE* NODE_MAP::operator[](std::string const& s)
{
  assert(_map);
  auto i = _map->find(s);
  if (i != _map->end()) {
    assert(i->second < int(_nodes.size()));
    return _nodes[int(i->second)];
  }else if (OPT::case_insensitive) {
    std::string ls(s);
    notstd::to_lower(&ls);
    i = _map->find(ls);
  }else{
    return nullptr;
  }
  if(i != _map->end()){
    assert(i->second < int(_nodes.size()));
    return _nodes[i->second];
  }else{
    return nullptr;
  }
}
/*--------------------------------------------------------------------------*/
// access by index. this is a stub.
node_t const& NODE_MAP::operator[](int i)const
{ untested();
  assert(i<int(_nodes.size()));
  return _nodes[i];
}
/*--------------------------------------------------------------------------*/
node_t& NODE_MAP::operator[](int i)
{ untested();
  assert(i<int(_nodes.size()));
  return _nodes[i];
}
/*--------------------------------------------------------------------------*/
int NODE_MAP::how_many() const
{
  return int(_nodes.size()-1);
}
/*--------------------------------------------------------------------------*/
/* return a pointer to a node given a string
 * creates a new one if it isn't already there.
 */
NODE* NODE_MAP::new_node(std::string const& S)
{
  std::string s(S);
  if (OPT::case_insensitive) {
    notstd::to_lower(&s);
  }else{
  }
  idx& i = (*_map)[s];

  if (!i.is_valid()) {
    trace2("MAP::new_node", s, how_many());
    // temporary. may need USER_NODE here eventually.
    i = how_many() + 1; // the map number of the new node
    _nodes.push_back(node_t(new USER_NODE(s, i)));
  }else{
  }
  return _nodes[i];
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
