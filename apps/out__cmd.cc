/*                            -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2018 Felix Salfelder
 * Author: Felix Salfelder <felix@salfelder.org>
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
 * output commands
 */
////BUG//// this file needs to move to lib.
// apps is a collection of plugins, all optional, all independent.
// This is an essential part of the system, not optional.
// This file is a library, used to build output plugins.
// apps sources are not usually installed, so this file
// would not be available for additional modules if left in apps.

//testing=script 2020.01.16
#include "u_sim_data.h"
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
//#include "trace_on.h"
/*--------------------------------------------------------------------------*/
void OUTPUT_CMD::setup(CS& cmd)
{
  trace1("setup", cmd.tail());
  unsigned here = cmd.cursor();
  std::string s;
  cmd >> s;
  if (CMD* sim = command_dispatcher[s]) {
    trace2("attaching sink", s, short_label());
    std::string reason=short_label() + ":" + s;

    container_type::iterator a=_sinks.find(sim);
    OUTPUT_CMD* sink;
    if(a==_sinks.end() || !a->second){
      if(a==_sinks.end()){
	// really a new sink
      }else if(!a->second){untested();
	// had one before but lost it, so make a new one
      }else{untested();
	unreachable();
      }

      trace2("new sink", s, short_label());
      setup_probelist(reason);
      assert(&probelist() == _prb);

      //OUTPUT_CMD* o=clone();
      //sink = prechecked_cast<OUTPUT_CMD*>(o);
      sink = clone();

      assert(sink);
      assert(&sink->probelist() == _prb);
      _sinks[sim] = sink;
      assert(sink);
    }else{
      trace2("reusing sink", s, short_label());
      sink = prechecked_cast<OUTPUT_CMD*>(a->second);
      assert(sink);
      //assert(&sink->probelist() != _prb);
      //assert(&sink->probelist() == _prb);
      _prb = &sink->probelist();
    }
    sink->set_simname(s);
    sim->attach_output(sink);
    assert(&sink->probelist() == _prb);
  }else{
    trace2("no sim, no sink", s, short_label());
    cmd.reset(here);
    _prb = NULL;
  }
}
/*--------------------------------------------------------------------------*/
#if 0
// apply extra args to (newly added) probes
// possibly wrap probe into another one.
static void probeargs(CS& cmd,
    PROBE_BASE const* wrap,
    PROBELIST::iterator p, PROBELIST::iterator e)
{
  ////BUG//// only works for 2 args.

  ////BUG//// This really belongs to PROBE and PROBELIST, not here.
  // It really operates on a PROBE, so that's where it really belongs.
  // through a PROBELIST, where PROBEs are stored.
  // so here in OUTPUT_CMD is really two levels removed from where it belongs.
  // Polymorphic probes need work.  Will back out for now, reverting to the old
  // implementation of PROBE and PROBELIST.  This will make it possible to 
  // move ahead with output plugins, which is what this is all about.

  double a0, a1;
#if 1
  bool have_args = (cmd >> '(') && (cmd >> a0 >> a1 >> ')');
#else
  bool have_args=false;
  if (cmd.skip1b('(')) {
    // extra probe parameters (such as range)
    a0 = cmd.ctof();
    a1 = cmd.ctof();
    have_args = true;
    if (!cmd.skip1b(')')) {untested();
      cmd.check(bWARNING, "need )");
    }else{
    }
  }else{
    have_args=false;
  }
#endif

  for (; p!=e; ++p) {
    PROBE_BASE const* cP=dynamic_cast<PROBE_BASE const*>(*p);
    PROBE_BASE* P=const_cast<PROBE_BASE*>(cP);
    if(wrap){
      P = wrap->new_wrap(P);
      *p = P;
    }else{
    }
    if(have_args){
      P->set_param_by_index(0, a0);
      P->set_param_by_index(1, a1);
    }else{
    }
  }
}
#endif
/*--------------------------------------------------------------------------*/
// former do_probe
void OUTPUT_CMD::do_it(CS& cmd, CARD_LIST*)
{
  trace1("doing it", cmd.tail());
  CKT_BASE::_sim->set_command_none();
  enum {aADD, aDELETE, aNEW} action;

  if (cmd.match1('-')) {untested();	/* handle .probe - ac ...... */
    action = aDELETE;			/* etc. 		     */
    cmd.skip();
  }else if (cmd.match1('+')) {untested();
    action = aADD;
    cmd.skip();
  }else{			/* no -/+ means clear, but wait for */
    action = aNEW;		/* .probe ac + ..... 		    */
  }				/* which will not clear first	    */

  // cmd is something like "ac", "dc" ...
  // this results in NULL, if there is no SIM registered.
  setup(cmd);

  if (!_prb) {
    // go through all sims that have been mentioned
    if (cmd.is_end()) {
      // list probes for this output command.
      // forall simulations
      for(container_type::const_iterator i=_sinks.begin(); i!=_sinks.end(); ++i){
	OUTPUT_CMD const* S=prechecked_cast<OUTPUT_CMD const*>(i->second);
	assert(S);
	PROBELIST const& pl=S->probelist();
	pl.listing(S->simname());
      }
    }else if (cmd.umatch("clear ")) {untested();
      // clear all
      for(container_type::const_iterator i=_sinks.begin(); i!=_sinks.end(); ++i){untested();
	OUTPUT_CMD* S=prechecked_cast<OUTPUT_CMD*>(i->second);
	assert(S);
	PROBELIST& pl=S->probelist();
	pl.clear();
      }
      detach_sinks();
    }else{untested();
      throw Exception_CS("what's this?", cmd);
    }
  }else{
    if (cmd.is_end()) {				/* list */
      _prb->listing("");
    }else if (cmd.umatch("clear ")) {
      _prb->clear();
    }else{					/* add/remove */
      CKT_BASE::_sim->init();
      if (cmd.match1('-')) {			/* setup cases like: */
	action = aDELETE;			/* .probe ac + ....  */
	cmd.skip();
      }else if (cmd.match1('+')) {
	action = aADD;
	cmd.skip();
      }else{
      }
      if (action == aNEW) {			/* no +/- here or at beg. */
	_prb->clear();				/* means clear first	  */
	action = aADD;
      }else{
      }
      while (cmd.more()) {			/* do-it */
	if (cmd.match1('-')) {			/* handle cases like:	    */
	  action = aDELETE;			/* .pr ac +v(7) -e(6) +r(8) */
	  cmd.skip();
	}else if (cmd.match1('+')) {
	  action = aADD;
	  cmd.skip();
	}else{
	}
	if (action == aDELETE) {
	  _prb->remove_list(cmd);
	}else{
	  assert(_prb);
	  unsigned here1=cmd.cursor();
	  try{
	    //int s=int(_prb->end()-_prb->begin());
	    _prb->add_list(cmd);
	    //PROBELIST::iterator seek=_prb->begin()+s;
	    //probeargs(cmd, probe_proto(), seek, _prb->end());
	  }catch(Exception_Cant_Find& e){
	    cmd.warn(bWARNING, here1, "cannot resolve");
	  }
	}
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_CMD::detach_sinks()
{
  for (container_type::iterator i=_sinks.begin(); i!=_sinks.end(); ++i) {
    if (CMD* sim = i->first) {
      sim->detach_output(i->second);
    }else{untested();
      unreachable();
    }
    delete i->second;
    i->second = NULL;
  }
}
/*--------------------------------------------------------------------------*/
PROBELIST& OUTPUT_CMD::prblist(std::string const& ) ////reason)
{
  static PROBELIST x;
  return x; ////BUG//// PROBE_LISTS::get(reason);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if 0
////BUG//// doesn't work, crashes, due to problem in PROBE_LISTS
// Even if it did, it is out of place in this file, because it has no connection
// to OUTPUT_CMD.  It could exist as a stand-alone plugin.
class CMD_PROBES : public CMD{
public:
  void do_it(CS& cmd, CARD_LIST*){
    if (cmd.umatch("clear ")) {
      PROBE_LISTS::clear();
    }else if (cmd.umatch("list ")) {untested();
      incomplete();
    }else{untested();
      incomplete();
      // later
    }
  }
} cp;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "probes", &cp);
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
