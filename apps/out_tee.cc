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
////BUG//// this file needs to move to lib.
// apps is a collection of plugins, all optional, all independent.
// This is an essential part of the system, not optional.
// OUTPUT_TEE is acccessed directly in s__out.cc, not through the plugin mechanism.
// Therefore it cannot be a plugin.

/// it was designed to be optional. the OUTPUT_TEE was only attached to a
/// simulation command as-needed (e.g. in the spice tests). As part of lib and
/// attached in s__out (like now), it will no longer be possible to not
/// use it.

// It could be a plugin by giving it a name, registering with a dispatcher,
// and always using it that way, but it would still be required
// because the functionality is needed even in the most basic sense.
// Examples: bm_cond.cc, bm_value.cc, d_subckt.cc

/// out_tee.cc was (intended as) a plugin. there was no need for registering
/// with a dispatcher, because it was enabled by loading it with the default
/// plugins.
///
/// (either way, I don't object to hardwire it in lib, and this might simplify
/// things.)

//testing=script,complete 2020.01.14
#include "ap.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
OUTPUT_TEE::~OUTPUT_TEE()
{
  trace1("~tee", _outputs.size());
}
/*--------------------------------------------------------------------------*/
OUTPUT* OUTPUT_TEE::set(CS& cs)
{
  trace2("outset TEE", cs.tail(), _outputs.size());

  Get(cs, "pl{ot}", &IO::plotset)
    || OUTPUT::set(cs); // parser hidden here.

  // propagate (necessary?)
  for(outputs_type::iterator p=_outputs.begin(); p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->set(out());
  }
  return this;
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::init(int Dl, const std::string& Label)
{
  for(outputs_type::iterator p=_outputs.begin(); p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->set(out()); // BUG? here?
    (*p)->init(Dl, Label);
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::head(double start, double stop, const std::string& col1)
{
  for(outputs_type::const_iterator p=_outputs.begin(); p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->head(start, stop, col1);
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::commit(double x, int Level)
{ itested();
  trace1("TEE::commit", _outputs.size());
  for(outputs_type::iterator p=_outputs.begin(); p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->commit(x, Level);
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::flush()
{
  for(outputs_type::iterator p=_outputs.begin(); p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->flush();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
