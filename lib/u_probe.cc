/*$Id: u_probe.cc 2016/09/22 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2017, 2019 Felix Salfelder
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
#include "u_status.h"
#include "e_base.h"
#include "u_probe.h"
/*--------------------------------------------------------------------------*/
bool PROBE_BASE::operator==(const CKT_BASE& b)const
{
  if(PROBE_BASE const* p=dynamic_cast<PROBE_BASE const*>(brh())){
    return *p==b;
  }else{
    return (brh() == &b);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* "detach" a probe from a device
 * which means ...  1. tell the device that the probe has been removed
 *		    2. blank out the probe, so it doesn't reference anything
 * does not remove the probe from the list
 */
void PROBE_BASE::detach()
{
  if (!_brh) {
  }else if(_brh->has_probes()){
    _brh->dec_probes();
  }else{ untested();
    unreachable();
    trace1("",_what);
  }
  _what = "";

  if( PROBE_BASE const* p=dynamic_cast<PROBE_BASE const*>(_brh)) {
    delete p;
//  }else if( COMPONENT const* c=dynamic_cast<COMPONENT const*>(_brh)){ untested();
//  }else{ untested();
//    unreachable();
  }
  _brh = NULL;
}
/*--------------------------------------------------------------------------*/
void PROBE_BASE::set_param_by_index(int, double){ untested();
   // pass string value, use PARAMETERs?
  incomplete(); // currently
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
RANGE_PROBE::RANGE_PROBE(const std::string& What, PROBE_BASE const*Brh)
  :PROBE_BASE(What, Brh),
   _lo(0.),
   _hi(0.)
{
  assert(Brh);
  trace1("RANGE_PROBE::RANGE_PROBE", What);
  set_label(Brh->label());
}
/*--------------------------------------------------------------------------*/
RANGE_PROBE::RANGE_PROBE(const RANGE_PROBE& p)
  :PROBE_BASE(p),
   _lo(p._lo),
   _hi(p._hi)
{ untested();
  incomplete();
}
/*--------------------------------------------------------------------------*/
void RANGE_PROBE::set_param_by_index(int i, double d){
  switch(i){
    case 0:
      _lo = d;
      break;
    case 1:
      _hi = d;
      break;
    default: untested();
      incomplete(); // need to throw? pass to baseclass?
      unreachable(); // currently not needed.
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
