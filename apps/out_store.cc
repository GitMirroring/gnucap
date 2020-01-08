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
#include "u_sim_data.h"
#include "c_comand.h"
#include "u_prblst.h"
#include "globals.h"
#include "m_wave.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD_STORE : public OUTPUT_CMD {
private:
  explicit OUTPUT_CMD_STORE(const OUTPUT_CMD_STORE&p)
    : OUTPUT_CMD(p)
  {
  }
public:
  virtual ~OUTPUT_CMD_STORE(){ }
  OUTPUT_CMD_STORE() : OUTPUT_CMD() {
    set_label("store");
  }
public: // OUTPUT_CMD
  OUTPUT_CMD* clone() const{
    return new OUTPUT_CMD_STORE(*this);
  }
public: // OUTPUT
  // allocate space for output data
  //--------------------------------
  void head(double start, double stop, const std::string& col1)
  {
    std::string label;
    if(col1==""){untested();
      // fallback to legacy spice
      label = _sim->label();
    }else{
      label = col1;
    }
    trace0(label);
    OUTPUT::head(start, stop, label);
    PROBELIST const& pr=probelist();
    CKT_BASE* data = data_dispatcher[_sim->label()];
    WAVESTASH* wl;
    if(WAVESTASH* w=dynamic_cast<WAVESTASH*>(data)){
      // already there.
      wl = w;
    }else{
      // wrong type or not there, put new one
      wl = new WAVESTASH;
      data_dispatcher.install(_sim->label(), wl);
    }

    _wavep.resize(0);

    for (PROBELIST::const_iterator
	p=pr.begin(); p!=pr.end(); ++p) {
      assert(wl);
      trace1("--", (*p)->label());
      WAVE& w = (*wl)[(*p)->label()];
      w.initialize();
      _wavep.push_back(&w);
    }
  }
  //--------------------------------
  void commit(double x, int Flags) {
    trace1("store out", probelist().size());
    std::vector<WAVE*>::iterator ii=_wavep.begin();
    if(Flags & ofSTORE) {
      for (PROBELIST::const_iterator p=probelist().begin();
           p!=probelist().end(); ++p){
	trace2("commit", x,  (*p)->value());
	(*ii)->push(x, (*p)->value());
	++ii;
      }
      assert(ii==_wavep.end());
    }else{
    }
  }
  //-------------------------------------
private:
  std::vector<WAVE*> _wavep;
}p0; // OUTPUT_CMD_STORE
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "store", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
