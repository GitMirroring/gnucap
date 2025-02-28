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
//testing=script,complete 2006.07.14
#ifndef U_NODEMAP_H
#define U_NODEMAP_H
#include "md.h"
/*--------------------------------------------------------------------------*/
class NODE;
class node_t;
class NODE_MAP;
/*--------------------------------------------------------------------------*/
// make it look like an ordinary map.
// this kind of stuff happens when exposing STL interfaces...
// (it is used in u_probe, refactor later. maybe delete this.)
template<class ITER, class VALUE>
class WRAP_MAP_ITERATOR{
public:
  typedef std::pair<std::string, VALUE> value_type;
protected:
  WRAP_MAP_ITERATOR(ITER i, NODE_MAP const&m)
    :_i(i), _m(m){ }
public: // iterator
  std::pair<std::string, VALUE> operator*() const;
  bool operator==(WRAP_MAP_ITERATOR const& i) const{
    return(_i == i._i);
  }
  bool operator!=(WRAP_MAP_ITERATOR const& i) const{
    return(_i != i._i);
  }
  WRAP_MAP_ITERATOR& operator++(){
    ++_i;
    return *this;
  }
  WRAP_MAP_ITERATOR& operator--(){ untested();
    --_i;
    return *this;
  }
private:
  ITER _i;
  NODE_MAP const& _m;
  friend class NODE_MAP;
};
/*--------------------------------------------------------------------------*/
class NODE_MAP {
  class idx_t{
    int _i;
  public:
    idx_t() : _i(-1) {}
    operator int&(){return _i;}
    operator int const&()const {return _i;}
    idx_t& operator=(int i) {_i=i; return *this;}
    bool is_valid()const {return _i>=0;}
  };
  typedef std::map<const std::string, idx_t> map;
  typedef std::vector<node_t> vector;
public:
  typedef WRAP_MAP_ITERATOR<map::iterator, NODE*> iterator;
  typedef WRAP_MAP_ITERATOR<map::const_iterator, NODE const*> const_iterator;
private:
  map* _map;
  vector _nodes;

public:
  explicit  NODE_MAP(const NODE_MAP&);

public:
  explicit  NODE_MAP();
	   ~NODE_MAP();
  NODE*     operator[](std::string const&);
 // NODE*     operator[](int i);
  node_t    const& operator[](int i)const;
  node_t&          operator[](int i);
  node_t const&    new_node(std::string const&);

  iterator begin();
  iterator end();
  const_iterator begin()const;
  const_iterator end()const;
  int		 how_many()const;

  int index_of(node_t const&)const;
};
/*--------------------------------------------------------------------------*/
template<class ITER, class VALUE>
inline std::pair<std::string, VALUE>
WRAP_MAP_ITERATOR<ITER, VALUE>::operator*() const
{
  return value_type(_i->first, _m[_i->second]);
}
/*--------------------------------------------------------------------------*/
inline NODE_MAP::iterator NODE_MAP::begin()
{
  assert(_map);
  return iterator(_map->begin(), *this);
}
/*--------------------------------------------------------------------------*/
inline NODE_MAP::iterator NODE_MAP::end()
{
  assert(_map);
  return iterator(_map->end(), *this);
}
/*--------------------------------------------------------------------------*/
inline NODE_MAP::const_iterator NODE_MAP::begin()const
{
  assert(_map);
  return const_iterator(_map->begin(), *this);
}
/*--------------------------------------------------------------------------*/
inline NODE_MAP::const_iterator NODE_MAP::end()const
{
  assert(_map);
  return const_iterator(_map->end(), *this);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
