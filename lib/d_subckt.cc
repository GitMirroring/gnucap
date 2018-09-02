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
#define DO_TRACE
#include "e_node.h"
#include "globals.h"
#include "e_paramlist.h"
#include "e_subckt.h"
#include "u_nodemap.h" // debug
/*--------------------------------------------------------------------------*/
namespace{
/*--------------------------------------------------------------------------*/
#define PORTS_PER_SUBCKT 100
//BUG// fixed limit on number of ports
/*--------------------------------------------------------------------------*/
class COMMON_SUBCKT : public COMMON_PARAMLIST{
public:
  COMMON_SUBCKT(int a) : COMMON_PARAMLIST(a), _proto(nullptr) {}
  COMMON_SUBCKT() : COMMON_PARAMLIST(), _proto(nullptr) {}
  COMMON_SUBCKT(COMMON_SUBCKT const& p)
    : COMMON_PARAMLIST(p), _proto(p._proto) { incomplete();}

  COMMON_COMPONENT* clone()const { untested();
    return new COMMON_SUBCKT(*this);
  }

public: // TODO, only store subcircuit?
  CARD* _proto;
};
/*--------------------------------------------------------------------------*/
COMMON_SUBCKT Default_SUBCKT(CC_STATIC);
/*--------------------------------------------------------------------------*/
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
  int		net_nodes()const	{
    trace2("DEV_SUBCKT::net_nodes", this, common());
//    return _net_nodes;
    return proto()._net_nodes;
  }
  void		precalc_first();
  bool		makes_own_scope()const  {return false;}

  void		expand();
protected:
  DEV_SUBCKT& proto(){ itested();
    COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
    assert(c);
    assert(c->_proto);
    DEV_SUBCKT* d = prechecked_cast<DEV_SUBCKT*>(c->_proto);
    assert(d);
    return *d;
  }
  DEV_SUBCKT const& proto() const{ untested();
    COMMON_SUBCKT const* c = prechecked_cast<COMMON_SUBCKT const*>(common());
    assert(c);
    assert(c->_proto);
    DEV_SUBCKT const* d = prechecked_cast<DEV_SUBCKT const*>(c->_proto);
    assert(d);
    return *d;
  }
private:
  void		precalc_last();
  double	tr_probe_num(const std::string&)const;
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}

  std::string port_name(int i)const;
public:
  static int	count()			{untested();return _count;}
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
		~DEV_SUBCKT_PROTO(){ untested();
		detach_common(); }
public: // override virtual
  char		id_letter()const	{untested();return '\0';}
  CARD*		clone_instance()const;
  bool		print_type_in_spice()const {unreachable(); return false;}
  std::string   value_name()const	{untested();incomplete(); return "";}
  std::string   dev_type()const		{untested(); return "";}
  int		max_nodes()const	{return PORTS_PER_SUBCKT;}
  int		min_nodes()const	{return 0;}
  int		matrix_nodes()const	{untested();return 0;}
  int		net_nodes()const	{ untested();
    return _net_nodes;
  }
  bool node_is_connected(int i)const{ untested();
    return proto().node_is_connected(i);
  }
  CARD*		clone()const{ return new DEV_SUBCKT_PROTO(*this); }
  bool		is_device()const	{return false;}
  bool		makes_own_scope()const  {return true;}

  CARD_LIST*	   scope()		{return proto().subckt();}
  const CARD_LIST* scope()const		{return proto().subckt();}

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

  void set_port_by_index(int index, std::string& value){ untested();
    // redirect to proto? doesn't work because proto has the wrong owner
    COMPONENT::set_port_by_index(index, value);
    proto()._net_nodes=_net_nodes; // needed? (yes)
  }

public: // override CARD
  // used in void node_t::new_node(const std::string& node_name, const CARD* d)
  CARD_LIST* subckt(){
    return proto().subckt();
  }
  const CARD_LIST* subckt()const{untested();
    return proto().subckt();
  }
} pp;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "X|subckt", &pp);
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO(const DEV_SUBCKT_PROTO& p)
  :DEV_SUBCKT(p)
{ untested();

  detach_common();
  COMMON_SUBCKT* n=new COMMON_SUBCKT;
  attach_common(n);

  n->_proto=new DEV_SUBCKT();
  assert(!n->_proto->subckt());

  proto().detach_common();
  proto().attach_common(n);

  proto().new_subckt(); // here?!

  assert(subckt()==n->_proto->subckt());
  // use these nodes when reading in the model
  _n = &proto().n_(0);

  assert(common()==proto().common());
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT_PROTO::DEV_SUBCKT_PROTO()
  :DEV_SUBCKT()
{ untested();
  COMMON_SUBCKT* cp=new COMMON_SUBCKT;
  assert(cp);
  cp->_proto=new DEV_SUBCKT();

  untested();

  detach_common();
  attach_common(cp);
  proto().detach_common();
  proto().attach_common(cp);

  proto().new_subckt(); // here?!

  // use these nodes when reading in the model
  _n = &cp->_proto->n_(0);

  assert(common()==proto().common());
}
/*--------------------------------------------------------------------------*/
CARD* DEV_SUBCKT_PROTO::clone_instance()const
{ untested();
  COMMON_SUBCKT const* cp=dynamic_cast<COMMON_SUBCKT const*>(common());
  assert(cp);

  if (this == &pp){ untested();
    incomplete();
    // cloning from static, empty model
    // look out for _parent in expand
    return p1.clone();

  }else{ untested();
    assert(common()==proto().common());

    DEV_SUBCKT* new_instance = dynamic_cast<DEV_SUBCKT*>(proto().clone());
    assert(new_instance->common()==common());
    assert(new_instance->is_device());
    trace1("new_instance", new_instance->net_nodes());
    return new_instance;
  }
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT()
  :BASE_SUBCKT()
{ untested();
  incomplete();
  trace1("constr DEV_SUBCKT", this);
  attach_common(&Default_SUBCKT);
  _n = _nodes;
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_SUBCKT::DEV_SUBCKT(const DEV_SUBCKT& p)
  :BASE_SUBCKT(p)
{ untested();
  //strcpy(modelname, p.modelname); in common
  for (int ii = 0;  ii < max_nodes();  ++ii) { itested();
    _nodes[ii] = p._nodes[ii];
  }
  _n = _nodes;
  assert(!CARD::subckt());
  ++_count;
  assert(common()==p.common());
  assert(common()!=&Default_SUBCKT);
  trace3("net_nodes", this, common(), &proto());
}
/*--------------------------------------------------------------------------*/
std::string DEV_SUBCKT::port_name(int i)const
{ untested();
  COMMON_SUBCKT const* c = prechecked_cast<COMMON_SUBCKT const*>(common());
  assert(c);
  if (const DEV_SUBCKT* p=dynamic_cast<const DEV_SUBCKT*>(c->_proto)) { untested();
    if (i<p->net_nodes()){ untested();
      return p->port_value(i);
    }else{untested(); 
      return "";
    }
  }else if(c->_proto){untested(); untested();
    // reachable?
    return "";
  }else{untested();
    return "";
  }
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::expand()
{ untested();
  BASE_SUBCKT::expand();
  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
  assert(c);
  if (!c->_proto) { untested();
    incomplete();
    // get here when instanciating X, then set modelname
    assert(c->modelname()!="");
    const CARD* model=find_looking_out(c->modelname());
    BASE_SUBCKT const* proto=dynamic_cast<const BASE_SUBCKT*>(model);
    if(!proto) { untested();
      throw Exception_Type_Mismatch(long_label(), c->modelname(), "subckt");
    }else{ untested();
      COMMON_SUBCKT const* P=dynamic_cast<COMMON_SUBCKT const*>(proto->common());
      c = const_cast<COMMON_SUBCKT*>(P);
      attach_common(c);
    }
  }else{ untested();
    // possible after clone_instance.
    incomplete();
//    assert(find_looking_out(c->modelname()) == c->_proto->common()->modelname());
  }
  
  assert(c->_proto->subckt());
  assert(c->_proto->subckt()->params());
  PARAM_LIST* pl = const_cast<PARAM_LIST*>(c->_proto->subckt()->params());
  assert(pl);
  c->_params.set_try_again(pl);

  renew_subckt(&proto(), &(c->_params));
  subckt()->expand();
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_first()
{ untested();
  BASE_SUBCKT::precalc_first();

  if (subckt()) { untested();
    COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
    assert(c);
    subckt()->attach_params(&(c->_params), scope());
    subckt()->precalc_first();
  }else{ untested();
  }
  assert(!is_constant()); /* because I have more work to do */
}
/*--------------------------------------------------------------------------*/
void DEV_SUBCKT::precalc_last()
{ untested();
  BASE_SUBCKT::precalc_last();

  COMMON_SUBCKT* c = prechecked_cast<COMMON_SUBCKT*>(mutable_common());
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
} // namespace
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
