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
#include <boost/container/flat_set.hpp>
#include <boost/graph/minimum_degree_ordering.hpp>
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using boost::container::flat_set;
/*--------------------------------------------------------------------------*/
// undirected simple loopless graph
template<class G>
struct uvv_config : gala::graph_cfg_default<G> {
//    static constexpr bool is_directed=false;
    static constexpr bool is_directed=true;
    static constexpr bool force_symmetric=true;
    static constexpr bool force_simple=true;
    static constexpr bool force_loopless=true;
};

template<class G>
struct dpvv_config : uvv_config<G> {
    static constexpr bool force_simple=false;
    static constexpr bool force_symmetric=false;
    static constexpr bool is_directed=true;
};
/*--------------------------------------------------------------------------*/
class ORDER_BMD : public ORDERING{
private:
  typedef gala::graph<std::vector, std::vector, unsigned, uvv_config> graph_t;
  typedef gala::graph<flat_set, std::vector, unsigned, dpvv_config> graph_in_t;
private:
  ORDERING* clone()const{return new ORDER_BMD(*this);}
private:
  std::string name() const{return "bmd";}
  void init(unsigned total_nodes, NODE_ORDER& no){
    std::vector<unsigned>& n=nm(no);

    n.resize(total_nodes+1);
    n[0] = 0;
    for (unsigned node = 1; node<=total_nodes; ++node) {
      n[node] = unsigned(total_nodes - node) + 1;
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
  void remap(NODE_ORDER& no){

    // nm maps user_number->matrix_number.
    // iwant has used initial ordering..

    auto n=boost::num_vertices(_g);
    graph_t g(_g);

    _gtou.resize(n);
    for(unsigned i=0; i<n; ++i){
      assert(no[i+1]);
      _gtou[no[i+1]-1] = i+1;
    }

    if(0){
      auto E=boost::edges(g);
      for(;E.first!=E.second; ++E.first){
	std::cout << boost::source(*E.first, g) << " ";
	std::cout << boost::target(*E.first, g) << "\n";
      }
    }

    auto id=boost::get(boost::vertex_index, g);
    std::vector<int> supernode_sizes(n, 1);
    std::vector<int> degree(n, 0);
    std::vector<int> io(n, 0);
    std::vector<int> o(n, 0);
    boost::minimum_degree_ordering (g,
		 boost::make_iterator_property_map(&degree[0], id, degree[0]),
		 &io[0],
		 &o[0],
		 boost::make_iterator_property_map(&supernode_sizes[0], id, supernode_sizes[0]),
		 0,
		 id
		);

    std::vector<unsigned>& n_=nm(no);
    assert(n_[0]==0);


    for(unsigned i=1; i<=n; ++i){
      assert(n_[i]);

      n_[i] = unsigned(1 + o[n_[i]-1]);
      // n_[i] = n+1-unsigned(1 + io[n_[i]-1]);
      // n_[i] = unsigned(1 + io[n_[i]-1]);
      // n_[i] = n + 1 - unsigned(1 + o[n_[i]-1]);
    }

//    n=bmd_order(g);
    CARD_LIST::card_list.map_nodes();
  }
private:
  std::string _name;
  graph_in_t _g;
  std::vector<unsigned> _gtou;
}ro;
DISPATCHER<CMD>::INSTALL roi(&order_dispatcher, "bmd", &ro);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
struct set_default_order{
  set_default_order(){
    OPT::order=&ro;
  }
}d;
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
