/*$Id: d_subckt.cc  2018/05/27  $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * subcircuit stuff
 * base class for other elements using internal subckts
 * netlist syntax:
 * device: Xxxxx <nodelist> <subckt-name> <args>
 * model:  .subckt <subckt-name> <nodelist>
 *	   (device cards)
 *	   .ends <subckt-name>
 * storage note ...
 * the .subckt always has a comment at the hook point, so a for loop works
 * the expansion (attact to the X) has all comments removed
 *	- need to process the entire ring - for doesn't work
 */
//testing=script 2016.09.16
#include "e_node.h"
#include "globals.h"
#include "e_paramlist.h"
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
static COMMON_PARAMLIST Default_SUBCKT(CC_STATIC);
#define PORTS_PER_SUBCKT 100
//BUG// fixed limit on number of ports
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT : public BASE_SUBCKT {
  friend class DEV_SUBCKT_PROTO;
private:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT();
		~DEV_SUBCKT()		{--_count;}
  CARD*		clone()const		{return new DEV_SUBCKT(*this);}
private: // override virtual
  char		id_letter()const	{return 'X';}
  bool		print_type_in_spice()const {return true;}
  std::string   value_name()const	{return "#";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 0;}
  int		matrix_nodes()const	{return 0;}
  int		net_nodes()const	{return _net_nodes;}
  void		precalc_first();
  bool		makes_own_scope()const  {return false;}

  void          finish();
  void		expand();
private:
  void		precalc_last();
  double	tr_probe_num(const std::string&)const;
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}

  std::string port_name(int i)const;
public:
  static int	count()			{untested();return _count;}
protected:
  const BASE_SUBCKT* _parent;
private:
  node_t	_nodes[PORTS_PER_SUBCKT];
  static int	_count;
} p1;
int DEV_SUBCKT::_count = -1;
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT_PROTO : public DEV_SUBCKT {
private:
  explicit	DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO&p);
public:
  explicit	DEV_SUBCKT_PROTO();
		~DEV_SUBCKT_PROTO(){}
public: // override virtual
  char		id_letter()const	{untested();return '\0';}
  CARD*		clone_instance()const;
  bool		print_type_in_spice()const {unreachable(); return false;}
  std::string   value_name()const	{untested();incomplete(); return "";}
  std::string   dev_type()const		{untested(); return "";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 0;}
  int		matrix_nodes()const	{untested();return 0;}
  int		net_nodes()const	{return _net_nodes;}
  CARD*		clone()const		{return new DEV_SUBCKT_PROTO(*this);}
  bool		is_device()const	{return false;}
  bool		makes_own_scope()const  {return true;}
  CARD_LIST*	   scope()		{untested();return subckt();}
  const CARD_LIST* scope()const		{return subckt();}
private: // no-ops for prototype
  void precalc_first(){}
  void expand(){}
  void precalc_last(){}
  void map_nodes(){}
  void tr_begin(){}
  void tr_load(){}
  TIME_PAIR tr_review(){ return TIME_PAIR(NEVER, NEVER);}
  void tr_accept(){}
  void tr_advance(){}
  void tr_restore(){}
  void tr_regress(){}
  void dc_advance(){}
  void ac_begin(){}
  void do_ac(){}
  void ac_load(){}
  bool do_tr(){ return true;}
  bool tr_needs_eval()const{untested(); return false;}
  void tr_queue_eval(){}
  std::string port_name(int)const {untested();return "";}
} pp;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "X|subckt", &pp);
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO& p)
  :DEV_SUBCKT(p)
{
  new_subckt();
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO()
  :DEV_SUBCKT()
{
  new_subckt();
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT_PROTO::clone_instance()const
{
  DEV_SUBCKT* new_instance = dynamic_cast<DEV_SUBCKT*>(p1.clone());
  assert(!new_instance->subckt());

  if (this == &pp){
    // cloning from static, empty model
    // look out for _parent in expand
  }else{
    new_instance->_parent = this;
  }

  assert(new_instance->is_device());
  return new_instance;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT()
  :BASE_SUBCKT(),
   _parent(NULL)
{
  attach_common(&Default_SUBCKT);
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p),
   _parent(p._parent)
{
  //strcpy(modelname, p.modelname); in common
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  assert(!subckt());
  ++_count;
}
/*--------------------------------------------------------------------------*/
std::string DEV_SUBCKT::port_name(int i)const
{
  if (const DEV_SUBCKT* p=dynamic_cast<const DEV_SUBCKT*>(_parent)) {
    if (i<p->net_nodes()){
      return p->port_value(i);
    }else{untested(); 
      return "";
    }
  }else if(_parent){untested(); untested();
    // reachable?
    return "";
  }else{untested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{
  BASE_SUBCKT::expand();
  COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
  assert(c);
  if (!_parent) {
    // get here when instanciating X, then set modelname
    assert(c->modelname()!="");
    const CARD* model = find_looking_out(c->modelname());
    if(!dynamic_cast<const BASE_SUBCKT*>(model)) {
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt");
    }else{
      _parent = prechecked_cast<const BASE_SUBCKT*>(model);
    }
  }else{
    // possible after clone_instance.
    assert(find_looking_out(c->modelname()) == _parent);
  }
  
  assert(_parent->subckt());
  assert(_parent->subckt()->params());
  PARAM_LIST* pl = const_cast<PARAM_LIST*>(_parent->subckt()->params());
  assert(pl);
  c->_params.set_try_again(pl);

  renew_subckt(_parent, &(c->_params));
  subckt()->expand();
} // DEV_SUBCKT::expand
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_first()
{
  BASE_SUBCKT::precalc_first();

  if (subckt()) {
    COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
    assert(c);
    subckt()->attach_params(&(c->_params), scope());
    subckt()->precalc_first();
  }else{
  }
  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_last()
{
  BASE_SUBCKT::precalc_last();

  COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
  assert(c);
  subckt()->attach_params(&(c->_params), scope());
  subckt()->precalc_last();

  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
double DEV_SUBCKT::tr_probe_num(const std::string& x)const
{untested();
  if (Umatch(x, "p ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"P");
    }      
    return power;
  }else if (Umatch(x, "pd ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PD");
    }      
    return power;
  }else if (Umatch(x, "ps ")) {untested();
    double power = 0.;
    assert(subckt());
    for (CARD_LIST::const_iterator
	   ci = subckt()->begin(); ci != subckt()->end(); ++ci) {untested();
      power += CARD::probe(*ci,"PS");
    }      
    return power;
  }else{untested();
    return COMPONENT::tr_probe_num(x);
  }
  /*NOTREACHED*/
}
/*--------------------------------------------------------------------------*/
}
#define DO_TRACE
#include "io_trace.h"
#include "u_nodemap.h"

#include <boost/graph/cuthill_mckee_ordering.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

void hack_finish(CARD_LIST* subckt, unsigned net_nodes)
{

//  there should be nodes in _n. these are the ports.
//  more nodes are in scope()->nodes()
//  these need to be sorted somehow.
  size_t how_many=size_t(subckt->nodes()->how_many());
  trace2("finish", net_nodes, subckt->nodes()->how_many());

  for(auto n: *subckt->nodes()){
    trace3("finish", n.first, n.second->long_label(), n.second->user_number());
  }
  unsigned n=0;
  for(; n<unsigned(net_nodes); ++n){
//    trace2("user number", n, _n[n].t_());
  }
  for(; n<how_many; ++n){
    trace1("user number", n);
  }

  int internal_nodes=subckt->nodes()->how_many()-net_nodes;
  int port_supernode=(bool)net_nodes;

  boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS,
    boost::property<boost::vertex_degree_t,int> > g(
      size_t(internal_nodes+port_supernode)); // 1 dummy node for external ports.

  n = boost::num_vertices(g);

  trace2("setup", port_supernode, n);

  for(auto i : *subckt){
    if(!i->is_device()){ untested();
      continue;
    }
    trace2("user number", i->long_label(), i->net_nodes());
    // create a clique for each set of ports
    // this is an overapproximation, exact looks pretty expensive and perhaps
    // make no difference (in most cases)
    for(int j=0; j<i->net_nodes(); ++j){
      trace1("connectto", i->n_(j).e_());
      for(int k=0; k<j; ++k){
      	int n1 = i->n_(j).e_();
      	int n2 = i->n_(k).e_();
	if(!n1){
	  // gnd. ignore.
	}else if(!n2){
	  // gnd. ignore.
	}else{
	  n1 = std::max(0, n1-int(net_nodes)) + port_supernode - 1;
	  n2 = std::max(0, n2-int(net_nodes)) + port_supernode - 1;

	  if(n1!=n2){
	    assert(n1<n);
	    assert(n2<n);
	    boost::add_edge(unsigned(n1), unsigned(n2), g);
	  }else{
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

  if(port_supernode){
    auto start=*(boost::vertices(g).first); // fix port supernode
    cuthill_mckee_ordering(g, start, inv_perm.begin(), colormap, degreemap);
    assert(id[inv_perm[0]]==0); // external port supernode fixed.
  }else{
    // do it fully automatically: choose initial node, and do all connected
    // components
    cuthill_mckee_ordering(g, inv_perm.begin(), colormap, degreemap);
  }

#ifdef DO_TRACE
  boost::print_graph(g);
#endif

  for (int c = 0; c<n; ++c){
    trace3("cmk", c, id[inv_perm[c]], colormap[c]);
  }

  std::vector<unsigned> o(how_many + 1, -1u); // include gnd.

  for (int c = 0; c <=net_nodes; ++c){
    o[c] = c; // gnd and external ports cannot be moved.
    trace1("fix", c);
  }

  trace3("creating o", o.size(), port_supernode, id[inv_perm[0]]);
  unsigned uncolored=0;
  for (int c = port_supernode; c != inv_perm.size(); ++c){
    if(id[inv_perm[c]]!=-1u){
      trace3("map", c, id[inv_perm[c]]+net_nodes, c + net_nodes + 1 - port_supernode);

      assert(id[inv_perm[c]]+net_nodes + 1 - port_supernode < o.size());
	
      o[id[inv_perm[c]]+net_nodes + 1 - port_supernode] = c + net_nodes + 1 - port_supernode;
    }else{
      assert(port_supernode);
      while(colormap[++uncolored]);
      trace2("not mapped", c, uncolored);
      o[uncolored+net_nodes] = c + net_nodes;
    }
  }

  trace1("o", o.size());
  for( auto idx : o ){
    trace1("o", idx);
    assert(idx!=-1u);
  }


  // create a permutation p of [0 ... how_many], but fix <net_nodes

  if(subckt==&CARD_LIST::card_list){
    // TODO: use _sim->_nm instead.
    for(auto i : *subckt){
      if(!i->is_device()){ untested();
	continue;
      }
      for(int j=0; j<i->net_nodes(); ++j){
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

  if(subckt==&CARD_LIST::card_list){
    for(auto i : *subckt){
      if(!i->is_device()){ untested();
	continue;
      }
      for(int j=0; j<i->net_nodes(); ++j){
	trace2("check",i->n_(j).t_(), i->n_(j).e_());
	assert(i->n_(j).t_() == i->n_(j).e_());

      }
    }
  }
}

namespace{

// reorder nodes
// by degree: need degrees of each node.
//  === MEANT TO BE OPTIONAL/PLUGIN SPACE ===
void DEV_SUBCKT::finish()
{ untested();
  if(subckt()!=scope()){
    // not building netlist.
    return;
  }
  hack_finish(subckt(), net_nodes());
}
} // namespace
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
