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
void SIM::out_reset()
{
  if(_output){
    _output->reset();
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
bool SIM::out_set(CS& cmd)
{
  if(_output){
    return _output->set(cmd);
  }else{ untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void SIM::out_init(TRACE Trace)
{
  ::status.output.start();
  DATALEVEL dl = dl_ACCEPTED;
  switch (Trace) {
  case tNONE:	    dl=dl_STROBE; break;
  case tUNDER:	    dl=dl_STROBE; break;
  case tALLTIME:    dl=dl_ACCEPTED; break;
  case tREJECTED:   dl=dl_REJECTED; break;
  case tITERATION:  dl=dl_ITERATING; break;
  case tVERBOSE:    dl=dl_NONE; break;
  }

  if(_output){
    _output->init(dl, _sim->label());
  }else{ untested();
  }
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::out_head(double start, double stop, const std::string& col1)
{ untested();
  ::status.output.start();
  if(_output){ untested();
    _output->head(start, stop, col1);
  }else{ untested();
  }
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::out_commit(double XX, int Level)
{
  ::status.output.start();
  if(_output){ untested();
    _output->commit(XX, Level);
  }else{ untested();
  }
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::out_flush()
{
  ::status.output.start();
  if(_output){
    _output->flush();
  }else{ untested();
  }
  ::status.output.stop();
}
/*--------------------------------------------------------------------------*/
void SIM::attach_output(OUTPUT* o)
{
  OUTPUT::attach(o, _output);
}
/*--------------------------------------------------------------------------*/
void SIM::detach_output(OUTPUT* o)
{
  OUTPUT::detach(o, _output);
}
/*--------------------------------------------------------------------------*/
void SIM::attach_new_tee()
{
  // this is optional, but required to send data to multiple sinks.
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
