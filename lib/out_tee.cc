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
//testing=script,complete 2020.01.14
#include "ap.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
// namespace{
/*--------------------------------------------------------------------------*/
// class INTERFACE OUTPUT_TEE : public OUTPUT {
// [..]
// }
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
void OUTPUT_TEE::attach_output(OUTPUT* o)
{ untested();
	trace1("TEE attach", _outputs.size());
	_outputs.insert(o);
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::detach_output(CKT_BASE* b)
{ untested();

	bool got_it=false;
	if(OUTPUT* o=dynamic_cast<OUTPUT*>(b)){
		got_it = _outputs.erase(o);
	}else{
	}
	if(!got_it){
	  for(outputs_type::iterator p=_outputs.begin(); p!=_outputs.end(); ++p){
		 assert(*p);
		 (*p)->detach_output(b);
	  }
	}else{
	}
}
/*--------------------------------------------------------------------------*/
// class CMD_TEE : public CMD {
// // ?
// }p1;
// DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "attach_tee", &p1);
/*--------------------------------------------------------------------------*/
// } // namespace
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
