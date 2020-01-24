/*                        -*- C++ -*-
 * Copyright (C) 2015-18 Felix Salfelder
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
 * alarm command
 * print a message when a probe is out of range
 */
//testing=script 2020.01.14
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD_ALARM : public OUTPUT_CMD {
private:
  typedef RANGE_PROBE probe_type;
  static probe_type _probe_proto;
private:
  OUTPUT_CMD_ALARM(OUTPUT_CMD_ALARM const&p) : OUTPUT_CMD(p) {}
public:
  OUTPUT_CMD_ALARM() : OUTPUT_CMD()	{set_label("alarm");}
  virtual ~OUTPUT_CMD_ALARM()		{}
public: // OUTPUT_CMD
  OUTPUT_CMD* clone() const		{return new OUTPUT_CMD_ALARM(*this);}
  PROBE_BASE const* probe_proto() const {return &_probe_proto;}
public: // OUTPUT
  void commit(double, int Level)
  {
    if (Level < dl_ACCEPTED) {
      // only look at dl_ACCEPTED or better.
    }else{
      OMSTREAM o=out();
      o.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
      for (PROBELIST::const_iterator p=probelist().begin(); p!=probelist().end(); ++p) {
	assert(*p);
	probe_type const* q = dynamic_cast<probe_type const*>(*p);
	if(!q) {untested();
	  // wrong kind of probe.  skip.
	}else if (!q->in_range()) {
	  o << (*p)->label() << "=" << (*p)->value() << '\n';
	}else{
	  // in range, no alarm
	}
      }
    }
  }
}p1;
OUTPUT_CMD_ALARM::probe_type OUTPUT_CMD_ALARM::_probe_proto(PROBE_BASE::_STATIC);
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "alarm", &p1);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
