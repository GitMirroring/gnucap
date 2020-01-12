/*     -*- C++ -*-
 * Copyright (C) 2017 Felix Salfelder
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
 * attach tees to simulation commands
 */
#include "u_out.h"
#include "u_sim_data.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::head(double start, double stop, const std::string& col1)
{
  OUTPUT::head(start, stop, col1);
  if(empty()){
    // print something by default
    // this is to imitate pre-output behaviour
    int width=std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);
    out().form(format, '#', col1.c_str());
    out() << '\n';
  }else{
    for(outputs_type::const_iterator p=_outputs.begin();
        p!=_outputs.end(); ++p){
      (*p)->head(start, stop, col1);
    }
  }
}
/*--------------------------------------------------------------------------*/
OUTPUT_TEE::~OUTPUT_TEE()
{
  trace1("~tee", _outputs.size());
  assert(empty());
}
/*--------------------------------------------------------------------------*/
OUTPUT* OUTPUT_TEE::set(CS& cs)
{
  trace2("outset TEE", cs.tail(), _outputs.size());

  Get(cs, "pl{ot}", &IO::plotset)
    || OUTPUT::set(cs); // parser hidden here.

  // propagate (necessary?)
  for(outputs_type::iterator p=_outputs.begin();
      p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->set(out());
  }
  return this;
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::init()
{
  for(outputs_type::iterator p=_outputs.begin();
      p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->set(out()); // BUG? here?
    (*p)->init();
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::commit(double x, int Flags)
{
  if(empty()){
    // legacy
    OMSTREAM o=out();
    o.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    assert(x != NOT_VALID);
    o << x;
    o << '\n';
  }else{
    for(outputs_type::iterator p=_outputs.begin();
	p!=_outputs.end(); ++p){
      assert(*p);
      (*p)->commit(x, Flags);
    }
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::flush()
{
  for(outputs_type::iterator p=_outputs.begin();
      p!=_outputs.end(); ++p){
    (*p)->flush();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
