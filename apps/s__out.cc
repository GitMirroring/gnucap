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
// trace .. show interim results
void SIM::out_trace(double XX)
{
  ::status.output.start();
  ++::status.hidden_steps;
  if(_output){
    _output->commit(XX, OUTPUT::ofTRACE);
  }else{ untested();
  }
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
// commit .. commit, print, plot, etc. data point
void SIM::out_commit(double XX)
{
  ::status.output.start();
  if(_output){
    _output->commit(XX, OUTPUT::ofPRINT|OUTPUT::ofSTORE);
  }else{ untested();
  }
  _sim->reset_iteration_counter(iPRINTSTEP);
  ::status.hidden_steps = 0;
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
// commit .. commit, print, plot, etc. data point
void SIM::out_commit_hide(double XX)
{
  ::status.output.start();
  ++::status.hidden_steps;
  if(_output){
    _output->commit(XX, OUTPUT::ofSTORE);
  }else{ untested();
  }
  ::status.output.stop();
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
void SIM::outhead(double start, double stop, const std::string& col1)
{
  if(_output){
    _output->head(start, stop, col1);
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
void SIM::attach_output(OUTPUT* o)
{
  if(_output){
    _output->attach_output(o);
  }else{
    _output = o;
  }
}
/*--------------------------------------------------------------------------*/
void SIM::detach_output(OUTPUT* o)
{
  if(_output == o){
    _output = NULL;
  }else{
    _output->detach_output(o);
  }
}
/*--------------------------------------------------------------------------*/
void SIM::attach_new_tee()
{
  _output = new OUTPUT_TEE;
}
/*--------------------------------------------------------------------------*/
void SIM::delete_outputs()
{
  assert(_output);
  delete _output;
  _output = NULL;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
