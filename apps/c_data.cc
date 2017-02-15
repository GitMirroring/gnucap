/*                                 -*- C++ -*-
 * Copyright (C) 2014-17 Felix Salfelder
 * Author: Felix Salfelder
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
 */
#include "c_comand.h"
#include "u_parameter.h"
#include "globals.h"
#include "m_wave.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_WAVE : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    std::string what;
    cmd >> what;

    if(what=="clear"){
      for(DISPATCHER<CARD*>::const_iterator i=data_dispatcher.begin();
          i!=data_dispatcher.end(); ++i){
	WAVESTASH* L=dynamic_cast<WAVESTASH*>(i->second);

	if(L){
	  L->clear();
	  delete L;
	  data_dispatcher.uninstall(i);
	}else if(i->second){ untested();
	  unreachable();
	}else{
	}
      }
    }else{ untested();
      // lot more stuff in -uf...
      incomplete();
    }

  }
} p5;
/*--------------------------------------------------------------------------*/
DISPATCHER<CMD>::INSTALL d5(&command_dispatcher, "wave|data", &p5);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
