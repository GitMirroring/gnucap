/*$Id: u_probe.cc 2016/09/22 al $ -*- C++ -*-
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
 * general probe object
 */
//testing=script 2009.06.21
#include "u_sim_data.h"
#include "u_status.h"
#include "e_card.h"
#include "u_probe.h"
#include "e_cardlist.h"
#include "u_nodemap.h"
#include "e_node.h"
#include "ap.h"
#include "u_xprobe.h"
/*--------------------------------------------------------------------------*/
class PROBE0 : public CKT_BASE {
  mutable int _probes{0};
public:
  PROBE0(){ set_label("0"); }
public:// doesnt work
//   double tr_probe_num(std::string const&x)const override { untested();
//     return probe_num(x);
//   }
//   XPROBE ac_probe_ext(std::string const&x)const override { untested();
//     return XPROBE(probe_num(x));
//   }
  double probe_num(std::string const&)const;
private:
  void  inc_probes()const override {++_probes;}
  void  dec_probes()const override {assert(_probes>0); --_probes;}
}probe0;
CKT_BASE* prb0 = &probe0;
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const std::string& what,const CARD *brh)
  :CKT_BASE(),
   _what(what),
   _brh(brh),
   _lo(0.),
   _hi(0.)
{
  if (_brh) {
    _brh->inc_probes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const PROBE& p)
  :CKT_BASE(p),
   _what(p._what),
   _brh(p._brh),
   _lo(p._lo),
   _hi(p._hi)
{
  if (_brh) {
    _brh->inc_probes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* operator=  ...  assignment
 * copy a probe
 */
PROBE& PROBE::operator=(const PROBE& p)
{
  detach();
  _what = p._what;
  _brh  = p._brh;
  _lo   = p._lo;
  _hi   = p._hi;
  if (_brh) {
    _brh->inc_probes();
  }else{
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* "detach" a probe from a device
 * which means ...  1. tell the device that the probe has been removed
 *		    2. blank out the probe, so it doesn't reference anything
 * does not remove the probe from the list
 */
void PROBE::detach()
{
  if (_brh) {
    _what = _what+"("+_brh->long_label()+")";
    _brh->dec_probes();
  }else{
    _what = "";
  }
  _brh = nullptr;
}
/*--------------------------------------------------------------------------*/
// same. but keep label
void PROBE::store()
{
  if(_brh){
    trace2("store probe", label(), _brh->short_label());
    detach();
    trace1("stored probe", _what);
  //  _what = s;
  }else{
    incomplete();
    // hmm node probe?
  }
  assert(!_brh);
}
/*--------------------------------------------------------------------------*/
static CKT_BASE const* find_device(CARD_LIST const* scope, std::string const& what)
{
  std::string::size_type dotplace = what.find_first_of(".");
  if (dotplace != std::string::npos) {
    std::string dev = what.substr(dotplace+1, std::string::npos);
    std::string container = what.substr(0, dotplace);
    for (CARD_LIST::const_iterator
	i = scope->begin();  i != scope->end();  ++i) {
      CARD* card = *i;
      if (card->is_device()
	  && card->subckt()
	  && card->short_label() == container) {

	return find_device(card->subckt(), dev);
      }else{
      }
    }
  }else{
    for (NODE_MAP::const_iterator 
	i = scope->nodes()->begin();
	i != scope->nodes()->end();
	++i) {
      if (i->first != "0") {
	NODE* node = i->second;
	assert (node);
	if (node->short_label() == what) {
	  return node;
	}else{
	}
      }else{
      }
    }
    {// components
      for (CARD_LIST::const_iterator
	  i = scope->begin();  i != scope->end();  ++i) {
	CARD* card = *i;
	if (card->short_label() == what){
	  return card;
	}else{
	}
      }
    }
  }
  return NULL;
}
/*--------------------------------------------------------------------------*/
void PROBE::restore(CARD_LIST const* scope)
{
  std::string s = _what;
  trace2("restore probe", _what, _brh);
  
  if(_brh){
  }else{
    CS cmd(CS::_STRING, s);
    std::string parameter(cmd.ctos(TOKENTERM));
    int paren = cmd.skip1b('(');
    std::string device(cmd.ctos(TOKENTERM));
    paren -= cmd.skip1b(')');
    assert(paren==0);

    if(device=="0"){
      _brh = &probe0;
      _what = parameter;
    }else{
      _brh = (*scope->nodes())[device];
      _what = parameter;
    }
    if(_brh){
      _brh->inc_probes();
    }else{
      trace2("restore probe", parameter, device);
      if (CKT_BASE const* cc = find_device(scope, device)) {
	_what = parameter;
	_brh = cc;
	_brh->inc_probes();
      }else{
	error(bTRACE, "device is gone %s %s %s\n", s.c_str(), parameter.c_str(), device.c_str());
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/* label: returns a string corresponding to a possible probe point
 * (suitable for printing)
 * It has nothing to do with whether it was selected or not
 */
const std::string PROBE::label(void)const
{
  if (_brh) {
    return _what + '(' + _brh->long_label() + ')';
  }else{
    return _what + "(0)";
  }
}
/*--------------------------------------------------------------------------*/
double PROBE::value(void)const
{
  // _brh is either a node or a "branch", which is really any device
  if (_brh == &probe0) {
    // avoid ac_probes, as they mess with what.
    return probe0.probe_num(_what);
  }else if (_brh) {
    return _brh->probe_num(_what);
  }else{
    return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
double PROBE0::probe_num(std::string const& _what) const
{
  if (Umatch(_what, "iter ")) {
    assert(iPRINTSTEP - sCOUNT == 0);
    assert(iSTEP      - sCOUNT == 1);
    assert(iTOTAL     - sCOUNT == 2);
    assert(iCOUNT     - sCOUNT == 3);
    return _sim->_iter[sCOUNT];
  }else if (Umatch(_what, "bypass ")) {untested();
    return OPT::bypass + 10*_sim->_bypass_ok;
  }else if (Umatch(_what, "control ")) {
    return ::status.control;
  }else if (Umatch(_what, "damp ")) {untested();
    return _sim->_damp;
  }else if (Umatch(_what, "gen{erator} ")) {untested();
    return _sim->_genout;
  }else if (Umatch(_what, "hidden ")) {
    return ::status.hidden_steps;
  }else if (Umatch(_what, "temp{erature} ")) {
    return _sim->_temp_c;
  }else if (Umatch(_what, "time ")) {untested();
    return _sim->_time0;
  }else{
    return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
