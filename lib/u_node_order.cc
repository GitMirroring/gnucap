/*                           -*- C++ -*-
 * Copyright (C) 2018 Felix Salfelder
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
 */
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#include "ap.h"
#include "globals.h"
#include "u_node_order.h"
#include "u_status.h"
/*--------------------------------------------------------------------------*/
bool Get(CS& cmd_, const std::string& key, ORDERING const** val)
{
  if (cmd_.umatch(key + " {=}")) {
    CMD* cmd = order_dispatcher[cmd_];
    if (ORDERING* n=dynamic_cast<ORDERING*>(cmd)) {
      *val = n;
    }else{
      std::string choices;
      for(DISPATCHER<CMD>::const_iterator
	  i = order_dispatcher.begin(); i!=order_dispatcher.end(); ++i) {
	if (!dynamic_cast<ORDERING const*>(i->second)) {
	}else if (i->second) {
	  choices += i->first + ' ';
	}else{untested();
	}
      }
      cmd_.warn(bWARNING, "need an order (" + choices + ")");
    }
    return true;
  }else{
    return false;
  }
}
/*--------------------------------------------------------------------------*/
OMSTREAM& operator<<(OMSTREAM& o, ORDERING const* x)
{
  if (x) {
    return (o << x->name());
  }else{untested();
    return (o << "none");
  }
}
/*--------------------------------------------------------------------------*/
void NODE_ORDER::reinit(unsigned total_nodes)
{ untested();
	if(_order){ untested();
		incomplete();
	}else{
	}

	if(!OPT::order){ itested();
		// default order is overriden in default_plugins.
		assert(order_dispatcher["default"]);
		ORDERING const* od=prechecked_cast<ORDERING*>(order_dispatcher["default"]);
		assert(od);
		OPT::order = od;
	}else{ untested();
	}

	_order = OPT::order->clone();
	assert(_order);
	::status.order.reset().start();
	_order->init(total_nodes, *this);
	::status.order.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
