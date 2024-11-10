/*$Id: u_sim_data.cc 2016/03/23 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
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
 * aux functions associated with the SIM class
 */
#include "s__.h"
#include "m_wave.h"
#include "e_logicnode.h"
#include "u_nodemap.h"
#include "e_cardlist.h"
#include "u_status.h"
/*--------------------------------------------------------------------------*/
<<<<<<< HEAD
SIM_DATA::SIM_DATA()
  :_time0(0.),
   _freq(0.),
   _temp_c(0.),
   _damp(0.),
   _dtmin(0.),
   _genout(0.),
   _bypass_ok(true),
   _fulldamp(false),
   _last_time(0.),
   _freezetime(false),
   _user_nodes(0),
   _subckt_nodes(0),
   _model_nodes(0),
   _total_nodes(0),
   _jomega(0.,0.),
   _limiting(true),
   _vmax(0.),
   _vmin(0.),
   _uic(false),
   _inc_mode(tsNO),
   _mode(s_NONE),
   _phase(p_NONE),
   _nm(nullptr),
   _i(nullptr),
   _v0(nullptr),
   _vt1(nullptr),
   _ac(nullptr),
   _noise(nullptr),
   _nstat(nullptr),
   _vdc(nullptr),
   _aa(),
   _lu(_aa), // alias.
   _acx(),
   _eq(),
   _loadq(),
   _acceptq(),
   _evalq1(),
   _evalq2(),
   _late_evalq(),
   _evalq(nullptr),
   _evalq_uc(nullptr),
   _waves(nullptr),
   _has_op(s_NONE),
   _aa_solver(nullptr),
   _acx_solver(nullptr)
{
  _evalq = &_evalq1;
  _evalq_uc = &_evalq2;
  std::fill_n(_iter, iCOUNT, 0);

=======
/*--------------------------------------------------------------------------*/
bool SIM::is_first_expand()
{
  return !_nstat;
>>>>>>> 01b5ca126 (rearrange SIM_DATA into SIM)
}
/*--------------------------------------------------------------------------*/
void SIM::sim_data_cleanup()
{
// was SIM_DATA::
  if (_nm) {
    delete [] _nm;
    _nm = nullptr;
  }else{
  }
  if (_i) {
    delete [] _i;
    _i = nullptr;
  }else{
  }
  if (_v0) {
    delete [] _v0;
    _v0 = nullptr;
  }else{
  }
  if (_vt1) {
    delete [] _vt1;
    _vt1 = nullptr;
  }else{
  }
  if (_ac) {
    delete [] _ac;
    _ac = nullptr;
  }else{
  }
  if (_noise) {
    delete [] _noise;
    _noise = nullptr;
  }else{
  }
  if (_nstat) {
    delete [] _nstat;
    _nstat = nullptr;
  }else{
  }
  if (_vdc) {
    delete [] _vdc;
    _vdc = nullptr;
  }else{
  }
  //assert(_eq.empty()); //not empty means an analysis ended with an unhandled event
			 // could be DC, could be tran with event time past the end
  assert(_loadq.empty());
  assert(_acceptq.empty());
  assert(_evalq1.empty());
  assert(_evalq2.empty());
  assert(_late_evalq.empty());
  assert(_evalq);
  assert(_evalq_uc);
  _evalq = nullptr;
  _evalq_uc = nullptr;

  if (_waves) {
    delete [] _waves;
    _waves = nullptr;
  }else{
  }

  _aa.set_solver(nullptr);
  delete _aa_solver;
  _acx.set_solver(nullptr);
  delete _acx_solver;
}
/*--------------------------------------------------------------------------*/
// was SIM_DATA::
void SIM::set_limit()
{
  for (int ii = 1;  ii <= _total_nodes;  ++ii) {
    set_limit(_v0[ii]);
  }
}
/*--------------------------------------------------------------------------*/
void SIM::set_limit(double v)
{
  if (v+.4 > _vmax) {
    _vmax = v+.5;
    error(bTRACE, "new max = %g, new limit = %g\n", v, _vmax);
  }
  if (v-.4 < _vmin) {
    _vmin = v-.5;
    error(bTRACE, "new min = %g, new limit = %g\n", v, _vmin);
  }
}
/*--------------------------------------------------------------------------*/
void SIM::clear_limit()
{
  _vmax = OPT::vmax;
  _vmin = OPT::vmin;
}
/*--------------------------------------------------------------------------*/
void SIM::keep_voltages()
{
  if (!_freezetime) {
    for (int ii = 1;  ii <= _total_nodes;  ++ii) {
      _vdc[ii] = _v0[ii];
    }
    _last_time = (_time0 > 0.) ? _time0 : 0.;
  }else{untested();
    //BUG// probably incorrect
  }
}
/*--------------------------------------------------------------------------*/
void SIM::restore_voltages()
{
  for (int ii = 1;  ii <= _total_nodes;  ++ii) {
    _vt1[ii] = _v0[ii] = _vdc[ii];
  }
}
/*--------------------------------------------------------------------------*/
// was SIM_DATA::
void SIM::zero_voltages()
{
  for (int ii = 1;  ii <= _total_nodes;  ++ii) {
    _vt1[ii] = _v0[ii] = _vdc[ii] = _i[ii] = 0.;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* map__nodes: map intermediate node number to internal node number.
 * Ideally, this function would find some near-optimal order
 * and squash out gaps.
 */
void SIM::map__nodes()
{
  _nm = new int[_total_nodes+1];
  ::status.order.reset().start();
  switch (OPT::order) {
  default:       unreachable();
    error(bWARNING, "invalid order spec: %d\n", OPT::order);
    // fall through
  case oAUTO:    order_auto();    break;
  case oREVERSE: order_reverse(); break;
  case oFORWARD: order_forward(); break;
  }
  ::status.order.stop();
}
/*--------------------------------------------------------------------------*/
/* order_reverse: force ordering to reverse of user ordering
 *  subcircuits at beginning, results on border at the bottom
 */
void SIM::order_reverse()
{
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {
    _nm[node] = _total_nodes - node + 1;
  }
}
/*--------------------------------------------------------------------------*/
/* order_forward: use user ordering, with subcircuits added to end
 * results in border at the top (worst possible if lots of subcircuits)
 */
void SIM::order_forward()
{
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {
    _nm[node] = node;
  }
}
/*--------------------------------------------------------------------------*/
/* order_auto: full automatic ordering
 * reverse, for now
 */
void SIM::order_auto()
{
  _nm[0] = 0;
  for (int node = 1;  node <= _total_nodes;  ++node) {
    _nm[node] = _total_nodes - node + 1;
  }
}
/*--------------------------------------------------------------------------*/
/* init: allocate, set up, etc ... for any type of simulation
 * also called by status and probe for access to internals and subckts
 */
void SIM::sim_data_init()
{
  assert(_scope);
  CARD_LIST* scope = _scope;
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
    incomplete();
  }

  SIM* previous = /* BUG: scope */ CKT_BASE::_sim;

  if (!previous) { untested();
    // nothing there, nothing to do.
  }else if (previous != this) { untested();
    // import "is_first_expand" from previous.
    // i.e. re-use data, if applicable.
    assert(previous->_scope == _scope);
    trace1("init: prev move", previous->is_first_expand());
    bool tmp = previous->is_first_expand();
    *this = std::move(*previous);
    assert(tmp==is_first_expand());
  }else{
  }

  CKT_BASE::_sim = this;

  trace1("SIM::sim_data_init..", is_first_expand());
  if (is_first_expand()) {
    sim_data_uninit();
    assert(CKT_BASE::_sim);
    init_node_count(scope->nodes()->how_many(), 0, 0);
    scope->expand();
    map__nodes();
    scope->map_nodes();
    alloc_hold_vectors();
    _aa.reinit(_total_nodes);
    _acx.reinit(_total_nodes);
    scope->tr_iwant_matrix(); // TODO: tr_set
    scope->ac_iwant_matrix();
    _last_time = 0;
  }else{
    scope->precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* load: load sim data from sequence
 * .tran (*)
 * .op   (**)
 * .tran // run (*) again, but circuit state set in (**). go get it.
 */
void SIM::sim_data_load()
{
  SIM* prev = /* BUG: scope */ CKT_BASE::_sim;

  if (prev == this) {
    unreachable();
    // no need.
  }else if (prev) {
   // _time0       = prev->_time0;
/// _freq        = prev->_freq;
/// _temp_c      = prev->_temp_c;
/// _damp        = prev->_damp;
/// _dtmin       = prev->_dtmin;
/// _genout      = prev->_genout;
    _bypass_ok   = prev->_bypass_ok;
    _fulldamp    = prev->_fulldamp;
    _last_time   = prev->_last_time;
    _freezetime  = prev->_freezetime;
    std::copy_n(   prev->_iter, iCOUNT, _iter);
   // _user_nodes  = prev->_user_nodes;
   // _subckt_nodes= prev->_subckt_nodes;
   // _model_nodes = prev->_model_nodes;
   // _total_nodes = prev->_total_nodes;
    _jomega      = prev->_jomega;
    _limiting    = prev->_limiting;
    _vmax        = prev->_vmax;
    _vmin        = prev->_vmin;
    //_uic         = prev->_uic;
    _inc_mode    = prev->_inc_mode;
   // _mode        = prev->_mode; overriden before try/catch block
    _phase       = prev->_phase;

    _has_op = prev->_has_op;

  // std::fill_n(_iter, iCOUNT, 0);
  //
    // move "held vectors"
    *this = std::move(*prev);
#if 0
    assert(!_vdc);
    assert(!_nstat);

    _vdc = prev->_vdc;
    _nstat = prev->_nstat;

    prev->_vdc = nullptr;
    prev->_nstat = nullptr;
#endif
  }else{
  }

  CKT_BASE::_sim = this;
}
/*--------------------------------------------------------------------------*/
/* alloc_hold_vectors:
 * allocate space to hold data between commands.
 * for restart, convergence assistance, bias for AC, post-processing, etc.
 * must be done BEFORE deciding what array elements to allocate,
 * but after mapping
 * if they already exist, leave them alone to save data
 */
void SIM::alloc_hold_vectors()
{
  trace1("SIM::alloc_hold_vectors", _total_nodes);
  assert(is_first_expand());

  assert(!_nstat);
  _nstat = new LOGIC_NODE[_total_nodes+1];
  for (int ii=0;  ii <= _total_nodes;  ++ii) {
    _nstat[_nm[ii]].set_user_number(ii);
  }

  assert(!_vdc);
  _vdc = new double[_total_nodes+1];
  std::fill_n(_vdc, _total_nodes+1, 0);

  assert(_nstat);
  assert(_vdc);
}
/*--------------------------------------------------------------------------*/
/* alloc_vectors:
 * these are new with every run and are discarded after the run.
 */
void SIM::alloc_vectors()
{
  assert(_evalq1.empty());
  assert(_evalq2.empty());
  assert(_evalq != _evalq_uc);

  assert(!_ac);
  assert(!_i);
  assert(!_v0);
  assert(!_vt1);

  _ac = new COMPLEX[_total_nodes+1];
  _noise = new COMPLEX[_total_nodes+1];
  _i   = new double[_total_nodes+1];
  _v0  = new double[_total_nodes+1];
  _vt1 = new double[_total_nodes+1];
  std::fill_n(_ac, _total_nodes+1, 0);
  std::fill_n(_noise, _total_nodes+1, 0);
  std::fill_n(_i,  _total_nodes+1, 0);
  std::fill_n(_v0, _total_nodes+1, 0);
  std::fill_n(_vt1,_total_nodes+1, 0);
}
/*--------------------------------------------------------------------------*/
// was SIM_DATA
void SIM::unalloc_vectors()
{
  _evalq1.clear();
  _evalq2.clear();
  delete [] _i;
  _i = nullptr;
  delete [] _v0;
  _v0 = nullptr;
  delete [] _vt1;
  _vt1 = nullptr;
  delete [] _ac;
  _ac = nullptr;
  delete [] _noise;
  _noise = nullptr;
}
/*--------------------------------------------------------------------------*/
/* uninit: undo all the allocation associated with any simulation
 * called when the circuit changes after a run, so it needs a restart
 * may be called multiple times without damage to make sure it is clean
 */
void SIM::sim_data_uninit()
{
  if (_vdc) {
    _acx.reinit(0);
    _aa.reinit(0);
    delete [] _vdc;
    _vdc = nullptr;
    delete [] _nstat;
    _nstat = nullptr;
    delete [] _nm;
    _nm = nullptr;
  }else{
    assert(_acx.size() == 0);
    assert(_aa.size() == 0);
    assert(!_nstat);
    assert(!_nm);
  }
  _has_op = s_NONE;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
