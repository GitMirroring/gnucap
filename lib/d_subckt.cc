/*$Id: d_subckt.cc  2018/05/27  $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2019 Felix Salfelder
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
#include "u_nodemap.h"
#include <memory> // C++11
#ifndef NDEBUG
#include "e_model.h"
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static COMMON_PARAMLIST Default_SUBCKT(CC_STATIC);
static COMMON_SUBCKT Default_SUBCKT_(CC_STATIC);
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
class DEV_SUBCKT : public BASE_SUBCKT {
protected:
  explicit	DEV_SUBCKT(const DEV_SUBCKT&);
public:
  explicit	DEV_SUBCKT();
  virtual ~DEV_SUBCKT()		{--_count;}
  CARD*		clone()const		{return new DEV_SUBCKT(*this);}
private: // override virtual
  char		id_letter()const	{return 'X';}
  bool		print_type_in_spice()const {untested(); return true;}
  std::string   value_name()const	{return "#";}
  node_t& n_(int i) const { return const_cast<node_t&>(_nodes[i]); }
  int		max_nodes()const	{
    COMMON_SUBCKT const* cs = dynamic_cast<COMMON_SUBCKT const*>(common());
    if(cs){ untested();
      trace2("DS::max_nodes", long_label(), cs->net_nodes());
      return cs->net_nodes()+1;
    }else{ untested();
      // INT_MAX results in arithmetic overflow in lang_spice
      // still, needed in spice, where ports are assigned before type is known
      return INT_MAX/2;
    }
  }

public:
  void set_port_by_index(int Index, std::string& Value){ untested();
    _nodes.resize(std::max(size_t(Index)+1, _nodes.size()));
    trace3("resized", long_label(), Index, _nodes.capacity());

    BASE_SUBCKT::set_port_by_index(Index, Value);
  }

  // override. the base class does not know about _parent.
  void set_port_by_name(std::string& int_name, std::string& ext_name) {
    int max = max_nodes();

    for (int i=0; i<max; ++i) {
      if (int_name == port_name(i)) {
	set_port_by_index(i, ext_name);
	return;
      }else{
      }
    }
    untested();
    throw Exception_No_Match(int_name);
  }
private:
  int		min_nodes()const	{return 0;}
  int    	ext_nodes()const	{ /* 0?? */  return net_nodes();}
  int		matrix_nodes()const	{return 0;}
  int		net_nodes()const	{return _net_nodes;}
  void		precalc_first();
  bool		makes_own_scope()const  {return false;}

  void		expand();
private:
  void		precalc_last();
  double	tr_probe_num(const std::string&)const;
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}

  std::string port_name(int i)const;
public:
  static int	count()			{untested();return _count;}
private:
  std::vector<node_t> _nodes;
  static int	_count;

} p1;
DISPATCHER<CARD>::INSTALL d0(&device_dispatcher, "X", &p1);
int DEV_SUBCKT::_count = -1;
/*--------------------------------------------------------------------------*/
// inherit from BASE_SUBCKT?
class DEV_SUBCKT_PROTO : public DEV_SUBCKT {
private:
  explicit	DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO&p);
public:
  explicit	DEV_SUBCKT_PROTO();
		~DEV_SUBCKT_PROTO(){
		  _subckt=NULL; // owned by common.
		}
public: // override virtual
  char		id_letter()const	{untested();return '\0';}
  CARD*		clone_instance()const;
  bool		print_type_in_spice()const {unreachable(); return false;}
  std::string   value_name()const	{untested();incomplete(); return "";}
  std::string   dev_type()const		{untested(); return "";}
  node_t& n_(int i) const{ untested();
    COMMON_SUBCKT const* cs = prechecked_cast<COMMON_SUBCKT const*>(common());
    assert(cs);
    return const_cast<node_t&>(cs->_ports[i]);
  }
  int	ext_nodes()const	{ untested(); /* 0?? */  return net_nodes();}
  int		min_nodes()const	{return 0;}
  int		matrix_nodes()const	{untested();return 0;}
  int		net_nodes()const;
  int		max_nodes()const	{return INT_MAX/2;}
  CARD*		clone()const		{return new DEV_SUBCKT_PROTO(*this); }
  bool		is_device()const	{return false;}
  bool		makes_own_scope()const  {return true;}
  CARD_LIST*	   scope();
  const CARD_LIST* scope()const;
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
  void set_port_by_index(int Index, std::string& Value);
} pp;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "subckt", &pp);
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT_PROTO::set_port_by_index(int Index, std::string& Value)
{ untested();
  COMMON_SUBCKT* cs = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
  assert(cs);

  cs->_ports.resize(std::max(size_t(Index)+1, cs->_ports.size()));
  trace3("resized", long_label(), Index, cs->_ports.capacity());

  BASE_SUBCKT::set_port_by_index(Index, Value);
  assert(cs->_ports.size() == size_t(_net_nodes));
}
/*--------------------------------------------------------------------------*/
int DEV_SUBCKT_PROTO::net_nodes() const
{ untested();
  return _net_nodes;
  COMMON_SUBCKT const* cs = prechecked_cast<COMMON_SUBCKT const*>(common());
  assert(cs);
  return int(cs->_ports.size());
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO& p)
  : DEV_SUBCKT(p)
{ untested();
  assert(common());
  COMMON_SUBCKT* cs = dynamic_cast<COMMON_SUBCKT*>(p.common()->clone());
  assert(cs);
  if(!_subckt){
    _subckt = cs->new_subckt();
  }else{
  }
  attach_common(cs);
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO()
  :DEV_SUBCKT()
{ untested();
  attach_common(&Default_SUBCKT_);
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT_PROTO::clone_instance()const
{
  incomplete();
  DEV_SUBCKT* new_instance = dynamic_cast<DEV_SUBCKT*>(p1.clone());
  assert(!new_instance->subckt());

  if (this == &pp){ untested();
    unreachable();
  }else{ untested();
  }
  new_instance->attach_common(common()->clone());

  assert(new_instance->is_device());
  return new_instance;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT()
  :BASE_SUBCKT()
{
  trace2("DEV_SUBCKT", this, &Default_SUBCKT);
  attach_common(&Default_SUBCKT);
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p)
{
  _nodes = p._nodes;
  assert(!subckt());
  ++_count;
  trace1("DEV_SUBCKT clone", p.dev_type());
}
/*--------------------------------------------------------------------------*/
std::string DEV_SUBCKT::port_name(int i)const
{
   COMMON_SUBCKT const* cs = prechecked_cast<COMMON_SUBCKT const*>(common());
   if (cs) { untested();
     return cs->port_name(i);
   }else{
    return "";
   }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{
  BASE_SUBCKT::expand();
  if(subckt()){
    // all set
  }else{
    if(dynamic_cast<COMMON_SUBCKT const*>(common())){ untested();
      // getting here if something went wrong
      // or if subckt proto is a stub.
    }else{ untested();
      // get here when instanciating X, then set modelname
      // X has a COMMON_PARAMLIST
    }
    COMMON_PARAMLIST const* c=dynamic_cast<COMMON_PARAMLIST const*>(common());
    assert(c->modelname()!="");
    const CARD* model = find_looking_out(c->modelname());
    if(!dynamic_cast<const BASE_SUBCKT*>(model)) {
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt");
    }else{
      // lookup model and create common subcircuit with parameters attached.
      BASE_SUBCKT const* parent = prechecked_cast<BASE_SUBCKT const*>(model);
      assert(parent);

      COMMON_COMPONENT* new_common = parent->common()->clone();
      new_common->set_modelname(c->modelname());
      COMMON_SUBCKT* m=dynamic_cast<COMMON_SUBCKT*>(new_common);
      assert(m);
      m->_params = c->_params;
      m->_params.set_try_again(m->subckt()->params());
      attach_common(new_common);
    }
  }

  COMMON_SUBCKT* m=dynamic_cast<COMMON_SUBCKT*>(mutable_common());
  assert(m);
  assert(m->subckt());

  if(m->net_nodes() > int(_nodes.size())){
    _nodes.resize(m->net_nodes());
  }else{
  }

  COMMON_SUBCKT* cs = prechecked_cast<COMMON_SUBCKT*>(mutable_common());

  //  cs->expand(this);
  {
    //  renew_subckt(_parent, &(c->_params));
    PARAM_LIST* p = &(cs->_params);
    if (_sim->is_first_expand()) {
      trace1("isfirst", subckt());
      if(subckt()){
	delete _subckt;
	_subckt = NULL;
      }else{
      }
      new_subckt();
      CARD_LIST* s = subckt();
      s->attach_params(p, scope());
      s->shallow_copy(cs->subckt());
      s->set_owner(this);
      cs->map_subckt_nodes(this);
    }else{untested();
      assert(subckt());
      subckt()->attach_params(p, scope());
    }
  }
  subckt()->expand();
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_first()
{ untested();
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
CARD_LIST* DEV_SUBCKT_PROTO::scope()
{ untested();
#ifdef NDEBUG
  COMMON_SUBCKT const* cs = prechecked_cast<COMMON_SUBCKT const*>(common());
  assert(_subckt);
  assert(_subckt == cs->subckt()); // but const.
#endif
  return _subckt;
}
/*--------------------------------------------------------------------------*/
CARD_LIST const* DEV_SUBCKT_PROTO::scope() const
{ untested();
  COMMON_SUBCKT const* cs = prechecked_cast<COMMON_SUBCKT const*>(common());
  assert(_subckt);
  assert(_subckt == cs->subckt()); // but const.
  return cs->subckt();
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
} // namespace
/*--------------------------------------------------------------------------*/
void COMMON_SUBCKT::map_subckt_nodes(BASE_SUBCKT* owner) const
{
  COMMON_SUBCKT const* model = this;
  assert(model);
  assert(model->subckt());
  assert(model->subckt()->nodes());
  assert(owner);
  assert(owner->subckt());
  trace0(model->long_label().c_str());
  trace0(owner->long_label().c_str());

  CARD_LIST* cl = owner->subckt();

  int num_nodes_in_subckt = model->subckt()->nodes()->how_many();
  trace2("",  model->net_nodes(),  num_nodes_in_subckt);
  assert(model->net_nodes() <= num_nodes_in_subckt);
  std::vector<NODE*> map = std::vector<NODE*>(num_nodes_in_subckt+1);
  {
    map[0] = &ground_node;
    // self test: verify that port node numbering is correct
    trace1("ports", model->net_nodes());
    for (int port = 0; port < model->net_nodes(); ++port) {
//      assert(model->n_(port).e_() <= num_nodes_in_subckt);
      //assert(model->n_(port).e_() == port+1);
 //     trace3("ports", port, model->n_(port).e_(), owner->n_(port).t_());
    }
    {
      // take care of the "port" nodes (external connections)
      // map them to what the calling circuit wants
      //
      // this is a bug.
      //if(net_nodes() < model->net_nodes()){
      //  throw Exception(long_label() + ": need more nodes");
      //}
      int i=0;
      trace3("ports", owner->long_label(), model->net_nodes(), net_nodes());
      for (i=1; i <= model->net_nodes(); ++i) { untested();
	assert(i <= num_nodes_in_subckt);
	map[i] = owner->n_(i-1).n_();
	trace2("ports", i, owner->n_(i-1).t_());
      }

      // collecting ordered nodes from nodemap. clone internal nodes.
      // this is alphabetic order, presumably. need to assign
      // newnode_subckt() in order of appearance, below
      for(auto ii : (*model->subckt()->nodes())){ itested();
	int f = ii.second->user_number();
	assert(f == ii.second->flat_number());
	// if(f>model->net_nodes())
	if(f>model->net_nodes()){
	  CARD const* c = ii.second;
	  CARD* nn = c->clone();
	  assert(nn);
	  nn->set_owner(owner);
	  cl->push_back(nn);
	  NODE* nnn = prechecked_cast<NODE*>(nn);
	  map[f] = nnn;
	}else{
	}
      }
    
      // get new node numbers, and assign them to the remaining
      trace3("internal", owner->long_label(), model->net_nodes(), num_nodes_in_subckt);
      for (assert(i==model->net_nodes() + 1); i <= num_nodes_in_subckt; ++i) {
	int f = CKT_BASE::_sim->newnode_subckt();
	NODE* nnn = map[i];
	assert(nnn);
	nnn->set_flat_number(f); // TODO: let NODE decide. NODE::expand?
	nnn->set_user_number(f); // TODO: let NODE decide. NODE::expand?
      }
    }
  }
  // "map" now contains pointers to nodes in newly created scope
  // Mapping is done in node_t.

  // scan the list, map the nodes
  for (CARD_LIST::iterator ci = cl->begin(); ci != cl->end(); ++ci) { untested();
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
// vim:ts=8:sw=2:noet:
