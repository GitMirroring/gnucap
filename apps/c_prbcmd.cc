/*$Id: c_prbcmd.cc,v 26.137 2010/04/10 02:37:05 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2020 Felix Salfelder
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
 */
#include "c_comand.h"
#include "u_parameter.h"
#include "globals.h"
#include "u_prblst.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
////BUG//// doesn't work, crashes, due to problem in PROBE_LISTS
// Even if it did, it is out of place in this file, because it has no connection
// to OUTPUT_CMD.  It could exist as a stand-alone plugin.
/*--------------------------------------------------------------------------*/
class CMD_PROBES : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*){
    if (cmd.umatch("clear ")) {
      PROBE_LISTS::clear();
    }else if (cmd.umatch("list ")) {untested();
      incomplete();
    }else{untested();
      incomplete();
      // later
    }
  }
} p0;
/*--------------------------------------------------------------------------*/
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "probes", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
