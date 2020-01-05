/*$Id: e_base.cc 2015/02/05 al $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
//testing=script 2014.07.04
#include "ap.h"
#include "globals.h" // data_dispatcher
#include "u_sim_data.h"
#include "m_wave.h"
#include "u_prblst.h"
#include "u_xprobe.h"
#include "e_base.h"
#include "u_out.h"
#include "ap.h"
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::tr_probe_num(const std::string&)const {return NOT_VALID;}
XPROBE CKT_BASE::ac_probe_ext(const std::string&)const {return XPROBE(NOT_VALID, mtNONE);}
/*--------------------------------------------------------------------------*/
SIM_DATA* CKT_BASE::_sim = NULL; 
/*--------------------------------------------------------------------------*/
CKT_BASE::~CKT_BASE()
{
  trace3("~CKT_BASE", _probes, short_label(), this);
  if (_probes == 0) {
  }else if (!_sim) {untested();
  }else{
    PROBE_LISTS::purge(this);
    assert(!has_probes());
  }
  trace2("", _probes, short_label());
  assert(!has_probes());
}
/*--------------------------------------------------------------------------*/
const std::string CKT_BASE::long_label()const
{
  //incomplete();
  std::string buffer(short_label());
  //for (const CKT_BASE* brh = owner(); exists(brh); brh = brh->owner()) {untested();
  //  buffer += '.' + brh->short_label();
  //}
  return buffer;
}
/*--------------------------------------------------------------------------*/
bool CKT_BASE::help(CS& Cmd, OMSTREAM& Out)const
{
  if (help_text() != "") {
    size_t here = Cmd.cursor();
    std::string keyword;
    Cmd >> keyword;
    CS ht(CS::_STRING, help_text());
    if (keyword == "") {
      Out << ht.get_to("@@");
    }else if (ht.scan("@@" + keyword + ' ')) {
      Out << ht.get_to("@@");
    }else if (keyword == "?") {
      while (ht.scan("@@")) {
	Out << "  " << ht.get_to("\n") << '\n';
      }
    }else{
      Cmd.warn(bWARNING, here, "no help on subtopic " + Cmd.substr(here));
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
// should not be required in new code.
double CKT_BASE::probe_num(const std::string& what)const
{
  double x;
  if (_sim->analysis_is_ac()) { untested();
    x = ac_probe_num(what);
  }else{
    x = tr_probe_num(what);
  }
  return (std::abs(x)>=1) ? x : floor(x/OPT::floor + .5) * OPT::floor;
}
/*--------------------------------------------------------------------------*/
double CKT_BASE::ac_probe_num(const std::string& what)const
{
  size_t length = what.length();
  mod_t modifier = mtNONE;
  bool want_db = false;
  char parameter[BUFLEN+1];
  strcpy(parameter, what.c_str());

  if (length > 2  &&  Umatch(&parameter[length-2], "db ")) {
    want_db = true;
    length -= 2;
  }
  if (length > 1) { // selects modifier based on last letter of parameter
    switch (fix_case(parameter[length-1])) {
      case 'm': modifier = mtMAG;   length--;	break;
      case 'p': modifier = mtPHASE; length--;	break;
      case 'r': modifier = mtREAL;  length--;	break;
      case 'i': modifier = mtIMAG;  length--;	break;
      default:  modifier = mtNONE;		break;
    }
  }
  parameter[length] = '\0'; // chop
  
  // "p" is "what" with the modifier chopped off.
  // Try that first.
  XPROBE xp = ac_probe_ext(parameter);

  // If we don't find it, try again with the full string.
  if (!xp.exists()) {
    xp = ac_probe_ext(what);
    if (!xp.exists()) {
      // Still didn't find anything.  Print "??".
    }else{untested();
      // The second attempt worked.
    }
  }
  return xp(modifier, want_db);
}
/*--------------------------------------------------------------------------*/
// used in d_subckt to collect power (dc, tran)
// and in switch to get input (dc, tran).
/*static*/ double CKT_BASE::probe(const CKT_BASE *This, const std::string& what)
{
  assert (!_sim->analysis_is_ac());

  if (This) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
/*static*/ WAVE const* CKT_BASE::find_wave(const std::string& probe_name)
{
  trace2("find_wave", probe_name, _sim->label());
  CKT_BASE* wl = data_dispatcher[_sim->label()];
//  CKT_BASE* wl = _sim->_current?
  if(!wl){untested();
  }else if(WAVESTASH* WL=dynamic_cast<WAVESTASH*>(wl)){
    WAVESTASH::const_iterator w = WL->find(probe_name);
    if(w!=WL->end()) {
      return &(w->second);
    }else{
    }
  }else{ untested();
  }

  return NULL;
}
/*--------------------------------------------------------------------------*/
namespace legacy{
// do dictionary lookup at run time (legacy)
// not required in new code.
class string_probe : public PROBE_BASE{
private:
  explicit string_probe(CKT_BASE const* b, const std::string& w)
    : PROBE_BASE(w, b)
  {
    assert(what()==w);
    if(brh()){
      set_label(w + "(" + brh()->long_label() + ")");
    }else{
      set_label(w);
    }
  }
public:
  ~string_probe(){
    if(brh()){
    }else{ untested();
    }
  }
public:
  double value() const{ itested();
    assert(brh());
    double x;
    if (_sim->analysis_is_ac()) {
      x = brh()->ac_probe_num(what());
    }else{
      x = brh()->tr_probe_num(what());
    }
    if (std::abs(x)>=1){
       return x;
    }else{
      return floor(x/OPT::floor + .5) * OPT::floor;
    }
  }
private:
  friend class CKT_BASE;
};
} // legacy
/*--------------------------------------------------------------------------*/
PROBE_BASE const* CKT_BASE::new_probe(std::string const& s) const
{
  PROBE_BASE* n=new legacy::string_probe(this, s);
  assert(!n->has_probes());
  return n;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
