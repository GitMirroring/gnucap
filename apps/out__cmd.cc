/*                            -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2018 Felix Salfelder
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
 * output commands
 */
////BUG//// this file needs to move to lib.
// apps is a collection of plugins, all optional, all independent.
// This is an essential part of the system, not optional.
// This file is a library, used to build output plugins.
// apps sources are not usually installed, so this file
// would not be available for additional modules if left in apps.

/// same with s__out, perhaps merge out_cmd into s__out?
/// additional modules need to carry s__out anyway, but it can be adapted to
/// what is needed.

//testing=script 2020.01.16
#include "u_sim_data.h"
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
#include "trace_on.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
////BUG//// doesn't work, crashes, due to problem in PROBE_LISTS
// Even if it did, it is out of place in this file, because it has no connection
// to OUTPUT_CMD.  It could exist as a stand-alone plugin.

/// previously, hardwired probelists were cleared by
///   command("alarm clear", Scope);
///   command("plot clear", Scope);
///   command("print clear", Scope);
/// need global access to all PROBELISTs from here (or some stand-alone plugin).

//
class CMD_PROBES : public CMD{
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
} cp;
DISPATCHER<CMD>::INSTALL d4(&command_dispatcher, "probes", &cp);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
