/*                          -*- C++ -*-
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
 *------------------------------------------------------------------
 * apply node ordering algorithms
 *
 * some code borrowed from c_delete.cc
 */
#define DO_TRACE
#include "globals.h"
#include "e_cardlist.h"
#include "c_comand.h"
#include "u_nodemap.h"
#include "e_node.h"
#include "e_subckt.h"
#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
static void do_cmk(CARD_LIST* subckt, unsigned net_nodes=0)
{ untested();
//  there should be nodes in _n. these are the ports.
//  more nodes are in scope()->nodes()
//  these need to be sorted somehow.
  size_t how_many=size_t(subckt->nodes()->how_many());
  trace2("finish", net_nodes, subckt->nodes()->how_many());

  for(auto n: *subckt->nodes()){ untested();
    trace3("finish", n.first, n.second->long_label(), n.second->user_number());
  }
  unsigned n=0;
  for(; n<unsigned(net_nodes); ++n){ untested();
//    trace2("user number", n, _n[n].t_());
  }
  for(; n<how_many; ++n){ untested();
    trace1("user number", n);
  }

  int internal_nodes=subckt->nodes()->how_many()-int(net_nodes);
  int port_supernode=(bool)net_nodes;

  boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
    boost::property<boost::vertex_degree_t,int> > g(
      size_t(internal_nodes+port_supernode)); // 1 dummy node for external ports.

  n = unsigned(boost::num_vertices(g));

  trace2("setup", port_supernode, n);

  for(auto i : *subckt){ untested();
    if(!i->is_device()){ untested();
      continue;
    }
    trace2("user number", i->long_label(), i->net_nodes());
    // create a clique for each set of ports
    // this is an overapproximation, exact looks pretty expensive and perhaps
    // make no difference (in most cases)
    for(int j=0; j<i->net_nodes(); ++j){ untested();
      trace1("connectto", i->n_(j).e_());
      for(int k=0; k<j; ++k){ untested();
      	int n1 = i->n_(j).e_();
      	int n2 = i->n_(k).e_();
	if(!n1){ untested();
	  // gnd. ignore.
	}else if(!n2){ untested();
	  // gnd. ignore.
	}else{ untested();
	  // squash ports into one supernode
	  n1 = std::max(0, n1-int(net_nodes)) + port_supernode - 1;
	  n2 = std::max(0, n2-int(net_nodes)) + port_supernode - 1;

	  if(n1!=n2){ untested();
	    assert(n1<int(n));
	    assert(n2<int(n));
	    boost::add_edge(unsigned(n1), unsigned(n2), g);
	  }else{ untested();
	  }
	}
      }
    }
  }

  auto id=boost::get(boost::vertex_index, g);

  std::vector<unsigned> inv_perm(n, -1u);
  std::vector<unsigned> color(n, 0);

  auto colormap=boost::make_iterator_property_map(&color[0], id, color[0]);
  auto degreemap=boost::get(boost::vertex_degree, g);

  if(port_supernode){ untested();
    auto start=*(boost::vertices(g).first); // fix port supernode
    cuthill_mckee_ordering(g, start, inv_perm.begin(), colormap, degreemap);
    assert(id[inv_perm[0]]==0); // external port supernode fixed.
  }else{ untested();
    // do it fully automatically: choose initial node, and do all connected
    // components
    cuthill_mckee_ordering(g, inv_perm.begin(), colormap, degreemap);
  }

#ifdef DO_TRACE
  boost::print_graph(g);
#endif

  for (int c = 0; c<n; ++c){ untested();
    trace3("cmk", c, id[inv_perm[c]], colormap[c]);
  }

  std::vector<unsigned> o(how_many + 1, -1u); // include gnd.

  for (int c = 0; c <=net_nodes; ++c){ untested();
    o[c] = c; // gnd and external ports cannot be moved.
    trace1("fix", c);
  }

  trace3("creating o", o.size(), port_supernode, id[inv_perm[0]]);
  unsigned uncolored=0;
  for (int c = port_supernode; c != inv_perm.size(); ++c){ untested();
    if(id[inv_perm[c]]!=-1u){ untested();
      trace3("map", c, id[inv_perm[c]]+net_nodes, c + net_nodes + 1 - port_supernode);

      assert(id[inv_perm[c]]+net_nodes + 1 - port_supernode < o.size());
	
      o[id[inv_perm[c]]+net_nodes + 1 - port_supernode] = c + net_nodes + 1 - port_supernode;
    }else{ untested();
      assert(port_supernode);
      while(colormap[++uncolored]);
      trace2("not mapped", c, uncolored);
      o[uncolored+net_nodes] = c + net_nodes;
    }
  }

  trace1("o", o.size());
  for( auto idx : o ){ untested();
    trace1("o", idx);
    assert(idx!=-1u);
  }

  // create a permutation p of [0 ... how_many], but fix <net_nodes

  if(subckt==&CARD_LIST::card_list){ untested();
    // TODO: use _sim->_nm instead.
    for(auto i : *subckt){ untested();
      if(!i->is_device()){ untested();
	continue;
      }
      for(int j=0; j<i->net_nodes(); ++j){ untested();
	trace4("b4",i->n_(j).t_(), i->n_(j).e_(),  CKT_BASE::_sim->_total_nodes, n );
	int on= CKT_BASE::_sim->_total_nodes;
	assert(on == 0 ||on==n);
	CKT_BASE::_sim->_total_nodes = n;
	i->n_(j).map_subckt_node((int*)o.data(), NULL);
	CKT_BASE::_sim->_total_nodes = on;
	trace2("",i->n_(j).t_(), i->n_(j).e_());
      }
    }
  }

  subckt->nodes()->permute(o.data()); // change user numbers.

  if(subckt==&CARD_LIST::card_list){ untested();
    for(auto i : *subckt){ untested();
      if(!i->is_device()){ untested();
	continue;
      }
      for(int j=0; j<i->net_nodes(); ++j){ untested();
	trace2("check",i->n_(j).t_(), i->n_(j).e_());
	assert(i->n_(j).t_() == i->n_(j).e_());

      }
    }
  }
}
class CMD_CMK : public CMD {
private:
  void cmk_recursive( CARD_LIST* scope, unsigned net_nodes=0){
    do_cmk(scope, net_nodes);
    for (CARD_LIST::iterator i = scope->begin(); i != scope->end(); ++i) {
      if ((*i)->is_device()){
	// can only do models
      }else if ( BASE_SUBCKT* c = dynamic_cast< BASE_SUBCKT*>(*i) ) {
	cmk_recursive(c->subckt(), (*i)->net_nodes());
      }else{
      }
    }
  }
  bool cmk_one(const std::string& name, CARD_LIST* Scope)const
  { untested();
    assert(Scope);
    
    std::string::size_type dotplace = name.find_first_of(".");
    if (dotplace != std::string::npos) { untested();
      // has a dot, look deeper
      // Split the name into two parts:
      // "container" -- where to look (all following the dot)
      // "dev_name" -- what to look for (all before the dot)
      std::string dev_name  = name.substr(dotplace+1, std::string::npos);
      std::string container = name.substr(0, dotplace);
      // container name must be exact match
      CARD_LIST::iterator i = Scope->find_(container);
      if (i == Scope->end()) { untested();
	// can't find "container" (probably .subckt) - no match
	// try reverse
	dotplace = name.find_last_of(".");
	container = name.substr(dotplace+1, std::string::npos);
	dev_name  = name.substr(0, dotplace);
	// container name must be exact match
	i = Scope->find_(container);
      }else{ untested();
      }
      if (i == Scope->end()) { untested();
	// can't find "container" (probably .subckt) - no match
	return false;
      }else if ((**i).is_device()) { untested();
	// found a match, but it isn't a container (subckt)
	return false;
      }else{ untested();
	// found the container, look inside
	return cmk_one(dev_name, (**i).subckt());
      }
      unreachable();
    }else{ untested();
      // no dots, look here
      if (name.find_first_of("*?") != std::string::npos) { untested();
	// there's a wild card.  do linear search for all matches
	bool didit = false;
	{ untested();
	  CARD_LIST::iterator i = Scope->begin();
	  while (i != Scope->end()) { untested();
	    CARD_LIST::iterator old_i = i++;
	    // ^^^^^^^^^^^^ move i past the item being deleted
	    if (wmatch((**old_i).short_label(), name)) { untested();
	      if((*old_i)->is_device()){ untested();
	      }else if(BASE_SUBCKT* s=dynamic_cast<BASE_SUBCKT*>(*old_i)){ untested();
		do_cmk(s->subckt(), (*old_i)->net_nodes());
		didit = true;
	      }
	    }
	  }
	}
	return didit;
      }else{ untested();
	// no wild card.  fast search for one exact match
	CARD_LIST::iterator i = Scope->find_(name);
	if (i != Scope->end()) { untested();
	  if((*i)->is_device()){ untested();
	  }else if(BASE_SUBCKT* s=dynamic_cast<BASE_SUBCKT*>(*i)){ untested();
	    do_cmk(s->subckt(), (*i)->net_nodes());
	    return true;
	  }
	}else{ untested();
	  return false;
	}
      }
      unreachable();
    }
    unreachable();
    return false;
  }
  //-----------------------------------
  void do_it(CS& cmd, CARD_LIST* Scope)
  { untested();
    if (cmd.umatch(". ")) { untested();
      do_cmk(&CARD_LIST::card_list);
    }else if (cmd.umatch("all ")) { untested();
      cmk_recursive(&CARD_LIST::card_list);
    }else{ untested();
      while (cmd.more()) { untested();
	unsigned mark = cmd.cursor();
	bool didit = cmk_one(cmd.ctos(), Scope);
	if (!didit) { untested();
	  cmd.warn(bWARNING, mark, "no match");
	}
      }
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "cmk|order_cmk", &p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
