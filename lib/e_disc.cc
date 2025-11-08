/*                -*- C++ -*-
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
 *------------------------------------------------------------------
 * discipline and nature
 */
#include "e_disc.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
DISCIPLINE::DISCIPLINE(std::string const& name, std::string p, std::string f)
  : NODE(name),
    _user_number(int(discipline_dispatcher.size())),
    _potential(p),
    _flow(f)
{
  trace3("new_disc", name, user_number(), discipline_dispatcher.size());
  assert(!_installer);
  _installer = new inst(&discipline_dispatcher, name, this);
}
/*--------------------------------------------------------------------------*/
DISCIPLINE::~DISCIPLINE()
{
  delete _installer;
  _installer = nullptr;
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class ELECTRICAL : public DISCIPLINE {
public:
  explicit ELECTRICAL() : DISCIPLINE("electrical", "Voltage", "Current") {
    set_continuous();
  }
}p1;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
