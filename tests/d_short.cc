/*                            -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * short device: contract nodes in enclosing circuit (if types permit)
 */
#include "globals.h"
#include "e_compon.h"
#include "u_xprobe.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class SHORT : public COMPONENT {
private:
  explicit SHORT(const SHORT& p) :COMPONENT(p) {
	  trace1("SHORT", _net_nodes);
	  for(int i = 0; i<_net_nodes; ++i){
		  _n[i] = p._n[i];
	  }
  }
public:
  explicit SHORT()		:COMPONENT() {}
private: // override virtual
  char	   id_letter()const override	{return '\0';}
  std::string value_name()const override {return "";}
  std::string dev_type()const override	{return "short";}
  int	   max_nodes()const override		{return 17;}
  int	   min_nodes()const override		{return 1;}
  int	   net_nodes()const override		{return _net_nodes;}
  int	   ext_nodes()const override		{return net_nodes();}
  CARD*	   clone()const override		{return new SHORT(*this);}
  void	   tr_iwant_matrix()override	{}
  void	   ac_iwant_matrix()override	{}
  void     map_nodes()override;
  void    expand_first()override;
  void    expand()override;
  void     precalc_last()override;

  double   tr_probe_num(const std::string&)const override;

  node_t _n[17];
  node_t const& n_(int i)const override { assert(i<17); return _n[i];}
  node_t& node(int i)override { assert(i<17); return _n[i]; }
  std::string port_name(int i)const override {
    return "p" + to_string(i);
  }
  bool print_type_in_spice()const override{return false;}
}p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "short", &p1);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void SHORT::expand_first()
{
  trace1("SHORT::expand_first", long_label());
  if (_sim->is_first_expand()) {
	  //node(0).merge(&s->nodes()->nodes()[i]); // pass to caller.
	  assert(net_nodes()==2);
	  if(node(0).is_link()){
	  }else{
	  }
	  for(int i=1; i<net_nodes(); ++i){
		  if(node(1).is_link()){
		  }else{
		  }
		  trace3("SHORT::expand", node(0).is_node(), node(i).is_node(), i);
		 // trace3("SHORT::expand", node(0).user_number(), node(i).user_number(), i);
		  trace3("SHORT::expand", node(0).is_ground(), node(i).is_ground(), i);
		  trace3("SHORT::expand", node(0).is_grounded(), node(i).is_grounded(), i);
		  node(0).merge(&node(i));
	  }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void SHORT::expand()
{
  trace1("SHORT::expand", long_label());
}
/*--------------------------------------------------------------------------*/
void SHORT::precalc_last()
{
  set_constant(true);
  set_converged();
}
/*--------------------------------------------------------------------------*/
double SHORT::tr_probe_num(const std::string& x)const
{ untested();
  if (Umatch(x, "v ")) { untested();
    return _n[0]->v0();
  }else{
	  return COMPONENT::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
void SHORT::map_nodes()
{
  assert(is_device());
  assert(0 <= min_nodes());
  //assert(min_nodes() <= net_nodes());
  assert(net_nodes() <= max_nodes());
  //assert(ext_nodes() + int_nodes() == matrix_nodes());

  for (int ii = 0; ii < ext_nodes()+int_nodes(); ++ii) {
  	 _n[ii].map();
	 assert(ii<2); // for now
	 assert(_n[ii].is_none() || _n[ii].is_node());
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
