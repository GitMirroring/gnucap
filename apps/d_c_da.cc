/*$Id: d_d_a.cc  $ -*- C++ -*-
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
 * D to A connect module
 * event triggered, analog out
 */
//testing=none
#include "globals.h"
#include "e_logicmod.h"
#include "e_logic.h"
#include "u_xprobe.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_D_A : public ELEMENT {
public:
  enum {UPPER=0, LOWER=1, OUTNODE=UPPER, INNODE=LOWER}; //node labels
private:
  static int	_count;
public:
  explicit DEV_D_A(COMMON_COMPONENT* c=nullptr);
  explicit DEV_D_A(const DEV_D_A& p);
	   ~DEV_D_A()			{--_count;}
private: // override virtuals
  char	   id_letter()const override	{return '\0';}
  std::string value_name()const override{return "";}
  bool	   print_type_in_spice()const override{return true;}
  std::string dev_type()const override	{return "d_d_a";}
  int	   tail_size()const override	{return 2;}
  int	   max_nodes()const override	{return 2;}
  int	   min_nodes()const override	{return 2;}
  int	   matrix_nodes()const override	{return 2;}
  int	   net_nodes()const override	{return _net_nodes;}

  CARD*	   clone()const override	{return new DEV_D_A(*this);}
  void	   precalc_first()override	{ELEMENT::precalc_first();}
  void	   expand()override;
  void	   precalc_last() override	{ELEMENT::precalc_last();}
  void	   tr_iwant_matrix()override	{untested();}
  void	   tr_begin()override		{ELEMENT::tr_begin();}
  void	   tr_restore()override		{ELEMENT::tr_restore();}
  void	   dc_advance()override;
  void	   tr_advance()override;
  void	   tr_regress()override;
  bool	   tr_needs_eval()const override;
  void	   tr_queue_eval()override	{ELEMENT::tr_queue_eval();}
  bool	   do_tr()override;
  void	   tr_load()override;
  void	   tr_unload()override;
  TIME_PAIR tr_review()override;
  //void   tr_accept()override;
  double   tr_involts()const override	{ untested();unreachable(); return 0;}
  //double tr_input()const		//ELEMENT
  double   tr_involts_limited()const override { untested();unreachable(); return 0;}
  //double tr_input_limited()const	//ELEMENT
  //double tr_amps()const		//ELEMENT
  double   tr_probe_num(const std::string& what)const override
					{untested(); return n_(OUTNODE)->tr_probe_num(what);}

  void	   ac_iwant_matrix()override	{untested();}
  void	   ac_begin()override
		{untested(); error(bWARNING, long_label() + ": no logic in AC analysis\n");}
  void	   do_ac()override		{untested();}
  void	   ac_load()override		{untested();}
  COMPLEX  ac_involts()const override	{unreachable(); return 0.;}
  COMPLEX  ac_amps()const override	{unreachable(); return 0.;}
  XPROBE   ac_probe_ext(const std::string& what)const override
					{return n_(OUTNODE)->ac_probe_ext(what);}

  std::string port_name(int i)const override {
    assert(i >= 0);
    assert(i < 2);
    static std::string names[] = {"out", "in"};
    return names[i];
  }
public:
  static int count()			{untested();return _count;}
};
/*--------------------------------------------------------------------------*/
DEV_D_A::DEV_D_A(COMMON_COMPONENT* c)
  :ELEMENT(c)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_D_A::DEV_D_A(const DEV_D_A& p)
  :ELEMENT(p)
{
  assert(max_nodes() == 2);
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    assert(_nodes[ii] == p._nodes[ii]);
  }
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_D_A::expand()
{
  ELEMENT::expand();
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);

  attach_model();
  const MODEL_LOGIC* m = dynamic_cast<const MODEL_LOGIC*>(c->model());
  if (!m) {
    throw Exception_Model_Type_Mismatch(long_label(), c->modelname(), "logic family (LOGIC)");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_D_A::dc_advance()
{
  ELEMENT::dc_advance();
  if (n_(INNODE)->in_transit()) {
    //q_eval(); evalq is not used for DC
    n_(INNODE)->propagate();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_D_A::tr_advance()
{
  ELEMENT::tr_advance();

  if (n_(INNODE)->in_transit()) {
    q_eval();
    if (_sim->_time0 >= n_(INNODE)->final_time()) {
      n_(INNODE)->propagate();
    }else{
      // not ready to propagate.
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_D_A::tr_regress()
{
  ELEMENT::tr_regress();

  q_eval();
  if (n_(INNODE)->last_change_time() > _sim->_time0) {
    n_(INNODE)->unpropagate();
    assert(_sim->_time0 < n_(INNODE)->final_time());
  }else if (_sim->_time0 >= n_(INNODE)->final_time()) {untested();
    n_(INNODE)->propagate();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_D_A::tr_needs_eval()const
{
  //assert(!is_q_for_eval());
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  return (_sim->analysis_is_static() || _sim->analysis_is_restore());
}
/*--------------------------------------------------------------------------*/
bool DEV_D_A::do_tr()
{
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
    tr_accept();
  }else{
    assert(_sim->analysis_is_tran_dynamic());
  }
  
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  _y[0].x = 0.;
  _y[0].f1 = n_(INNODE)->to_analog(m);
  _y[0].f0 = 0.;
  _m0.x = 0.;
  _m0.c1 = 1./m->rs;
  _m0.c0 = _y[0].f1 / m->rs;
  set_converged(conv_check());
  store_values();
  q_load();
  
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_D_A::tr_load()
{
  tr_load_diagonal_point(n_(OUTNODE), &_m0.c1, &_m1.c1);
  tr_load_source_point(n_(OUTNODE), &_m0.c0, &_m1.c0);
}
/*--------------------------------------------------------------------------*/
void DEV_D_A::tr_unload()
{
  _m0.c0 = _m0.c1 = 0.;
  _sim->mark_inc_mode_bad();
  tr_load();
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_D_A::tr_review()
{
  // not calling ELEMENT::tr_review();
  q_accept();
  //digital mode queues events explicitly in tr_accept
  _time_by.reset();
  return _time_by;
}
/*--------------------------------------------------------------------------*/
int DEV_D_A::_count = -1;
/*--------------------------------------------------------------------------*/
DEV_D_A d_buf;
DISPATCHER<CARD>::INSTALL dd_buf(&device_dispatcher, "xxda", &d_buf);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
