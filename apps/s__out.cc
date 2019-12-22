/*$Id: s__out.cc 2016/09/22 $ -*- C++ -*-
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
 * tr,dc analysis output functions (and some ac)
 */
//testing=obsolete,script 2005.09.17
#include "globals.h"
#include "u_sim_data.h"
#include "u_status.h"
#include "u_out.h"
#include "m_wave.h"
#include "u_prblst.h"
#include "declare.h"	/* plottr, plopen */
#include "s__.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
bool SIM::outset(CS& cmd)
{
  if(_output){
    return _output->set(cmd);
  }else{ untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void SIM::outreset()
{
  if(_output){
    _output->reset();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
PROBELIST const* SIM::outprobes() const
{
  if(_output){
    return _output->probes();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
/* SIM::out: output the data, "keep" for ac reference
 * x = the x coordinate
 * print = selected points, "print" to screen, files, etc.
 * store = all points, for internal postprocessing, measure
 * keep = after the command is done, dcop for ac
 */
void SIM::outcommit(int outflags)
{
  ::status.output.start();
  if (outflags & OUTPUT::ofKEEP) {
    _sim->keep_voltages();
  }else{
  }

  if (!(outflags & OUTPUT::ofPRINT)) {
    ++::status.hidden_steps;
  }else{
  }

  if(_output){
    _output->commit(outflags);
  }else{ untested();
  }

  if (outflags & OUTPUT::ofPRINT) {
    _sim->reset_iteration_counter(iPRINTSTEP);
    ::status.hidden_steps = 0;
  }else{
  }
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
// obsolete
void SIM::outdata(double const& x, int outflags)
{ untested();
  _sim->_axes.hack(&x);   // bit of a hack.
  outcommit(outflags); // go for it.
}
/*--------------------------------------------------------------------------*/
/* SIM::head: print column headings and draw plot borders
 * obsolete version, all output functions start with "out"
 */
void SIM::he_ad(double start, double stop, const std::string& col1)
{
  _sim->_axes.set_axis(0, NULL, col1, start, stop);
  outhead();
}
/*--------------------------------------------------------------------------*/
void SIM::outinit()
{
  if(_output){
    _output->init();
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
void SIM::outhead()
{
  if(_output){
    _output->h_ead();
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
void SIM::finish()
{
  outflush();
}
/*--------------------------------------------------------------------------*/
void SIM::outflush()
{
  if(_output){
    _output->flush();
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
//OUTPUT* SIM::output()
//{
//  return _output;
//}
/*--------------------------------------------------------------------------*/
OUTPUT* SIM::attach_output(OUTPUT& o)
{
  if(_output){
    // let output decide.
    _output = _output->attach_output(o);
  }else{
    _output = &o;
  }
  return _output;
}
/*--------------------------------------------------------------------------*/
void SIM::detach_output(OUTPUT& o)
{
  if(_output == &o){
    _output = NULL;
  }else{
    _output->detach_output(o);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
