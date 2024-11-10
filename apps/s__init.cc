/* $Id: s__init.cc 2016/03/28 al $ -*- C++ -*-
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
 * initialization (allocation, node mapping, etc)
 */
//testing=obsolete
#include "e_cardlist.h"
#include "u_status.h"
#include "u_sim_data.h"
#include "u_prblst.h"
#include "u_nodemap.h"
#include "s__.h"
#include "e_logicnode.h"
/*--------------------------------------------------------------------------*/
SIM::SIM() :
   CMD(),
   _scope(nullptr),
   _time0(0.),
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
   _lu(),
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
   _has_op(s_NONE)
{
  _evalq = &_evalq1;
  _evalq_uc = &_evalq2;
  std::fill_n(_iter, iCOUNT, 0);
  probe_lists();

  _aa_solver = new LU_COPY<double>(_aa);
  _aa.set_solver(_aa_solver);
  _acx_solver = new LU_INPLACE<COMPLEX>(_acx);
  _acx.set_solver(_acx_solver);
}
/*--------------------------------------------------------------------------*/
SIM::SIM(SIM const& s) : CMD(s),
  _probe_lists(s._probe_lists),
  _time0	(s._time0),
  _freq		(s._freq),
  _temp_c	(s._temp_c),
  _damp		(s._damp),
  _dtmin	(s._dtmin),
  _genout	(s._genout),
  _bypass_ok	(s._bypass_ok),
  _fulldamp	(s._fulldamp),
  _last_time	(s._last_time),
  _freezetime	(s._freezetime),
  _user_nodes	(s._user_nodes),
  _subckt_nodes	(s._subckt_nodes),
  _model_nodes	(s._model_nodes),
  _total_nodes	(s._total_nodes),
  _jomega	(s._jomega),
  _limiting	(s._limiting),
  _vmax(0.),
  _vmin(0.),
  _uic(false),
  _inc_mode(tsNO),
  _mode(s_NONE),
  _phase(p_NONE),
  _aa(),
  _lu(),
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
  _has_op(s_NONE)
{
  trace2("SIM::SIM", _time0, _last_time);
  _evalq = &_evalq1;
  _evalq_uc = &_evalq2;

  _aa_solver = new LU_COPY<double>(_aa);
  _aa.set_solver(_aa_solver);
  _acx_solver = new LU_INPLACE<COMPLEX>(_acx);
  _acx.set_solver(_acx_solver);
}
/*--------------------------------------------------------------------------*/
/* assign xvalue: carry over state and possibly internal data
 */
SIM& SIM::operator=(SIM&& ex)
{
  if(this==&ex){ untested();
    incomplete();
    return *this;
  }else{
  }
  assert(_scope);
  assert(ex._scope);
  assert(_scope == ex._scope);
//  _time0       = ex._time0;
//  _freq        = ex._freq;
  _temp_c      = ex._temp_c;
 // _damp        = ex._damp;
//  _dtmin       = ex._dtmin;
 // _genout      = ex._genout;
 // _bypass_ok   = ex._bypass_ok;
  //_fulldamp    = ex._fulldamp;
 // _last_time   = ex._last_time;
  //_freezetime  = ex._freezetime;
 // std::copy_n(ex._iter, iCOUNT, _iter);
  _user_nodes  = ex._user_nodes;
  _subckt_nodes= ex._subckt_nodes;
  _model_nodes = ex._model_nodes;
  _total_nodes = ex._total_nodes;
 // _jomega      = ex._jomega;
 // _limiting    = ex._limiting;
  // _vmax        = ex._vmax;
  // _vmin        = ex._vmin;
//  _uic         = ex._uic;
  //_inc_mode    = ex._inc_mode;
//  _mode        = ex._mode;
 // _phase       = ex._phase;

  _i     = ex._i;
  _v0    = ex._v0;
  _vt1   = ex._vt1;
  _ac    = ex._ac;
  _noise = ex._noise;
  assert(ex._i == nullptr);
  assert(ex._v0 == nullptr);
  assert(ex._vt1 == nullptr);
  assert(ex._ac == nullptr);
  assert(ex._noise == nullptr);


  _nm    = ex._nm;
  ex._nm = nullptr;
  _nstat = ex._nstat;
  ex._nstat = nullptr;
  _vdc   = ex._vdc;
  ex._vdc = nullptr;
  _waves = ex._waves;
  ex._waves = nullptr;

  _aa    = std::move(ex._aa);
  _lu    = std::move(ex._lu);
  _acx   = std::move(ex._acx);

  _eq         = std::move(ex._eq);
  _loadq      = std::move(ex._loadq);
  _acceptq    = std::move(ex._acceptq);
  _evalq1     = std::move(ex._evalq1);
  _evalq2     = std::move(ex._evalq2);
  _late_evalq = std::move(ex._late_evalq);

  assert(ex._evalq);
  assert(ex._evalq_uc);
  if(ex._evalq == &ex._evalq1){
    assert(ex._evalq_uc == &ex._evalq2);
    _evalq = &_evalq1;
    _evalq_uc = &_evalq2;
  }else{
    assert(ex._evalq == &ex._evalq2);
    assert(ex._evalq_uc == &ex._evalq1);
    _evalq = &_evalq2;
    _evalq_uc = &_evalq1;
  }

 // std::fill_n(_iter, iCOUNT, 0);

  // ex._evalq = nullptr;
  // ex._evalq_uc = nullptr;
  //
  trace1("SIM::op=", _last_time);

  return *this;
}
/*--------------------------------------------------------------------------*/
void SIM::command_base(CS& cmd)
{
  assert(_scope);
  if (_scope == &CARD_LIST::card_list) {
  }else{untested();
  }

  reset_timers();
  reset_iteration_counter(_mode);
  reset_iteration_counter(iPRINTSTEP);

  try {
    load(_scope);
    assert(_sim == this); // for now.
    reset_iteration_counter(_mode);
    setup(cmd);
    init(_scope); // wrong.
    assert(CKT_BASE::_sim == this);
    _scope->precalc_last();
    allocate();
    assert(_nstat);

    ::status.set_up.stop();

    switch (ENV::run_mode) {
    case rPRE_MAIN:	unreachable();	break;
    case rBATCH:	sweep(); final(); break;
    case rINTERACTIVE:	itested();sweep(); final(); break;
    case rSCRIPT:	sweep(); final(); break;
    case rPRESET:	/*nothing*/	break;
    }
  }catch (Exception& e) {
    error(bDANGER, e.message() + '\n');
    count_iterations(iTOTAL);
  }
  finish();
  _sim->unalloc_vectors();
  _sim->_aa.unallocate();

  ::status.total.stop();
}
/*--------------------------------------------------------------------------*/
SIM::~SIM()
{

  sim_data_cleanup(); // was ~SIM_DATA
 // delete _probe_lists;
  _probe_lists = nullptr;

  if(CKT_BASE::_sim == this){
    CKT_BASE::_sim = nullptr;
  }else{
  }

}
/*--------------------------------------------------------------------------*/
void SIM::reset_timers()
{
  ::status.advance.reset();
  ::status.queue.reset();
  ::status.evaluate.reset();
  ::status.load.reset();
  ::status.lud.reset();
  ::status.back.reset();
  ::status.review.reset();
  ::status.accept.reset();
  ::status.output.reset();
  ::status.aux1.reset();
  ::status.aux2.reset();
  ::status.aux3.reset();
  ::status.set_up.reset().start();
  ::status.total.reset().start();
}
/*--------------------------------------------------------------------------*/
void SIM::load(CARD_LIST* scope)
{
  _scope = scope;
  assert(_scope);


  if(CKT_BASE::_sim == this){
  }else{
    sim_data_load();
  }
  assert(CKT_BASE::_sim == this);
}
/*--------------------------------------------------------------------------*/
void SIM::init(CARD_LIST* scope)
{
  trace1("SIM::init", short_label());
  _scope = scope;

  sim_data_init();
}
/*--------------------------------------------------------------------------*/
void SIM::uninit()
{
  trace0("SIM::uninit");
  sim_data_uninit();
}
/*--------------------------------------------------------------------------*/
PROBE_LISTS& SIM::probe_lists()
{
  if(!_probe_lists) {
    assert(CKT_BASE::_probe_lists);
    _probe_lists = CKT_BASE::_probe_lists->new_probelists();
  }else{
  }
  return *_probe_lists;
}
/*--------------------------------------------------------------------------*/
const PROBE_LISTS& SIM::probe_lists() const
{
  return const_cast<SIM*>(this)->probe_lists();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
