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
#include "e_node.h" // node_t
/*--------------------------------------------------------------------------*/
class NODE;
class node_t;
class NODE_MAP;
/*--------------------------------------------------------------------------*/
// USER_NODE is permanent, and admits probes.
// .. refers to the NODE used in simulation,
// has an index. (NODE does not), stored in a node_t.
// mapping as in a 1-net-node device.
class USER_NODE : public NODE {
  mutable node_t _n;
  bool _global{false};
public:
  explicit USER_NODE(std::string const& s, int i=INVALID_NODE)
    : NODE(s), _n(i) {
    assert(_n.t_() == i);
  }
public:
  int user_number()const override {return _n.t_();}
  void set_to_ground() { untested(); _global=true; _n.set_to_ground(nullptr); }
  bool is_global()const {return _global;}
  // int matrix_number()const override {untested(); return _n.m_();} // ??
private: // probes
  double	tr_probe_num(const std::string& s)const override;
  XPROBE	ac_probe_ext(const std::string&)const override;
public: // connection
  int net_nodes()const override {return 1;}
  node_t& n_(int i)const override {
    (void)i;
    assert(i==0);
    return _n;
  }
  void map_nodes()override {
    _n.map();
  }
};
/*--------------------------------------------------------------------------*/
extern USER_NODE ground_node;
/*--------------------------------------------------------------------------*/
class NODE_MAP {
  typedef std::map<std::string, USER_NODE*> map;
  typedef std::vector<node_t> vector;
public:
  typedef map::iterator iterator;
  typedef map::const_iterator const_iterator;
private:
  map* _map;
  vector _nodes;

public:
  explicit  NODE_MAP(const NODE_MAP&);

public:
  explicit  NODE_MAP();
	   ~NODE_MAP();
  NODE*     operator[](std::string);
 // NODE*     operator[](int i);
  node_t    const& at(int i)const;
  node_t&          at(int i);
  node_t    const& operator[](int i)const { untested();return at(i);}
  node_t&          operator[](int i) {return at(i);}
  USER_NODE*       new_node(std::string);

  iterator begin() {assert(_map); return _map->begin();}
  iterator end() {assert(_map); return _map->end();}
  const_iterator begin()const {assert(_map); return _map->begin();}
  const_iterator end()const {assert(_map); return _map->end();}
  int		 size()const {return int(_nodes.size());}

  std::string const& name(int)const;
  void map_nodes();
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
