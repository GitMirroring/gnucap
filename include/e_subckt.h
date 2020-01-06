/*$Id: e_subckt.h,v 26.126 2009/10/16 05:29:28 al Exp $ -*- C++ -*-
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
 * base class for elements made of subcircuits
 */
//testing=script 2006.07.12
#ifndef E_SUBCKT_H
#define E_SUBCKT_H
#include "e_compon.h"
#include "e_node.h"
#include "e_paramlist.h"
#include <memory> // shared_ptr, C++11
/*--------------------------------------------------------------------------*/
class BASE_SUBCKT : public COMPONENT {
protected:
  explicit BASE_SUBCKT()
    :COMPONENT(),
     _subckt(0) {}
  explicit BASE_SUBCKT(const BASE_SUBCKT& p)
    :COMPONENT(p),
     _subckt(0) //BUG// isn't this supposed to copy????
     {}

  ~BASE_SUBCKT() {
    delete _subckt;
  }
protected: // override virtual
  //char  id_letter()const		//CARD/null
  std::string dev_type()const {assert(common()); return common()->modelname();}
  int	  tail_size()const		{return 1;}
  //int	  max_nodes()const		//COMPONENT/null
  //int	  num_nodes()const		//COMPONENT/null
  //int	  min_nodes()const		//COMPONENT/null
  int     matrix_nodes()const		{return 0;}
  int     net_nodes()const		{return _net_nodes;}
  //CARD* clone()const			//CARD/null
  //void  precalc_first()	{assert(subckt()); subckt()->precalc();}
  //void  expand()			//COMPONENT
  //void  precalc_last()	{assert(subckt()); subckt()->precalc();}
  void  map_nodes(){
    COMPONENT::map_nodes();

    if (subckt()) {
      subckt()->map_nodes();
    }else{
    }
  }
  void      tr_iwant_matrix(){
    if (is_device()) {
      assert(matrix_nodes() == 0);
      if (subckt()) {
	subckt()->tr_iwant_matrix();
      }else{untested();
      }
    }else{
    }
  }
  void      ac_iwant_matrix(){
    if (is_device()) {
      assert(matrix_nodes() == 0);
      if (subckt()) {
	subckt()->ac_iwant_matrix();
      }else{untested();
      }
    }else{
    }
  }
  void set_slave(){
    COMPONENT::set_slave();
    if (subckt()) {
      subckt()->set_slave();
    }else{
    }
  }
  void	  tr_begin()	{assert(subckt()); subckt()->tr_begin();}
  void	  tr_restore()	{assert(subckt()); subckt()->tr_restore();}
  void	  dc_advance()	{assert(subckt()); subckt()->dc_advance();}
  void	  tr_advance()	{assert(subckt()); subckt()->tr_advance();}
  void	  tr_regress()	{assert(subckt()); subckt()->tr_regress();}
  bool	  tr_needs_eval()const
	{assert(subckt()); return subckt()->tr_needs_eval();}
  void	  tr_queue_eval() {assert(subckt()); subckt()->tr_queue_eval();}
  bool	  do_tr()
	{assert(subckt());set_converged(subckt()->do_tr());return converged();}
  void	  tr_load()	{assert(subckt()); subckt()->tr_load();}
  TIME_PAIR tr_review()	{assert(subckt()); return _time_by = subckt()->tr_review();}
  void	  tr_accept()	{assert(subckt()); subckt()->tr_accept();}
  void	  tr_unload()	{assert(subckt()); subckt()->tr_unload();}
  void	  ac_begin()	{assert(subckt()); subckt()->ac_begin();}
  void	  do_ac()	{assert(subckt()); subckt()->do_ac();}
  void	  ac_load()	{assert(subckt()); subckt()->ac_load();}
public:
  CARD_LIST*	     subckt(){ return _subckt; }
  const CARD_LIST*   subckt()const{ return _subckt; }
  void	  new_subckt();
  void	  renew_subckt(const CARD* model, PARAM_LIST* p);
#if 0 // forward to COMMON?
  void	  new_subckt(const CARD* model, PARAM_LIST* p);
  void	  new_subckt(const CARD_LIST* model, PARAM_LIST* p);
              //    (forward to COMMON?)
#endif
protected:
  CARD_LIST*	_subckt;
};
/*--------------------------------------------------------------------------*/
class COMMON_SUBCKT : public COMMON_PARAMLIST{
public:
  explicit COMMON_SUBCKT(int c);
  explicit COMMON_SUBCKT(COMMON_PARAMLIST const& s,
                         std::shared_ptr<const CARD_LIST> cl);
  ~COMMON_SUBCKT(){ }
private:
  explicit COMMON_SUBCKT(){unreachable();}
  explicit COMMON_SUBCKT(COMMON_SUBCKT const& s)
    : COMMON_PARAMLIST(s),
      _ports(s._ports),
      _subckt(s._subckt)
  {
    if(_subckt.get()){
      _params.set_try_again(_subckt->params());
    }else{
    }
  }
  COMMON_COMPONENT* clone() const{
    trace1("COMMON_SUBCKT::clone", modelname());
    return new COMMON_SUBCKT(*this);
  }
public:
  bool operator==(const COMMON_COMPONENT& x)const {
    const COMMON_SUBCKT* p = dynamic_cast<const COMMON_SUBCKT*>(&x);
    bool rv = p
      && COMMON_PARAMLIST::operator==(x)
      && _subckt.get() == p->_subckt.get();
    return rv;
  }

public: // overrides
  void set_port_by_index(int Index, std::string& Value);
public: // sckt model
  CARD_LIST* new_subckt(){
    assert(!_subckt.get());
    _subckt = std::make_shared<CARD_LIST>();
    trace3("COMMON_SUBCKT new sckt", this, _subckt.use_count(), _subckt.get());
    _params.set_try_again(_subckt->params());
    return const_cast<CARD_LIST*>(_subckt.get());
  }
   void new_subckt(BASE_SUBCKT* owner, PARAM_LIST* Params) const;
   void renew_subckt(BASE_SUBCKT* owner, PARAM_LIST* Params) const;
  CARD_LIST const* subckt() const{
    return _subckt.get();
  }
  int net_nodes() const;
  std::string port_name(int i) const;
  void map_subckt_nodes(BASE_SUBCKT* owner, CARD_LIST const* sckt=NULL) const;

public:
  std::vector<node_t> _ports;
private:
  std::shared_ptr<const CARD_LIST> _subckt;
}; // COMMON_SUBCKT
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
