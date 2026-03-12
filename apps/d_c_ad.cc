/*$Id: d_logic.cc  $ -*- C++ -*-
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
 * logic model and device.
 * netlist syntax:
 * device:  mxxxx  out gnd vdd in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
//testing=none
#include "globals.h"
#include "e_subckt.h"
#include "e_logicmod.h"
#include "e_logic.h"
#include "u_xprobe.h"
#include "e_elemnt.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_A_D : public ELEMENT {
public:
  enum {UPPER=0, LOWER=1, OUTNODE=UPPER, INNODE=LOWER}; //node labels
private:
  static int	_count;
public:
  explicit DEV_A_D(COMMON_COMPONENT* c=nullptr);
  explicit DEV_A_D(const DEV_A_D& p);
	   ~DEV_A_D()			{--_count;}
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

  CARD*	   clone()const override	{return new DEV_A_D(*this);}
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
  void	   tr_accept()override;
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
DEV_A_D::DEV_A_D(COMMON_COMPONENT* c)
  :ELEMENT(c)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
DEV_A_D::DEV_A_D(const DEV_A_D& p)
  :ELEMENT(p)
{
  assert(max_nodes() == 2);
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    assert(_nodes[ii] == p._nodes[ii]);
  }
  ++_count;
}
/*--------------------------------------------------------------------------*/
void DEV_A_D::expand()
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
void DEV_A_D::dc_advance()
{
  ELEMENT::dc_advance();
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_A_D::tr_advance()
{
  ELEMENT::tr_advance();
}
/*--------------------------------------------------------------------------*/
void DEV_A_D::tr_regress()
{
  ELEMENT::tr_regress();
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_A_D::tr_needs_eval()const
{
  assert(!is_q_for_eval());
  return false;
}
/*--------------------------------------------------------------------------*/
bool DEV_A_D::do_tr()
{  
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_A_D::tr_load()
{
}
/*--------------------------------------------------------------------------*/
void DEV_A_D::tr_unload()
{
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_A_D::tr_review()
{
  // not calling ELEMENT::tr_review();
  q_accept();
  //digital mode queues events explicitly in tr_accept
  _time_by.reset();
  return _time_by;
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_A_D::tr_accept()
{
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);

  n_(OUTNODE)->to_logic(m, n_(INNODE)->v0());
#if 0
  if (!_sim->_bypass_ok
      || _sim->analysis_is_static()
      || _sim->analysis_is_restore()) {
    LOGICVAL future_state = c->logic_eval(&n_(INNODE), net_nodes()-INNODE);
    //		         ^^^^^^^^^^
    if ((n_(OUTNODE)->is_unknown()) &&
	(_sim->analysis_is_static() || _sim->analysis_is_restore())) {
      n_(OUTNODE)->force_initial_value(future_state);
      n_(OUTNODE)->store_old_lv();
      /* This happens when initial DC is digital.
       * Answers could be wrong if order in netlist is reversed 
       */
    }else if (future_state != n_(OUTNODE)->lv()) {
      assert(future_state != lvUNKNOWN);
      switch (future_state) {
      case lvSTABLE0:	/*nothing*/		break;
      case lvRISING:  future_state=lvSTABLE0;	break;
      case lvFALLING: future_state=lvSTABLE1;	break;
      case lvSTABLE1:	/*nothing*/		break;
      case lvUNKNOWN: unreachable();		break;
      }
      /* This handling of rising and falling may seem backwards.
       * These states occur when the value has been contaminated 
       * by another pending action.  The "old" value is the
       * value without this contamination.
       * This code is planned for replacement as part of VHDL/Verilog
       * conversion, so the kluge stays in for now.
       */
      assert(future_state.lv_old() == future_state.lv_future());
      if (n_(OUTNODE)->lv() == lvUNKNOWN
	  || future_state.lv_future() != n_(OUTNODE)->lv_future()) {
	n_(OUTNODE)->set_event(c->_real_delay, future_state);
	//assert(future_state == n_(OUTNODE).lv_future());
      }else{
      }
    }else{
    }
  }else{
  }
  n_(OUTNODE)->store_old_last_change_time();
  n_(OUTNODE)->store_old_lv(); // needed? yes
#endif
}
/*--------------------------------------------------------------------------*/
int DEV_A_D::_count = -1;
/*--------------------------------------------------------------------------*/
DEV_A_D d_buf;
DISPATCHER<CARD>::INSTALL dd_buf(&device_dispatcher, "xxad", &d_buf);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
