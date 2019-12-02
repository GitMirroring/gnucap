/*$Id: e_subckt.cc                  -*- C++ -*-
 * Copyright (C) 2001 Albert Davis,
 *               2019 Felix Salfelder
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
 * Base class for subcircuits in the circuit description file
 */
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::new_subckt()
{
  assert(!_subckt);
  delete _subckt;
  _subckt = NULL;
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::new_subckt(const CARD* Model, PARAM_LIST* Params)
{ untested();
  delete _subckt;
  _subckt = NULL;
  _subckt = new CARD_LIST(Model, this, scope(), Params);
  _subckt->map_subckt_nodes(Model, this);
}
/*--------------------------------------------------------------------------*/
void BASE_SUBCKT::renew_subckt(const CARD* Model, PARAM_LIST* Params)
{
  if (_sim->is_first_expand()) {
    new_subckt(Model, Params);
  }else{untested();
    assert(subckt());
    subckt()->attach_params(Params, scope());
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST const* CARD::subckt() const
{ untested();
  unreachable(); // this is a compatibility hack
  BASE_SUBCKT const* s=dynamic_cast<BASE_SUBCKT const*>(this);
  if(s){ untested();
    return s->subckt();
  }else{ untested();
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::subckt()
{ untested();
  unreachable(); // this is a compatibility hack
  BASE_SUBCKT* s=dynamic_cast<BASE_SUBCKT*>(this);
  if(s){ untested();
    return s->subckt();
  }else{ untested();
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
