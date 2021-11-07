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
 * store command
 * set up print and plot (select points, maintain probe lists)
 * command line operations
 */
//testing=script 2020.01.14
#include "u_sim_data.h"
#include "m_wave.h"
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class OUTPUT_STORE : public OUTPUT {
private:
private:
  explicit OUTPUT_STORE(const OUTPUT_STORE&p) : OUTPUT(p) {}
public:
  OUTPUT_STORE() : OUTPUT()	{set_label("store");}
  virtual ~OUTPUT_STORE()		{}
public: // OUTPUT
  OUTPUT* clone() const		{return new OUTPUT_STORE(*this);}
public: // OUTPUT
  void init(int, const std::string& /*Label*/)
  {
    if (_sim->_waves) {
      delete _sim->_waves;
      _sim->_waves = NULL;
    }else{
    }
    _sim->_waves = new WAVEstash(_prb);
  }
  void commit(double XX, int Level)
  {
    if (Level < dl_ACCEPTED) {
      // only look at dl_ACCEPTED or better.
    }else{
      trace1("store out", probelist().size());
      int ii = 0;
      PROBELIST const& pr=probelist();
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p){
	trace2("commit", XX,  p->value());
	_sim->_waves->_wav[ii++].push(XX, p->value());
      }
    }
  }

}o0; // OUTPUT_STORE
OUTPUT_CMD p0(&o0);
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "store", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
