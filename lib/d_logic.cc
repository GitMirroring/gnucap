/*$Id: d_logic.cc  2016/09/17 $ -*- C++ -*-
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
//testing=script,sparse 2006.07.17
#include "globals.h"
#include "e_subckt.h"
#include "u_xprobe.h"
#include "d_logic.h"
/*--------------------------------------------------------------------------*/
int DEV_LOGIC::_count = -1;
int BUILTIN_LOGIC::_count = -1;
// int MODEL_LOGIC::_count = -1; // there is one in e_node.cc, and the dispatcher
// static LOGIC_NONE Default_LOGIC(CC_STATIC);
/*--------------------------------------------------------------------------*/
static DEV_LOGIC p1;
static DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "U|logic", &p1);
static LOGIC_AND pand;
static DISPATCHER<CARD>::INSTALL dand(&device_dispatcher, "and", &pand);
static LOGIC_NAND pnand;
static DISPATCHER<CARD>::INSTALL dnand(&device_dispatcher, "nand", &pnand);
static LOGIC_OR por;
static DISPATCHER<CARD>::INSTALL dor(&device_dispatcher, "or", &por);
static LOGIC_NOR pnor;
static DISPATCHER<CARD>::INSTALL dnor(&device_dispatcher, "nor", &pnor);
static LOGIC_XOR pxor;
static DISPATCHER<CARD>::INSTALL dxor(&device_dispatcher, "xor", &pxor);
static LOGIC_INV pxnor;
static DISPATCHER<CARD>::INSTALL dxnor(&device_dispatcher, "xnor", &pxnor);
static LOGIC_INV pinv;
static DISPATCHER<CARD>::INSTALL dinv(&device_dispatcher, "inv", &pinv);
/*--------------------------------------------------------------------------*/
static BUILTIN_LOGIC p2(CC_STATIC);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_LOGIC::DEV_LOGIC()
  :ELEMENT(),
   _lastchangenode(0),
   _quality(qGOOD),
   _failuremode("ok"),
   _oldgatemode(moUNKNOWN),
   _gatemode(moUNKNOWN)   
{ itested();
  attach_common(&p2);
  _n = nodes;
}
/*--------------------------------------------------------------------------*/
DEV_LOGIC::DEV_LOGIC(const DEV_LOGIC& p)
  :ELEMENT(p),
   _lastchangenode(0),
   _quality(qGOOD),
   _failuremode("ok"),
   _oldgatemode(moUNKNOWN),
   _gatemode(moUNKNOWN)   
{
  assert(max_nodes() == PORTS_PER_GATE);
  for (int ii = 0;  ii < max_nodes();  ++ii) { itested();
    nodes[ii] = p.nodes[ii];
  }
  _n = nodes;
  trace2("copy logic", common()->modelname(), dynamic_cast<BUILTIN_LOGIC const*>(common()));
  trace1("copy logic", common()->has_model());
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::expand()
{ itested();
  trace4("DEV_LOGIC::expand", long_label(), common()->modelname(), common()->has_model(), common());
  trace3("expanded", long_label(), common()->modelname(), common());

  if(common()->modelname()==""){
    ELEMENT::expand();
    return;
  }else{
    // spice legacy stuff.
    //
    // keep modelname. use COMMON for logic.
    _modelname=common()->modelname();
  }

  // MODEL_LOGIC is actually a COMMON_COMPONENT. need to arrange things
  const MODEL_LOGIC* c = prechecked_cast<const MODEL_LOGIC*>(common());
  assert(c);

  attach_model();

#if 1
  if (!common()->has_model()) { untested();
    throw Exception_Model_Type_Mismatch(long_label(), c->modelname(), "logic family (LOGIC)");
  }else if (!common()->model()->new_common()) {
    throw Exception_Model_Type_Mismatch(long_label(), c->modelname(), "logic family (LOGIC)");
  }else{
  }
#endif
#if 1

  std::string modelname=common()->modelname();
  COMMON_COMPONENT* mc=common()->model()->new_common()->clone(); // zaps modelname .
  trace2("attached new common", long_label(), common()->modelname());
  mc->set_modelname("");
  attach_common(mc);
  precalc_first();
  ELEMENT::expand(); // does the deflate trick.

  std::string subckt_name(_modelname+name()+to_string(incount()));
  try {
    const CARD* model = find_looking_out(subckt_name);
    
    if(!dynamic_cast<const BASE_SUBCKT*>(model)) {untested();
      error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)),
	    long_label() + ": " + subckt_name + " is not a subckt, forcing digital\n");
    }else{
      _gatemode = OPT::mode;    
      renew_subckt(model, NULL/*&(c->_params)*/);    
      subckt()->expand();
    }
  }catch (Exception_Cant_Find&) {
    error(((!_sim->is_first_expand()) ? (bDEBUG) : (bWARNING)), 
	  long_label() + ": can't find subckt: " + subckt_name + ", forcing digital\n");
  }
#endif
  
  assert(!is_constant()); /* is a BUG */
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_iwant_matrix()
{
  if (subckt()) {
    subckt()->tr_iwant_matrix();
  }else{
  }
  tr_iwant_matrix_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_begin()
{
  ELEMENT::tr_begin();
  if (!subckt()) {
    _gatemode = moDIGITAL;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
    subckt()->tr_begin();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_restore()
{untested();
  ELEMENT::tr_restore();
  if (!subckt()) {untested();
    _gatemode = moDIGITAL;
  }else{untested();
    _gatemode = (OPT::mode==moMIXED) ? moANALOG : OPT::mode;
    subckt()->tr_restore();
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::dc_advance()
{
  ELEMENT::dc_advance();

  if (_gatemode != _oldgatemode) {untested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moANALOG:
    assert(subckt());
    subckt()->dc_advance();
    break;
  case moDIGITAL:
    if (_n[OUTNODE]->in_transit()) {
      //q_eval(); evalq is not used for DC
      _n[OUTNODE]->propagate();
    }else{
    }
    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_advance: the first to run on a new time step.
 * It sets up preconditions for the new time.
 */
void DEV_LOGIC::tr_advance()
{
  ELEMENT::tr_advance();

  if (_gatemode != _oldgatemode) {
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moANALOG:
    assert(subckt());
    subckt()->tr_advance();
    break;
  case moDIGITAL: 
    if (_n[OUTNODE]->in_transit()) {
      q_eval();
      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {
	_n[OUTNODE]->propagate();
      }else{
	// not ready to propagate. overclocked?
      }
    }else{
    }
    break;
  }
}
void DEV_LOGIC::tr_regress()
{itested();
  ELEMENT::tr_regress();

  if (_gatemode != _oldgatemode) {itested();
    tr_unload();
    _n[OUTNODE]->set_mode(_gatemode);
    _oldgatemode = _gatemode;
  }else{itested();
  }
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moANALOG:  itested();
    assert(subckt());
    subckt()->tr_regress();
    break;
  case moDIGITAL: itested();
    if (_n[OUTNODE]->in_transit()) {itested();
      q_eval();
      if (_sim->_time0 >= _n[OUTNODE]->final_time()) {itested();
	_n[OUTNODE]->propagate();
      }else{itested();
      }
    }else{itested();
    }
    break;
  }
}
/*--------------------------------------------------------------------------*/
/* tr_needs_eval
 * in digital mode ... DC always returns true, to queue it.
 * tran always returns false, already queued by tr_advance if needed
 */
bool DEV_LOGIC::tr_needs_eval()const
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL:
    //assert(!is_q_for_eval());
    if (_sim->analysis_is_restore()) {untested();
    }else if (_sim->analysis_is_static()) {
    }else{
    }
    return (_sim->analysis_is_static() || _sim->analysis_is_restore());
  case moANALOG:
    untested();
    assert(!is_q_for_eval());
    assert(subckt());
    return subckt()->tr_needs_eval();
  }
  unreachable();
  return false;
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_queue_eval()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:	  unreachable(); break;
  case moDIGITAL: ELEMENT::tr_queue_eval(); break;
  case moANALOG:  assert(subckt()); subckt()->tr_queue_eval(); break;
  }
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::tr_eval_digital()
{
  assert(_gatemode == moDIGITAL);
  if (_sim->analysis_is_restore()) {untested();
  }else if (_sim->analysis_is_static()) {
  }else{
  }
  if (_sim->analysis_is_static() || _sim->analysis_is_restore()) {
    tr_accept();
  }else{
    assert(_sim->analysis_is_tran_dynamic());
  }
  
  const MODEL_LOGIC* c = prechecked_cast<const MODEL_LOGIC*>(common());
  assert(c);
  // const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
  // assert(m);
  _y[0].x = 0.;
  _y[0].f1 = _n[OUTNODE]->to_analog(c);
  _y[0].f0 = 0.;
  _m0.x = 0.;
  _m0.c1 = 1./c->rs;
  _m0.c0 = _y[0].f1 / -c->rs;
  set_converged(conv_check());
  store_values();
  q_load();
  
  return converged();
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::do_tr()
{  
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL: set_converged(tr_eval_digital()); break;
  case moANALOG:  assert(subckt()); set_converged(subckt()->do_tr()); break;
  }
  return converged();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_load()
{
  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL: tr_load_passive(); break;
  case moANALOG:  assert(subckt()); subckt()->tr_load(); break;
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR DEV_LOGIC::tr_review()
{
  // not calling ELEMENT::tr_review();

  q_accept();
  //digital mode queues events explicitly in tr_accept

  switch (_gatemode) {
  case moUNKNOWN: unreachable(); break;
  case moMIXED:   unreachable(); break;
  case moDIGITAL: _time_by.reset(); break;
  case moANALOG:  assert(subckt()); _time_by = subckt()->tr_review(); break;
  }
  return _time_by;
}
/*--------------------------------------------------------------------------*/
/* tr_accept: This runs after everything has passed "review".
 * It sets up and queues transitions, and sometimes determines logic states.
 */
void DEV_LOGIC::tr_accept()
{
  trace2("DEV_LOGIC::tr_accept", long_label(), _n[OUTNODE]->lv() );
  assert(_gatemode == moDIGITAL || _gatemode == moANALOG);
  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(common());
  assert(m);
//  const MODEL_LOGIC* m = prechecked_cast<const MODEL_LOGIC*>(c->model());
//  assert(m);
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Check quality and get node info to local array. */
  /* side effect --- generate digital values for analog nodes */
  assert(PORTS_PER_GATE == max_nodes());
  {
    _n[OUTNODE]->to_logic(m);
    _quality = _n[OUTNODE]->quality();  /* the worst quality on this device */
    _failuremode = _n[OUTNODE]->failure_mode();    /* what is wrong with it? */
    _lastchangenode = OUTNODE;		/* which node changed most recently */
    int lastchangeiter=_n[OUTNODE]->d_iter();/* iteration # when it changed */
    trace0(long_label().c_str());
    trace2(_n[OUTNODE]->failure_mode().c_str(), OUTNODE, _n[OUTNODE]->quality());
    
    for (int ii = BEGIN_IN;  ii < net_nodes();  ++ii) {
      _n[ii]->to_logic(m);
      if (_n[ii]->quality() < _quality) {
	_quality = _n[ii]->quality();
	_failuremode = _n[ii]->failure_mode();
      }else{
      }
      if (_n[ii]->d_iter() >= lastchangeiter) {
	lastchangeiter = _n[ii]->d_iter();
	_lastchangenode = ii;
      }else{
      }
      trace2(_n[ii]->failure_mode().c_str(), ii, _n[ii]->quality());
    }
    /* If _lastchangenode == OUTNODE, no new changes, bypass may be ok.
     * Otherwise, an input changed.  Need to evaluate.
     * If all quality are good, can evaluate as digital.
     * Otherwise need to evaluate as analog.
     */
    trace3(_failuremode.c_str(), _lastchangenode, lastchangeiter, _quality);
  }
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */  
  if (want_analog()) {
    if (_gatemode == moDIGITAL) {untested();
      error(bTRACE, "%s:%u:%g switch to analog, %s\n", long_label().c_str(),
	    _sim->iteration_tag(), _sim->_time0, _failuremode.c_str());
      _oldgatemode = _gatemode;
      _gatemode = moANALOG;
    }else{
    }
    assert(_gatemode == moANALOG);
  }else{
    assert(want_digital());
    if (_gatemode == moANALOG) {
      error(bTRACE, "%s:%u:%g switch to digital\n",
	    long_label().c_str(), _sim->iteration_tag(), _sim->_time0);
      _oldgatemode = _gatemode;
      _gatemode = moDIGITAL;
    }else{
    }
    assert(_gatemode == moDIGITAL);
    if (_sim->analysis_is_restore()) {untested();
    }else if (_sim->analysis_is_static()) {
    }else{
    }
    if (!_sim->_bypass_ok
	|| _lastchangenode != OUTNODE
	|| _sim->analysis_is_static()
	|| _sim->analysis_is_restore()) {
      LOGICVAL future_state = logic_eval(&_n[BEGIN_IN], incount());
      //		      ^^^^^^^^^^
      if ((_n[OUTNODE]->is_unknown()) &&
	  (_sim->analysis_is_static() || _sim->analysis_is_restore())) {
	_n[OUTNODE]->force_initial_value(future_state);
	/* This happens when initial DC is digital.
	 * Answers could be wrong if order in netlist is reversed 
	 */
      }else if (future_state != _n[OUTNODE]->lv()) {
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
	if (_n[OUTNODE]->lv() == lvUNKNOWN
	    || future_state.lv_future() != _n[OUTNODE]->lv_future()) {
	  _n[OUTNODE]->set_event(m->delay, future_state);
	  _sim->new_event(_n[OUTNODE]->final_time());
	  //assert(future_state == _n[OUTNODE].lv_future());
	  if (_lastchangenode == OUTNODE) { untested();
	    unreachable();
	    error(bDANGER, "%s:%u:%g non-event state change\n",
		  long_label().c_str(), _sim->iteration_tag(), _sim->_time0);
	  }else{
	  }
	}else{
	}
      }else{
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::tr_unload()
{
  if (subckt()) {
    subckt()->tr_unload();
  }else{untested();
  }
  tr_unload_passive();
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::ac_iwant_matrix()
{
  if (subckt()) {
    subckt()->ac_iwant_matrix();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void DEV_LOGIC::ac_begin()
{untested();
  if (subckt()) {untested();
    subckt()->ac_begin();
  }else{untested();
    error(bWARNING, long_label() + ": no logic in AC analysis\n");
  }
}
/*--------------------------------------------------------------------------*/
double DEV_LOGIC::tr_probe_num(const std::string& what)const
{
  return _n[OUTNODE]->tr_probe_num(what);
}
/*--------------------------------------------------------------------------*/
XPROBE DEV_LOGIC::ac_probe_ext(const std::string& what)const
{untested();
  return _n[OUTNODE]->ac_probe_ext(what);
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::want_analog()const
{
  return subckt() &&
    ((OPT::mode == moANALOG) || (OPT::mode == moMIXED && _quality != qGOOD));
}
/*--------------------------------------------------------------------------*/
bool DEV_LOGIC::want_digital()const
{
  return !subckt() ||
    ((OPT::mode == moDIGITAL) || (OPT::mode == moMIXED && _quality == qGOOD));
}
/*--------------------------------------------------------------------------*/
MODEL_LOGIC::container_type MODEL_LOGIC::_commons;
/*--------------------------------------------------------------------------*/
bool MODEL_LOGIC::operator==(const COMMON_COMPONENT& x)const
{
  const MODEL_LOGIC* p = dynamic_cast<const MODEL_LOGIC*>(&x);
  trace3("ODEL_LOGIC::operator==", delay.string(), p->delay.string(), delay==p->delay);
  bool rv = p
    && _deflated == p->_deflated
    && delay == p->delay
    && vmax == p->vmax
    && vmin == p->vmin
    && unknown == p->unknown
    && rise == p->rise
    && fall == p->fall
    && rs == p->rs
    && rw == p->rw
    && th1 == p->th1
    && th0 == p->th0
    && mr == p->mr
    && mr == p->mf
    && over == p->over
    && tnom == p->tnom
    && COMMON_COMPONENT::operator==(x);
  if (rv) { untested();
  }else{
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void CMP_lt(double x, double y){
  if(x<y){
    throw true;
  }else if(y<x){
    throw false;
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_LOGIC::operator<(const MODEL_LOGIC& p)const
{ itested();
  trace3("operator<",delay.string(), p.delay.string(), delay<p.delay);
  trace3("operator<",double(delay), double(p.delay), double(delay)<double(p.delay));
  assert(_deflated);
  assert(p._deflated);
  try{
    CMP_lt(delay, p.delay);
    CMP_lt(vmax, p.vmax);
    CMP_lt(vmin, p.vmin);
    CMP_lt(unknown, p.unknown);
    CMP_lt(rise, p.rise);
    CMP_lt(fall, p.fall);
    CMP_lt(rs, p.rs);
    CMP_lt(rw, p.rw);
    CMP_lt(th1, p.th1);
    CMP_lt(th0, p.th0);
    CMP_lt(mr, p.mr);
    CMP_lt(mf, p.mf);
    CMP_lt(over, p.over);
    CMP_lt(tnom, p.tnom);
  }catch(bool b){
    return b;
  }
  return modelname() < p.modelname(); // COMMON_COMPONENT::operator<(x)
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
