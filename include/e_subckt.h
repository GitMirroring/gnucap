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
/*--------------------------------------------------------------------------*/
class BASE_SUBCKT : public COMPONENT {
  CARD_LIST	_subckt;
protected:
  explicit BASE_SUBCKT(COMMON_COMPONENT* c=nullptr)
    :COMPONENT(c) {
    assert(!_subckt.size());
  }
  explicit BASE_SUBCKT(const BASE_SUBCKT& p)
    :COMPONENT(p) {
      // BUG: copy subckt?
      assert(!_subckt.size());
    }
public:
  ~BASE_SUBCKT() {}
protected: // override virtual
  //char  id_letter()const		//CARD/null
  std::string dev_type()const override{assert(common()); return common()->modelname();}
  int	  tail_size()const override	{return 1;}
  //int	  max_nodes()const		//COMPONENT/null
  //int	  num_nodes()const		//COMPONENT/null
  //int	  min_nodes()const		//COMPONENT/null
  int     matrix_nodes()const override	{return 0;}
public:
  int     net_nodes()const override	{return _net_nodes;}
protected:
  //CARD* clone()const			//CARD/null
  //void  precalc_first()	{assert(subckt()); subckt()->precalc();}
  //void  expand()			//COMPONENT
  //void  precalc_last()	{assert(subckt()); subckt()->precalc();}
  //void  map_nodes();
  void	  tr_begin()override	{assert(subckt()); subckt()->tr_begin();}
  void	  tr_restore()override	{assert(subckt()); subckt()->tr_restore();}
  void	  dc_advance()override	{assert(subckt()); subckt()->dc_advance();}
  void	  tr_advance()override	{assert(subckt()); subckt()->tr_advance();}
  void	  tr_regress()override	{assert(subckt()); subckt()->tr_regress();}
  bool	  tr_needs_eval()const override
	{assert(subckt()); return subckt()->tr_needs_eval();}
  void	  tr_queue_eval()override {assert(subckt()); subckt()->tr_queue_eval();}
  bool	  do_tr()override
	{assert(subckt());set_converged(subckt()->do_tr());return converged();}
  void	  tr_load()override	{assert(subckt()); subckt()->tr_load();}
  TIME_PAIR tr_review()override	{assert(subckt()); return _time_by = subckt()->tr_review();}
  void	  tr_accept()override	{assert(subckt()); subckt()->tr_accept();}
  void	  tr_unload()override	{assert(subckt()); subckt()->tr_unload();}
  void	  dc_final()override	{assert(subckt()); subckt()->dc_final();}
  void	  tr_final()override	{assert(subckt()); subckt()->tr_final();}
  void	  ac_begin()override	{assert(subckt()); subckt()->ac_begin();}
  void	  do_ac()override	{assert(subckt()); subckt()->do_ac();}
  void	  ac_load()override	{assert(subckt()); subckt()->ac_load();}
  void	  ac_final()override	{assert(subckt()); subckt()->ac_final();}
  double  noise_num(std::string const& n)const override {itested(); assert(subckt()); return subckt()->noise_num(n);}
public:
  CARD_LIST*   subckt()override		{return &_subckt;}
  const CARD_LIST*   subckt()const	{return &_subckt;}
  void    new_subckt() {untested();} // obsolete. called from modelgen models
  void	  new_subckt(const CARD* model, PARAM_LIST const* p);
  void	  renew_subckt(const CARD* model, PARAM_LIST const* p);
};
/*--------------------------------------------------------------------------*/
inline void BASE_SUBCKT::new_subckt(const CARD* Model, PARAM_LIST const* Params)
{
  _subckt.erase_all();
  try {
    _subckt.build(Model, this, scope(), Params);
  }catch(Exception const& e){
    _subckt.erase_all();
    throw e;
  }
  _subckt.set_owner(this);
}
/*--------------------------------------------------------------------------*/
inline void BASE_SUBCKT::renew_subckt(const CARD* Model, PARAM_LIST const* Params)
{
  if (_sim->is_first_expand()) {
    new_subckt(Model, Params);
  }else{untested();
    assert(subckt());
    subckt()->attach_params(Params, scope());
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
