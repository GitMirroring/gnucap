/*$Id: u_prblst.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
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
 * probe list functions
 */
////BUG//// inappropriate use of dispatcher.
// as used here, no real advantage over std::map
// used only here, therefore should not be global.
// so change to a private std::map inside PROBE_LISTS

/// as it is, PROBE_LISTS manages DISPATCHER<PROBELIST>.  PROBE_LISTS has no
/// instance. but there is a "clear" command, accessing probe_lists through
/// PROBE_LISTS::clear (this seems to be needed).  a map in PROBE_LISTS would
/// require an instance (new global) and make probe_lists obsolete. replace the
/// dispatcher?

//testing=failed 2020.01.19
#include "c_comand.h"
#include "e_cardlist.h"
#include "e_node.h"
#include "e_card.h"
#include "u_nodemap.h"
#include "ap.h"
#include "u_prblst.h"
#include "globals.h"
#include "io_error.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
PROBE_LISTS::container_type PROBE_LISTS::_map;
/*--------------------------------------------------------------------------*/
PROBE_LISTS::~PROBE_LISTS()
{untested();
  // clear command does that.
  assert(_map.begin()==_map.end());
}
/*--------------------------------------------------------------------------*/
void PROBE_LISTS::clear()
{
  for(iterator i=_map.begin(); i!=_map.end(); ++i){
    PROBELIST* P = i->second;
    assert(P);

    P->clear();
  }
  _map.clear();
}
/*--------------------------------------------------------------------------*/
PROBELIST& PROBE_LISTS::get(std::string const& reason, CMD const* sim)
{
  PROBELIST*& p=_map[reason];

  if(p){ untested();
    trace1("probelist exists", reason);
    // assert(sim==p->_sim); need to rethink probe_dispatcher anyway.
    //                       this might become simpler with probes attached to
    //                       the output directly...
  }else if(sim){
    p = new PROBELIST(sim);
  }else{
    unreachable();
  }
  return *p;
}
/*--------------------------------------------------------------------------*/
void PROBE_LISTS::purge(CKT_BASE* brh)
{
  for(iterator i=_map.begin(); i!=_map.end(); ++i){
    PROBELIST* l=prechecked_cast<PROBELIST*>(i->second);
    if(l){
      l->remove_one(brh);
    }else{untested(); untested();
      // uninstalled already
    }
  }
}
/*--------------------------------------------------------------------------*/
PROBELIST::~PROBELIST()
{ untested();
  CMD* s=const_cast<CMD*>(_sim);
  s->detach_output(this);
}
/*--------------------------------------------------------------------------*/
void PROBELIST::listing(const std::string& label)const
{
  IO::mstdout.form("%-7s", label.c_str());
  for (const_iterator p = begin();  p != end();  ++p) {
    assert(*p);
    IO::mstdout << ' ' << (*p)->short_label();
    if ((*p)->param_count() != 0.) {
      // use u_lang?
      IO::mstdout.setfloatwidth(5) << '(' << (*p)->param_value(0);
      for(int i=1; i<(*p)->param_count(); ++i){
	IO::mstdout << ',' << (*p)->param_value(1);
      }
      IO::mstdout << ')';
    }else{
    }
  }
  IO::mstdout << '\n';
}
/*--------------------------------------------------------------------------*/
void PROBELIST::clear()
{ untested();
  trace2("PROBELIST::clear", bag.size(), this);
  erase(begin(), end());
  assert(begin() == end());
}
/*--------------------------------------------------------------------------*/
void PROBELIST::erase(PROBELIST::iterator b, PROBELIST::iterator e)
{
  for (iterator i=b; i!=e; ++i) { untested();
    assert (*i);
    trace1("PROBELIST::erase deleting", (*i)->short_label());
    delete(*i);
    *i = NULL;
  }
  trace0("PROBELIST::erase");
  bag.erase(b,e);
  trace0("PROBELIST::erase done");
}
/*--------------------------------------------------------------------------*/
/* check for match
 * called by STL remove, below
 * both are needed to support different versions of STL
 */
//bool operator==(const PROBE_BASE& prb, const std::string& par)
//{ untested();
//  trace2("==", prb.label(), par);
//  return wmatch(prb.label(), par);
//}
bool operator!=(const PROBE_BASE& prb, const std::string& par)
{untested();
  //return !wmatch(prb.label(), par);
  return !(prb == par);
}
bool operator==(PROBE_BASE const* prb, std::string const& par)
{ untested();
  assert(prb);
  return wmatch(prb->short_label(), par);
}
bool operator!=(PROBE_BASE const* prb, std::string const& par)
{ untested();
  return !(prb == par);
}
/*--------------------------------------------------------------------------*/
/* remove a complete probe, extract from CS
 * wild card match  ex:  vds(m*)
 */
void PROBELIST::remove_list(CS& cmd)
{ 
  unsigned mark = cmd.cursor();
  std::string parameter(cmd.ctos(TOKENTERM) + '(');
  int paren = cmd.skip1b('(');
  parameter += cmd.ctos(TOKENTERM) + ')';
  paren -= cmd.skip1b(')');
  if (paren != 0) {untested();
    cmd.warn(bWARNING, "need )");
  }else if (parameter.empty()) {untested();
    cmd.warn(bWARNING, "what's this?");
  }else{
  }

  bool gone=false;

  for (iterator p = begin();  p != end(); ) {
    assert (*p);
    if ((**p)==parameter){
      gone = true;
      delete(*p);
      p = bag.erase(p);
    } else {
      ++p;
    }
  }

  if (!gone) { untested();
    cmd.warn(bWARNING, mark, "probe isn't set -- can't remove");
  }else{
  }
  trace1("PROBELIST::remove_list", bag.size());
}
/*--------------------------------------------------------------------------*/
/* check for match
 * called by STL remove, below
 * both are needed to support different versions of stl
 */
// bool operator==(const PROBE_BASE& prb, const CKT_BASE* b)
// { untested();
//   return (prb.brh() == b);
// }
//bool operator!=(const PROBE_BASE& prb, const CKT_BASE* b)
//{untested();
//  return (prb.brh() != b);
//}
bool operator==(PROBE_BASE const* prb, CKT_BASE const& brh)
{
  return *prb == brh;
}
bool operator!=(PROBE_BASE const* prb, CKT_BASE const& brh)
{ untested();
  return *prb != brh;
}
/*--------------------------------------------------------------------------*/
/* remove a brh from a PROBELIST
 * removes all probes on brh
 */
namespace detail{
struct probe_finder_deleter {
  probe_finder_deleter(CKT_BASE const* brh)
    : _b(brh)
  {
  }
  bool operator()(PROBE_BASE const*& p) const {
    if (p == *_b) {
//      trace4("finder delete", _b->short_label(), _b, p, p->brh());
 //     trace2("deleting", p, p->object());
      delete const_cast<PROBE_BASE*>(p);
      return true;
    }else{
      trace1("unequal probes", _b->short_label());
      return false;
    }
  }
  CKT_BASE const* _b;
};
} // detail
/*--------------------------------------------------------------------------*/
void PROBELIST::remove_one(CKT_BASE *card)
{
  assert(card);
  trace3("removing probes", size(), card, card->short_label());

  detail::probe_finder_deleter d(card);
  iterator new_end=remove_if ( begin(), end(), d );
  bag.erase( new_end, end());
}
/*--------------------------------------------------------------------------*/
/* add_list: add a "list" of probes, usually only one
 * This means possibly several probes with a single parameter
 * like "v(r*)" meaning all resistors
 * but not "v(r4) v(r5)" which has two parameters.
 * It no longer takes care of setting the range for plot or alarm.
 */
void PROBELIST::add_list(CS& cmd)
{
  std::string what(cmd.ctos(TOKENTERM));/* parameter */
  if (what.empty()) {
    cmd.warn(bWARNING, "need a probe");
  }else{
  }

  int paren = cmd.skip1b('(');		/* device, node, etc. */
  if (cmd.umatch("nodes ")) {
    // all nodes
    add_all_nodes(what);
  }else if (cmd.is_alnum() || cmd.match1("*?")) {
    // branches or named nodes
    unsigned here1 = cmd.cursor();
    bool found_something = add_branches(cmd.ctos(), what,
                                        &CARD_LIST::card_list);
    if (!found_something) {
      cmd.warn(bWARNING, here1, "no match");
    }else{
    }
    for (;;) {
      // a list, as in v(r1,r2,r3) or v(1,2,3)
      if (!(cmd.is_alnum() || cmd.match1("*?"))) {
	break;
      }else{
      }
      unsigned here2 = cmd.cursor();
      found_something = add_branches(cmd.ctos(), what,
	                             &CARD_LIST::card_list);
      if (!found_something) {untested();
	cmd.reset(here2);
	break;
      }else{
      }
    }
  }else{
    cmd.warn(bDANGER, "need device or node");
  }
  paren -= cmd.skip1b(')');
  if (paren != 0) {
    cmd.warn(bWARNING, "need )");
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void PROBELIST::push_new_probe(const std::string& param, CKT_BASE const* obj)
{
  assert(obj);
  PROBE_BASE const* n=obj->new_probe(param);
  if(_sim){
    n = _sim->tap_probe(n);
  }else{
  }
  bag.push_back(n);
}
/*--------------------------------------------------------------------------*/
void PROBELIST::add_all_nodes(const std::string& what)
{
  for (NODE_MAP::const_iterator
       i = CARD_LIST::card_list.nodes()->begin();
       i != CARD_LIST::card_list.nodes()->end();
       ++i) {
    std::string const& nn=i->first;
    if (nn.find('.') == std::string::npos) {
      NODE* node = i->second;
      assert (node);
      try{
	push_new_probe(what, node);
      }catch(Exception_Cant_Find& e){
	error(bNOERROR, "probe wildcard: skipping %s on %s\n",
	    what.c_str(), nn.c_str());
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
/* add_branches: add net elements to probe list
 * 	all matching a label with wildcards
 */
bool PROBELIST::add_branches(const std::string&device, 
			     const std::string&param,
			     const CARD_LIST* scope)
{
  assert(scope);
  bool found_something = false;

  std::string::size_type dotplace = device.find_first_of(".");
  if (dotplace != std::string::npos) {
    // has a dot, look deeper
    { // forward (Verilog style)
      std::string dev = device.substr(dotplace+1, std::string::npos);
      std::string container = device.substr(0, dotplace);
      for (CARD_LIST::const_iterator
	     i = scope->begin();  i != scope->end();  ++i) {
	CARD* card = *i;
	if (card->is_device()
	    && card->subckt()
	    && wmatch(card->short_label(), container)) {
	  found_something |= add_branches(dev, param, card->subckt());
	}else{
	}
      }
    }
    { // reverse (ACS style)
      dotplace = device.find_last_of(".");
      std::string container = device.substr(dotplace+1, std::string::npos);
      std::string dev = device.substr(0, dotplace);
      for (CARD_LIST::const_iterator
	     i = scope->begin();  i != scope->end();  ++i) {
	CARD* card = *i;
	if (card->is_device()
	    && card->subckt()
	    && wmatch(card->short_label(), container)) {
	  found_something |= add_branches(dev, param, card->subckt());
	}else{
	}
      }
    }
  }else{
    // no dots, look here
    if (device.find_first_of("*?") != std::string::npos) {
      // there's a wild card.  do linear search for all
      {
	for (NODE_MAP::const_iterator 
	     i = scope->nodes()->begin();
	     i != scope->nodes()->end();
	     ++i) {
	  if (i->first == "0") {
	// cast to GROUND_NODE instead?
	  }else{
	    NODE* node = i->second;
	    assert (node);
	    if (wmatch(node->short_label(), device)) {
	      push_new_probe(param, node);
	      found_something = true;
	    }else{
	    }
	  }
	}
      }
      {// components
	for (CARD_LIST::const_iterator 
	     i = scope->begin();  i != scope->end();  ++i) {
	  CARD* card = *i;
	  if (wmatch(card->short_label(), device)) {
	    push_new_probe(param, card);
	    found_something = true;
	  }else{
	  }
	}
      }
    }else{
      // no wild card.  do fast search for one
      { // nodes
	NODE* node = (*scope->nodes())[device];
	if (node) {
	  push_new_probe(param, node);
	  found_something = true;
	}else{
	}
      }
      { //components
	CARD_LIST::const_iterator i = scope->find_(device);
	if (i != scope->end()) {
	  push_new_probe(param, *i);
	  found_something = true;
	}else{
	}
      }
    }
  }

  return found_something;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
