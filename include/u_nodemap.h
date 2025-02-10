/*$Id: u_nodemap.h,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
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
#ifndef U_NODEMAP_H
#define U_NODEMAP_H
#include "md.h"
#include "e_base.h"
#include "u_node.h" // BUG
#include "e_bits.h" // BUG
// #include <set>
/*--------------------------------------------------------------------------*/
class NODE;
class CARD_LIST;
class NODE_P;
/*--------------------------------------------------------------------------*/
// 2 modes.
// - prototype:
//    _node_map with USER_NODEs in it
// - instance:
//    _nodes with nodes in it.
//    _node_map points to parent->_node_map
//    additional non-parent nodes: linear search
//    _nodes may be copied around.
/*--------------------------------------------------------------------------*/
class NODE_MAP {
private:
  typedef std::vector<std::string> names_t;
  typedef std::vector<NODE_P> nodes_t;

  // could use names in names_t. but difficult with std::map
  // resort to duplicate string storage for now
  // also, could map to int and use node vector
  typedef std::map<std::string, NODE_P> map_t;

  typedef map_t::iterator map_iterator;
  typedef map_t::value_type map_value_t;
public:
  typedef map_t::iterator iterator;

  class const_iterator {
    friend class NODE_MAP;
    map_t::const_iterator _i;
    map_t::const_iterator _e;

    typedef std::pair<std::string, NODE const*> value_type;
    value_type _v;
    nodes_t const& _nodes;

  private:
    explicit const_iterator(
	map_t::const_iterator const& i,
	map_t::const_iterator const& e,
	nodes_t const& v)
      : _i(i), _e(e), _nodes(v) {
      skip_unused();
    }
    void skip_unused() {
      while(_i!=_e){
	if(_i->second.is_connected()){
	  break;
	}else{
	  ++_i;
	}
      }
    }

  public:
    const_iterator(const_iterator const& i)
      : _i(i._i), _e(i._e), _nodes(i._nodes) {
    }

    value_type const operator*(){ untested();
      int u = _i->second.user_number();
      assert(u>=0);
      if(_nodes[u].n_()){ untested();
	return std::make_pair(_i->first, _nodes[u].n_());
      }else{ untested();
	return std::make_pair(_i->first, _i->second.n_());
      }
    }
    value_type* operator->(){
      int u = _i->second.user_number();
      trace2("->", _i->first, u);
      assert(u < int(_nodes.size()));
      if(_nodes.size() <= size_t(u)){ untested();
	_v = std::make_pair(_i->first, _i->second.n_());
      }else if(_nodes[u].is_node()){
	_v = std::make_pair(_i->first, _nodes[u].n_());
      }else if(_nodes[u].is_none()){
	_v = std::make_pair(_i->first, (NODE*)NULL);
      }else{ untested();
	_v = std::make_pair(_i->first, _nodes[u].operator->());
      }
      return &_v;
    }

  public:
    bool operator != (const_iterator const&i)const { return i._i != _i; }
    bool operator == (const_iterator const&i)const { return i._i == _i; }
    const_iterator& operator++(){ ++_i; skip_unused(); return *this; }
  };
private:
  // one of {
  map_t* _node_map{NULL};
  NODE_MAP const* _parent{NULL}; // or pass context?
  // }

  nodes_t _nodes;
  names_t _names;

  explicit NODE_MAP(const NODE_MAP&);
public:
  explicit  NODE_MAP();
	   ~NODE_MAP();
  NODE_MAP* clone()const{
    return new NODE_MAP(*this);
  }
  NODE_P const& operator[](std::string const&)const;
 // NODE_P& operator[](std::string const&);
//  std::string operator[](int)const;
  USER_NODE* new_node(std::string, CARD const* d);
  NODE const* node(int i)const;
  NODE_P* map() {
    // assert(_parent);
    return _nodes.data();
  }
  NODE_P* nodes() {
    assert(_parent);
    return _nodes.data();
  }
  USER_NODE const* proto(int i) {
    assert(_parent);
    auto u = prechecked_cast<USER_NODE const*>(_parent->node(i));
    assert(u);
    return u;
  }
  void build_map();
  void map_nodes();

  size_t length()const {return _nodes.size();}
  NODE_P const& operator[](int const& i)const {return _nodes[i];}

  const_iterator begin()const {
    return const_iterator(parent_nm()->begin(), parent_nm()->end(), _nodes);
  }
  const_iterator end()const {
    return const_iterator(parent_nm()->end(), parent_nm()->end(), _nodes);
  }

  // yikes. exposing std::map
  iterator begin()		{assert(_node_map); return _node_map->begin();}
  iterator end()		{assert(_node_map); return _node_map->end();}

  int how_many()const;
  map_t const* node_map() const{
    if(_node_map){
      return _node_map;
    }else if(_parent){
      return _parent->node_map();
    }else{
      incomplete(); // needed?
      return NULL;
    }
  }
  std::string const& label(int i) const;
private:
  map_t* parent_nm() {
    trace2("pnm", _nodes.size(), _node_map);
    if(_parent == this){ untested();
      assert(_node_map);
      return _node_map;
    }else if(_parent){ untested();
      unreachable();
    //  assert( _parent->parent_nm() );
    //  return _parent->parent_nm();
    }else{ untested();
      return _node_map;
      incomplete(); // needed?
      return NULL;
    }
  }
  map_t const* parent_nm() const{
    trace2("pnm", _nodes.size(), _node_map);
    if(_parent == this){
      assert(_node_map);
      return _node_map;
    }else if(_parent){
      assert( _parent->parent_nm() );
      return _parent->parent_nm();
    }else{
      return _node_map;
      incomplete(); // needed?
      return NULL;
    }
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
