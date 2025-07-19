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
//testing=script,sparse 2023.11.22
#include "e_logicmod.h"
#include "globals.h"
#include "e_subckt.h"
#include "u_xprobe.h"
#include "e_logic.h"
#include "e_elemnt.h"
#include "u_nodemap.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DEV_D_A : public ELEMENT {
public:
  enum {OUTNODE=0,BEGIN_IN=1}; //node labels
private:
  static int	_count;
  mutable node_t _nodes[2];
public:
  explicit	DEV_D_A(COMMON_COMPONENT* c=nullptr);
  explicit	DEV_D_A(const DEV_D_A& p);
		~DEV_D_A()		{--_count;}
private: // override virtuals
  char	   id_letter()const override	{return '\0';}
  std::string value_name()const override{return "";}
  bool	      print_type_in_spice()const override{return true;}
  std::string dev_type()const override{assert(has_common()); return common()->name();}
  int	   tail_size()const override {return 2;}
  int	   max_nodes()const override {return 2;}
  int	   min_nodes()const override {return 2;}
  int	   matrix_nodes()const override	{untested(); return 1;}
  int	   net_nodes()const override {return _net_nodes;}

  CARD*	   clone()const override	{return new DEV_D_A(*this);}
  //void   precalc_first() override
  void	   expand()override;
  //void   precalc_last() override
  //void   map_nodes();

  void	   tr_iwant_matrix()override {}
  //void   tr_begin()override;
  //void   tr_restore()override;
  void	   dc_advance()override;
  void	   tr_advance()override;
  void	   tr_regress()override;
  bool	   tr_needs_eval()const override;
  //void   tr_queue_eval()override;
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
  double   tr_probe_num(const std::string&)const override;

  void	   ac_iwant_matrix()override {}
  void	   ac_begin()override {}
  void	   do_ac()override	{untested();}
  void	   ac_load()override	{untested();}
  COMPLEX  ac_involts()const override	{ untested();unreachable(); return 0.;}
  COMPLEX  ac_amps()const override	{ untested();unreachable(); return 0.;}
  XPROBE   ac_probe_ext(const std::string&)const override;

  node_t& n_(int i)const override {
    assert(_nodes); assert(i>=0); assert(i<max_nodes()); return _nodes[i];
  }
  std::string port_name(int i)const override {
    assert(i >= 0);
    assert(i < max_nodes());
    const COMMON_LOGIC* c = dynamic_cast<const COMMON_LOGIC*>(common());
    assert(c);
    return c->port_name(i);
    //static std::string names[max_nodes()] = {"out",
    //    "in1", "in2", "in3", "in4", "in5", "in6", "in7", "in8", "in9"};
    //return names[i];
  }
public:
  static int count()			{untested();return _count;}
private:
  bool	   want_analog()const {return true;}
  bool	   want_digital()const {return false;}
};
/*--------------------------------------------------------------------------*/
class LOGIC_BUF : public COMMON_LOGIC {
private:
  explicit LOGIC_BUF(const LOGIC_BUF& p) :COMMON_LOGIC(p){++_count;}
  COMMON_COMPONENT* clone()const override {return new LOGIC_BUF(*this);}
public:
  explicit LOGIC_BUF(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t* n, int)const override {
    return ~n[0]->lv();
  }
  std::string name()const override	  {return "xxbuf";}
};
/*--------------------------------------------------------------------------*/
class LOGIC_NONE : public COMMON_LOGIC {
private:
  explicit LOGIC_NONE(const LOGIC_NONE&p):COMMON_LOGIC(p){untested();++_count;}
  COMMON_COMPONENT* clone()const override {untested(); return new LOGIC_NONE(*this);}
public:
  explicit LOGIC_NONE(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t*, int)const override {untested();
    return lvUNKNOWN;
  }
  std::string name()const override	  {untested();return "xxerror";}
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
  for (int ii = 0;  ii < max_nodes();  ++ii) {
    _nodes[ii] = p._nodes[ii];
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
/*--------------------------------------------------------------------------*/
void DEV_D_A::dc_advance()
{
  ELEMENT::dc_advance();

  if (n_(OUTNODE)->in_transit()) {
    //q_eval(); evalq is not used for DC
    n_(OUTNODE)->propagate();
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

  if (n_(OUTNODE)->in_transit()) {
    q_eval();
    if (_sim->_time0 >= n_(OUTNODE)->final_time()) {
      n_(OUTNODE)->propagate();
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
  if (n_(OUTNODE)->last_change_time() > _sim->_time0) {
    n_(OUTNODE)->unpropagate();
    assert(_sim->_time0 < n_(OUTNODE)->final_time());
  }else if (_sim->_time0 >= n_(OUTNODE)->final_time()) {untested();
    n_(OUTNODE)->propagate();
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
  _y[0].f1 = n_(OUTNODE)->to_analog(m);
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
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_D_A::tr_accept()
{
  const COMMON_LOGIC* c = prechecked_cast<const COMMON_LOGIC*>(common());
  assert(c);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
}
/*--------------------------------------------------------------------------*/
double DEV_D_A::tr_probe_num(const std::string& what)const
{
  return n_(OUTNODE)->tr_probe_num(what);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_D_A::ac_probe_ext(const std::string& what)const
{untested();
  return n_(OUTNODE)->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
int DEV_D_A::_count = -1;
/*--------------------------------------------------------------------------*/
static LOGIC_NONE Default_LOGIC(CC_STATIC);
static DEV_D_A p1(&Default_LOGIC);
static DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "xxlogic", &p1);

static LOGIC_BUF  c_buf(CC_STATIC);
DISPATCHER<COMMON_COMPONENT>::INSTALL dc_buf(&bm_dispatcher, "xxbuf", &c_buf);
static DEV_D_A d_buf(&c_buf);
static DISPATCHER<CARD>::INSTALL dd_buf(&device_dispatcher, "xxbuf", &d_buf);

static MODEL_LOGIC p2(&p1);
static DISPATCHER<MODEL_CARD>::INSTALL d2(&model_dispatcher, "xxlogic", &p2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
