/*$Id: c_prbcmd.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
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
 * probe and plot commands
 * set up print and plot (select points, maintain probe lists)
 * command line operations
 */
//testing=script,sparse 2006.07.17
#include "u_sim_data.h"
#include "c_comand.h"
#include "u_prblst.h"
#include "globals.h"
#ifdef TRACE_UNTESTED
#include "e_cardlist.h" // not actually used.
#endif
#include <set> // to keep track (kludge)
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD : public CMD {
protected:
  SIM* _sim{nullptr};
  void set_sim(CS& cmd, CARD_LIST *scope);
  void do_probe(CS& cmd, std::string const& what, CARD_LIST *scope);
};
/*--------------------------------------------------------------------------*/
void OUTPUT_CMD::set_sim(CS& cmd, CARD_LIST *scope)
{

  trace1("set_sim", cmd.tail());
  size_t here = cmd.cursor();
  CMD* c = command_dispatcher[cmd];
  if (SIM* sim = dynamic_cast<SIM*>(c)) {
    trace1("set_sim1", sim->short_label());
    _sim = sim;
    _sim->load(scope);
    assert(CKT_BASE::_sim == _sim);
    trace1("set_sim1b", sim->short_label());
    _sim->init(scope);
  }else{
    trace1("set_sim2", cmd.fullstring());
    // leave CKT_BASE::_sim intact.
    cmd.reset(here);
    _sim = nullptr;
  }
}
/*--------------------------------------------------------------------------*/
void list_all(std::string const& what)
{
  std::set<void*> track;
  for(auto i : command_dispatcher){
    if(auto s = dynamic_cast<SIM*>(i.second)){
      PROBELIST const* probes_simtype = nullptr;
      bool done = !track.insert(s).second;
      if(done){
      }else if(what=="alarm") { untested();
	probes_simtype = &s->alarmlist();
      }else if(what=="print") {
	probes_simtype = &s->printlist();
      }else if(what=="store") { untested();
	probes_simtype = &s->storelist();
      }else if(what=="plot") { untested();
	probes_simtype = &s->plotlist();
      }else{ untested();
	incomplete();
	throw Exception("no such probelist");
      }
      if(probes_simtype){
      probes_simtype->listing(i.second->short_label());
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void clear_all()
{
  for(auto i : command_dispatcher){
    if(auto s = dynamic_cast<SIM*>(i.second)){
      s->probe_lists().alarm.clear();
      s->probe_lists().plot .clear();
      s->probe_lists().print.clear();
      s->probe_lists().store.clear();
    }
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_CMD::do_probe(CS& cmd, std::string const& what, CARD_LIST *scope)
{
  assert(scope);
  if (scope == &CARD_LIST::card_list) {
  }else{itested();
  }

  enum {aADD, aDELETE, aNEW} action;
  SIM_MODE simtype = s_NONE;

  if (cmd.match1('-')) {untested();	/* handle .probe - ac ...... */
    action = aDELETE;		/* etc. 		     */
    cmd.skip();
  }else if (cmd.match1('+')) {untested();
    action = aADD;
    cmd.skip();
  }else{			/* no -/+ means clear, but wait for */
    action = aNEW;		/* .probe ac + ..... 		    */
  }				/* which will not clear first	    */

  PROBELIST* probes_simtype = nullptr;
  assert(scope);
  set_sim(cmd, scope);
  if(!_sim){
    trace1("no sim name arg", cmd.fullstring());
  }else if(what=="alarm") {
    probes_simtype = &_sim->probe_lists().alarm;
  }else if(what=="print") {
    probes_simtype = &_sim->probe_lists().print;
  }else if(what=="plot") {
    probes_simtype = &_sim->probe_lists().plot;
  }else if(what=="store") {
    probes_simtype = &_sim->probe_lists().store;
  }else{ untested();
    incomplete();
    throw Exception("no such probelist");
  }

  trace1("do_probe", simtype);
  
  if (!probes_simtype) {			/* must be all simtypes */
    if (cmd.is_end()) {		/* list all */
      list_all(what);
    }else if (cmd.umatch("clear ")) {		/* clear all */
      clear_all();
    }else{itested();				/* error */
      throw Exception_CS("what's this?", cmd);
    }
  }else{
    if (cmd.is_end()) {untested();		/* list */
      probes_simtype->listing(_sim->short_label());
    }else if (cmd.umatch("clear ")) {itested();/* clear */
      probes_simtype->clear();
    }else{				/* add/remove */
      if (cmd.match1('-')) {itested();		/* setup cases like: */
	action = aDELETE;			/* .probe ac + ....  */
	cmd.skip();
      }else if (cmd.match1('+')) {
	action = aADD;
	cmd.skip();
      }else{
      }
      if (action == aNEW) {			/* no +/- here or at beg. */
	probes_simtype->clear();		/* means clear first	  */
	action = aADD;
      }else{
      }
      while (cmd.more()) {			/* do-it */
	if (cmd.match1('-')) {			/* handle cases like:	    */
	  action = aDELETE;			/* .pr ac +v(7) -e(6) +r(8) */
	  cmd.skip();
	}else if (cmd.match1('+')) {itested();
	  action = aADD;
	  cmd.skip();
	}else{
	}
	if (action == aDELETE) {
	  probes_simtype->remove_list(cmd);
	}else{
	  probes_simtype->add_list(cmd, scope);
	}
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
class CMD_STORE : public OUTPUT_CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if (Scope == &CARD_LIST::card_list) {
    }else{untested();
    }
#endif
    assert(_probe_lists);
    do_probe(cmd, "store", Scope);
  }
} p0;
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "store|`store", &p0);
/*--------------------------------------------------------------------------*/
class CMD_ALARM : public OUTPUT_CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if (Scope == &CARD_LIST::card_list) {
    }else{untested();
    }
#endif
    assert(_probe_lists);
    do_probe(cmd, "alarm", Scope);
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "alarm|`alarm", &p1);
/*--------------------------------------------------------------------------*/
class CMD_PLOT : public OUTPUT_CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if (Scope == &CARD_LIST::card_list) {
    }else{untested();
    }
#endif
    IO::plotset = true;
    assert(_probe_lists);
    do_probe(cmd, "plot", Scope);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "iplot|plot|`iplot|`plot", &p2);
/*--------------------------------------------------------------------------*/
class CMD_PRINT : public OUTPUT_CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    assert(Scope);
#ifdef TRACE_UNTESTED
    if (Scope == &CARD_LIST::card_list) {
    }else{itested();
    }
#endif
    IO::plotset = false;
    assert(_probe_lists);
    do_probe(cmd, "print", Scope);
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "iprint|print|probe|`iprint|`print|`probe", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
