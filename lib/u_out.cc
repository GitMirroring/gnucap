/*                            -*- C++ -*-
 * Copyright (C) 2001,2020 Albert Davis
 *               2018-2020 Felix Salfelder
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
 * output
 */
#include "u_out.h"
#include "u_prblst.h"
/*--------------------------------------------------------------------------*/
OUTPUT& OUTPUT::setup(std::string const& reason)
{ untested();
  _prb = &PROBE_LISTS::get(reason);
  return *this;
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
/*--------------------------------------------------------------------------*/
