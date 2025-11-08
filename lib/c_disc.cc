/*                       -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 */
#include "u_lang.h"
#include "globals.h"
#include "c_comand.h"
#include "e_disc.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_DISCIPLINE : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    std::string label;
    cmd >> label;
    NODE* nn = new DISCIPLINE(label);

    Scope->push_back(nn);
    if(cmd >> ';'){
    }else{
      cmd.warn(bDANGER, "expecting ';'");
    }

    while(1){
      if(!cmd.more()){
	cmd.getline("discipline>");
      }else{
      }
      if(cmd >> "enddiscipline"){
	break;
      }else{
	std::string n, v;
	cmd >> n;
	cmd >> v;
	nn->set_param_by_name(n, v);
	if(cmd >> ';'){
	}else{
	  cmd.warn(bDANGER, "expecting ';'");
	}
      }
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "discipline", &p1);
/*--------------------------------------------------------------------------*/
class CMD_DEFAULT_DISC : public CMD {
  void do_it(CS& cmd, CARD_LIST*)override {
    cmd >> OPT::default_discipline;
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "`default_discipline", &p2);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
