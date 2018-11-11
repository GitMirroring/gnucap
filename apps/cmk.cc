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
#include "e_cardlist.h"
#include <gala/boost.h>
#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/graph/graph_utility.hpp>
// #define DO_TRACE
#include "io_trace.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using boost::container::flat_set;
/*--------------------------------------------------------------------------*/
// undirected simple loopless graph
template<class G>
struct uvv_config : gala::graph_cfg_default<G> {
//    static constexpr bool is_directed=false;
    static constexpr bool is_directed=false;
    static constexpr bool force_simple=true;
    static constexpr bool force_loopless=true;
};
/*--------------------------------------------------------------------------*/
template<class G>
struct dpvv_config : uvv_config<G> {
    static constexpr bool force_simple=false;
    static constexpr bool force_symmetric=false;
    static constexpr bool is_directed=true;
};
/*--------------------------------------------------------------------------*/
///typedef boost::adjacency_list
/*--------------------------------------------------------------------------*/
class ORDER_CMK : public ORDERING{
private:
  //typedef gala::graph<boost::container::flat_set, std::vector, unsigned, dpvv_config> graph_in_t;
  typedef gala::graph<std::vector, std::vector, unsigned, uvv_config> graph_t;
  typedef gala::graph<flat_set, std::vector, unsigned, dpvv_config> graph_in_t;
private:
  ORDERING* clone()const{return new ORDER_CMK(*this);}
private:
  std::string name() const{return "rcmk";}
  void init(unsigned total_nodes, NODE_ORDER& no){ untested();
    std::vector<unsigned>& n=nm(no);

    n.resize(total_nodes+1);
    n[0] = 0;
    for (unsigned node = 1; node<=total_nodes; ++node) {
    //   n[node] = unsigned(total_nodes - node) + 1;
      n[node] = node;
    }
    _g.clear();
    _g.reshape(total_nodes);
  }
  void iwant(unsigned a, unsigned b, BSMATRIX_LAYOUT&){
    if(!a){
    }else if(!b){
    }else if(a==b){
      // need diagonal anyway
    }else if(a<b){
      boost::add_edge(a-1, b-1, _g);
    }else{
      boost::add_edge(b-1, a-1, _g);
    }
  }
  void bump(BSMATRIX_LAYOUT& m, NODE_ORDER const&nm){ untested();
    // _g vertices are user_numbers
    auto E=boost::edges(_g);
    for(;E.first!=E.second; ++E.first){
      trace2("bump", boost::target(*E.first, _g), boost::source(*E.first, _g));
      //   _g uses the previous nodemap...
      // _nstat maps matrix numbers to nodes
      //    these nodes know their user number
      //
      m.iwant(nm[_gtou[boost::target(*E.first, _g)]],
              nm[_gtou[boost::source(*E.first, _g)]]);
    }
  }
  void remap(NODE_ORDER& no){ untested();

    // nm maps user_number->matrix_number.
    // iwant has used initial ordering..

    auto n=boost::num_vertices(_g);
    graph_t g(_g);

    trace2("cmk", n, boost::num_edges(g));
    if(boost::num_edges(g)){
    }else{
      return;
    }

    _gtou.resize(n);
    for(unsigned i=0; i<n; ++i){
      assert(no[i+1]);
      _gtou[no[i+1]-1] = i+1;
    }

    auto E=boost::edges(g);
    for(;E.first!=E.second; ++E.first){
      trace2("E",  boost::source(*E.first, g), boost::target(*E.first, g));
    }
//    boost::print_graph(_g);
//    boost::print_graph(g);

    auto id=boost::get(boost::vertex_index, g);
    typedef boost::graph_traits<graph_t>::vertex_descriptor Vertex;

    std::vector<Vertex> inv_perm(n);
    std::vector<unsigned> o(n, -1u);
    std::vector<unsigned> color(n, 0);

    //auto start = 0; // BUG:boost::vertex(0, g);
    auto colormap=boost::make_iterator_property_map(&color[0], id, color[0]);
    auto degreemap=boost::get(boost::vertex_degree, g);
#if 0
    auto start=*(boost::vertices(g).first);
    cuthill_mckee_ordering(g, start, inv_perm.rbegin(), // boost::get(boost::vertex_color, g),
       colormap,	degreemap);

#else
    // figure out a start node.
    cuthill_mckee_ordering(g, inv_perm.rbegin(), colormap, degreemap);
#endif

    for (unsigned c=0; c<inv_perm.size(); ++c){
      trace3("cmk", c, inv_perm[c], colormap[c]);
      if(colormap[c]){
	o[id[inv_perm[c]]] = c;
      }else{
      }
    }

    std::vector<unsigned>& n_=nm(no);
    assert(n_[0]==0);

    for(unsigned i=0; i<n; ++i){
      assert(n_[i+1]);
      trace2("cmk", i, o[i]);

      n_[i+1] = unsigned(1 + o[n_[i+1]-1]);
      // n_[i] = n + 1 - unsigned(1 + o[n_[i]-1]);
    }

//    n=bmd_order(g);
    CARD_LIST::card_list.map_nodes();
  }
private:
  std::string _name;
  graph_in_t _g;
  std::vector<unsigned> _gtou;
  bool _reverse;
}ro;
DISPATCHER<CMD>::INSTALL roi(&order_dispatcher, "rcmk", &ro);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct set_default_order{
  set_default_order(){ untested();
    OPT::order=&ro;
  }
}d;
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
