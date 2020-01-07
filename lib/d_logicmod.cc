/*$Id: d_logicmod.cc,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
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
 * device:  mxxxx vdd out in1 in2 ... family gatetype
 * model:   .model mname LOGIC <args>
 */
//testing=script 2006.07.17
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
MODEL_LOGIC::MODEL_LOGIC(const DEV_LOGIC* p)
  :MODEL_CARD((ELEMENT const*)p),
   delay  (1e-9),
   vmax   (5.),
   vmin	  (0.),
   unknown((vmax+vmin)/2),
   rise   (delay / 2),
   fall   (delay / 2),
   rs     (100.),
   rw     (1e9),
   th1    (.75),
   th0    (.25),
   mr     (5.),
   mf     (5.),
   over   (.1),
   range  (vmax - vmin)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
MODEL_LOGIC::MODEL_LOGIC(const MODEL_LOGIC& p)
  :MODEL_CARD(p),
   delay  (p.delay),
   vmax   (p.vmax),
   vmin	  (p.vmin),
   unknown(p.unknown),
   rise   (p.rise),
   fall   (p.fall),
   rs     (p.rs),
   rw     (p.rw),
   th1    (p.th1),
   th0    (p.th0),
   mr     (p.mr),
   mf     (p.mf),
   over   (p.over),
   range  (p.range)
{
  ++_count;
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::precalc_first()
{
  MODEL_CARD::precalc_first();

  const CARD_LIST* par_scope = scope();
  assert(par_scope);

  delay.e_val(1e-9, par_scope);
  vmax.e_val(5., par_scope);
  vmin.e_val(0., par_scope);
  unknown.e_val((vmax+vmin)/2, par_scope);
  rise.e_val(delay / 2, par_scope);
  fall.e_val(delay / 2, par_scope);
  rs.e_val(100., par_scope);
  rw.e_val(1e9, par_scope);
  th1.e_val(.75, par_scope);
  th0.e_val(.25, par_scope);
  mr.e_val(5., par_scope);
  mf.e_val(5., par_scope);
  over.e_val(.1, par_scope);

  range = vmax - vmin;
}
/*--------------------------------------------------------------------------*/
void MODEL_LOGIC::set_param_by_index(int i, std::string& value, int offset)
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: delay = value; break;
  case 1: vmax = value; break;
  case 2: vmin = value; break;
  case 3: unknown = value; break;
  case 4: rise = value; break;
  case 5: fall = value; break;
  case 6: rs = value; break;
  case 7: rw = value; break;
  case 8: th1 = value; break;
  case 9: th0 = value; break;
  case 10: mr = value; break;
  case 11: mf = value; break;
  case 12: over = value; break;
  default: MODEL_CARD::set_param_by_index(i, value, offset); break;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_LOGIC::param_is_printable(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: 
  case 1: 
  case 2: 
  case 3: 
  case 4: 
  case 5: 
  case 6: 
  case 7: 
  case 8: 
  case 9: 
  case 10:
  case 11:
  case 12: return true;
  default: return MODEL_CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC::param_name(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: return "delay";
  case 1: return "vmax";
  case 2: return "vmin";
  case 3: return "unknown";
  case 4: return "rise";
  case 5: return "fall";
  case 6: return "rs";
  case 7: return "rw";
  case 8: return "thh";
  case 9: return "thl";
  case 10: return "mr";
  case 11: return "mf";
  case 12: return "over";
  default: return MODEL_CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC::param_name(int i, int j)const
{
  if (j == 0) {
    return param_name(i);
  }else if (i >= MODEL_CARD::param_count()) {
    return "";
  }else{
    return MODEL_CARD::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_LOGIC::param_value(int i)const
{
  switch (MODEL_LOGIC::param_count() - 1 - i) {
  case 0: return delay.string();
  case 1: return vmax.string();
  case 2: return vmin.string();
  case 3: return unknown.string();
  case 4: return rise.string();
  case 5: return fall.string();
  case 6: return rs.string();
  case 7: return rw.string();
  case 8: return th1.string();
  case 9: return th0.string();
  case 10: return mr.string();
  case 11: return mf.string();
  case 12: return over.string();
  default: return MODEL_CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// from e_node.cc
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::or_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvRISING,  lvRISING,  lvRISING,  lvSTABLE1, lvRISING},
  {lvFALLING, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvSTABLE1, lvSTABLE1, lvSTABLE1, lvSTABLE1, lvSTABLE1},
  {lvUNKNOWN, lvRISING,  lvUNKNOWN, lvSTABLE1, lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::xor_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvRISING,  lvFALLING, lvRISING,  lvFALLING, lvUNKNOWN},
  {lvFALLING, lvRISING,  lvFALLING, lvRISING,  lvUNKNOWN},
  {lvSTABLE1, lvFALLING, lvRISING,  lvSTABLE0, lvUNKNOWN},
  {lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN, lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::and_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvSTABLE0, lvSTABLE0, lvSTABLE0, lvSTABLE0},
  {lvSTABLE0, lvRISING,  lvFALLING, lvRISING,  lvUNKNOWN},
  {lvSTABLE0, lvFALLING, lvFALLING, lvFALLING, lvFALLING},
  {lvSTABLE0, lvRISING,  lvFALLING, lvSTABLE1, lvUNKNOWN},
  {lvSTABLE0, lvUNKNOWN, lvFALLING, lvUNKNOWN, lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
const _LOGICVAL LOGICVAL::not_truth[lvNUM_STATES] = {
  lvSTABLE1, lvFALLING, lvRISING,  lvSTABLE0, lvUNKNOWN  
};
/*--------------------------------------------------------------------------*/
static _LOGICVAL prop_truth[lvNUM_STATES][lvNUM_STATES] = {
  {lvSTABLE0, lvUNKNOWN, lvUNKNOWN, lvRISING,  lvUNKNOWN},
  {lvFALLING, lvUNKNOWN, lvUNKNOWN, lvRISING,  lvUNKNOWN},
  {lvFALLING, lvUNKNOWN, lvUNKNOWN, lvRISING,  lvUNKNOWN},
  {lvFALLING, lvUNKNOWN, lvUNKNOWN, lvSTABLE1, lvUNKNOWN},
  {lvFALLING, lvUNKNOWN, lvUNKNOWN, lvRISING,  lvUNKNOWN}
};
/*--------------------------------------------------------------------------*/
inline LOGICVAL& LOGICVAL::set_in_transition(LOGICVAL newval)
{
  _lv = prop_truth[_lv][newval];
  assert(_lv != lvUNKNOWN);
  return *this;
}
LOGIC_NODE::LOGIC_NODE()
  :NODE_CARD(),
   _family(0),
   _d_iter(-1), // initially d_iter is older than a_iter
   _a_iter(0),
   _final_time(0),
   _lastchange(0),
   _old_lastchange(0),
   _mode(moANALOG),
   _lv(),
   _old_lv(),
   _quality(qBAD),
   _failure_mode("initial")
{
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::tr_probe_num(const std::string& x)const
{ untested();
  if (Umatch(x, "l{ogic} ")) {
    return annotated_logic_value();
  }else if (Umatch(x, "la{stchange} ")) {untested();
    return _lastchange;
  }else if (Umatch(x, "fi{naltime} ")) {untested();
    return final_time();
  }else if (Umatch(x, "di{ter} ")) {untested();
    return static_cast<double>(_d_iter);
  }else if (Umatch(x, "ai{ter} ")) {untested();
    return static_cast<double>(_a_iter);
  }else{ untested();
    trace2("LN::tr_probe_num", matrix_number(), flat_number());
    return NODE_CARD::tr_probe_num(x);
  }
}
/*--------------------------------------------------------------------------*/
/* annotated_logic_value:  a printable value for probe
 * that has secondary info encoded in its fraction part
 */
double LOGIC_NODE::annotated_logic_value()const
{
  return (_lv + (.1 * (OPT::transits - quality())) + (.01 * (2 - _mode)));
}
/*--------------------------------------------------------------------------*/
static bool newly_stable[lvUNKNOWN+1][lvUNKNOWN+1] = { // oldlv, _lv
  /*	   s0	  rise   fall	s1     u */
  /* s0 */{false, false, false, true,  false},
  /*rise*/{false, false, false, true,  false},
  /*fall*/{true,  false, false, false, false},
  /* s1 */{true,  false, false, false, false},
  /* u  */{true,  false, false, true,  false}
};
/*--------------------------------------------------------------------------*/
inline bool LOGIC_NODE::just_reached_stable()const
{
  return newly_stable[old_lv()][lv()];
}
/*--------------------------------------------------------------------------*/
/* to_logic: set up logic data for a node, if needed
 * If the logic data is already up to date, do nothing.
 * else set up: logic value (_lv) and quality.
 * Use and update _d_iter, _lastchange to keep track of what was done.
 */
void LOGIC_NODE::to_logic(const MODEL_LOGIC*f)
{
  if (is_analog()){
    set_a_iter();
  }else{
  }

  assert(f);
  if (process() && process() != f) {untested();
    set_bad_quality("logic process mismatch");
    error(bWARNING, "node " + long_label() 
	  + " logic process mismatch\nis it " + process()->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  set_process(f);

  if (is_analog() &&  d_iter() < a_iter()) {
    if (_sim->analysis_is_restore()) {untested();
    }else if (_sim->analysis_is_static()) {
    }else{
    }
    if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
      set_last_change_time(0);
      store_old_last_change_time();
      set_lv(lvUNKNOWN);
    }else{
    }
    double dt = _sim->_time0 - last_change_time();
    if (dt < 0.) {untested();
      error(bPICKY, "time moving backwards.  was %g, now %g\n",
	    last_change_time(), _sim->_time0);
      dt = _sim->_time0 - old_last_change_time();
      if (dt <= 0.) {untested();
	throw Exception("internal error: time moving backwards, can't recover");
      }else{untested();
      }
      assert(dt > 0.);
      restore_lv();			/* skip back one */
    }else{
      store_old_last_change_time();
      store_old_lv();			/* save to see if it changes */
    }
    
    double sv = v0() / process()->range;	/* new scaled voltage */
    if (sv >= process()->th1) {		/* logic 1 */
      switch (lv()) {
      case lvSTABLE0: dont_set_quality("stable 0 to stable 1");	break;
      case lvRISING:  dont_set_quality("begin stable 1");	break;
      case lvFALLING:untested();set_bad_quality("falling to stable 1"); break;
      case lvSTABLE1: dont_set_quality("continuing stable 1");	break;
      case lvUNKNOWN: set_good_quality("initial 1");		break;
      }
      set_lv(lvSTABLE1);
    }else if (sv <= process()->th0) {	/* logic 0 */
      switch (lv()) {
      case lvSTABLE0: dont_set_quality("continuing stable 0");	break;
      case lvRISING: untested();set_bad_quality("rising to stable 0");	break;
      case lvFALLING: dont_set_quality("begin stable 0");	break;
      case lvSTABLE1: dont_set_quality("stable 1 to stable 0");	break;
      case lvUNKNOWN: set_good_quality("initial 0");		break;
      }
      set_lv(lvSTABLE0);
    }else{				/* transition region */
      double oldsv = vt1() / process()->range;/* old scaled voltage */
      double diff  = sv - oldsv;
      if (diff > 0) {	/* rising */
	switch (lv()) {
	case lvSTABLE0:
	  dont_set_quality("begin good rise");
	  break;
	case lvRISING:
	  if (diff < dt/(process()->mr * process()->rise)) {
	    set_bad_quality("slow rise");
	  }else{
	    dont_set_quality("continuing good rise");
	  }
	  break;
	case lvFALLING:
	  untested();
	  set_bad_quality("positive glitch in fall");
	  break;
	case lvSTABLE1:
	  untested();
	  set_bad_quality("negative glitch in 1");
	  break;
	case lvUNKNOWN:
	  set_bad_quality("initial rise");
	  break;
	}
	set_lv(lvRISING);
      }else if (diff < 0) {	/* falling */
	switch (lv()) {
	case lvSTABLE0:
	  untested();
	  set_bad_quality("positive glitch in 0");
	  break;
	case lvRISING:
	  set_bad_quality("negative glitch in rise");
	  break;
	case lvFALLING:
	  if (-diff < dt/(process()->mf * process()->fall)) {
	    set_bad_quality("slow fall");
	  }else{
	    dont_set_quality("continuing good fall");
	  }
	  break;
	case lvSTABLE1:
	  dont_set_quality("begin good fall");
	  break;
	case lvUNKNOWN:
	  untested();
	  set_bad_quality("initial fall");
	  break;
	}
	set_lv(lvFALLING);
      }else{				/* hanging up in transition */
	untested();
	error(bDANGER, "inflection???\n");
	set_bad_quality("in transition but no change");
	/* state (rise/fall)  unchanged */
      }
    }
    if (sv > 1.+process()->over || sv < -process()->over) {/* out of range */
      set_bad_quality("out of range");
    }
    if (just_reached_stable()) { /* A bad node gets a little better */
      improve_quality();	/* on every good transition.	   */
    }				/* Eventually, it is good enough.  */
				/* A good transition is defined as */
				/* entering a stable state from    */
				/* a transition state.		   */
    set_d_iter();
    set_last_change_time();
    trace3(_failure_mode.c_str(), _lastchange, _quality, _lv);
  }
}
/*--------------------------------------------------------------------------*/
double LOGIC_NODE::to_analog(const MODEL_LOGIC* f)
{
  assert(f);
  if (process() && process() != f) {untested();
    error(bWARNING, "node " + long_label() 
	  + " logic process mismatch\nis it " + process()->long_label() 
	  + " or " + f->long_label() + "?\n");
  }
  set_process(f);

  double start = NOT_VALID;
  double end = NOT_VALID;
  double risefall = NOT_VALID;
  switch (lv()) {
  case lvSTABLE0:
    return process()->vmin;
  case lvRISING:
    start = process()->vmin;
    end = process()->vmax;
    risefall = process()->rise;
    break;
  case lvFALLING:
    start = process()->vmax;
    end = process()->vmin;
    risefall = process()->fall;
    break;
  case lvSTABLE1:
    return process()->vmax;
  case lvUNKNOWN:
    return process()->unknown;
  }
  assert(start != NOT_VALID);
  assert(end   != NOT_VALID);
  assert(risefall != NOT_VALID);

  if (_sim->_time0 <= (final_time()-risefall)) {
    return start;
  }else if (_sim->_time0 >= final_time()) {
    untested();
    return end;
  }else{
    return end - ((end-start) * (final_time()-_sim->_time0) / risefall);
  }
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::propagate()
{
  assert(in_transit());
  if (lv().is_rising()) {
    set_lv(lvSTABLE1);
  }else if (lv().is_falling()) {
    set_lv(lvSTABLE0);
  }else{
    // lv no change
  }
  set_d_iter();
  set_final_time(NEVER);
  set_last_change_time();
  assert(!(in_transit()));
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::force_initial_value(LOGICVAL v)
{
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{untested();
  }
  assert(_sim->analysis_is_static() || _sim->analysis_is_restore());
  assert(_sim->_time0 == 0.);
  assert(is_unknown());
  assert(is_digital());
  set_lv(v); // BUG ??
  set_good_quality("initial dc");
  set_d_iter();
  set_final_time(NEVER);
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::set_event(double delay, LOGICVAL v)
{
  _lv.set_in_transition(v);
  if (_sim->analysis_is_tran_dynamic()  &&  in_transit()) {
    set_bad_quality("race");
  }else{
    // normal good quality event
    // leaving quality as it was
  }
  set_d_iter();
  set_final_time(_sim->_time0 + delay);
  if (OPT::picky <= bTRACE) {untested();
    error(bTRACE, "%s:%u:%g new event\n",
	  long_label().c_str(), d_iter(), final_time());
  }
  set_last_change_time();
}
/*--------------------------------------------------------------------------*/
void LOGIC_NODE::map_nodes()
{ untested();
  trace3("map ln", long_label(), _n[0].m_(), _n[0].t_());
  _n[0].map();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
