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
      _prb = new PROBELIST;
      assert(&probelist() == _prb);
      assert(_probe_lists);
      _probe_lists->insert(_prb);

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
	    _prb->add_list(cmd);
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
/*--------------------------------------------------------------------------*/
////BUG//// doesn't work, crashes, due to problem in PROBE_LISTS
// Even if it did, it is out of place in this file, because it has no connection
// to OUTPUT_CMD.  It could exist as a stand-alone plugin.
class CMD_PROBES : public CMD{
public:
  void do_it(CS& cmd, CARD_LIST*){
    if (cmd.umatch("clear ")) {
      CKT_BASE::_probe_lists->clear();
    }else if (cmd.umatch("list ")) {untested();
      incomplete();
    }else{untested();
      incomplete();
      // later
    }
  }
} cp;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "probes", &cp);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
