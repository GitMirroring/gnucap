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
#include "m_wave.h"
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD_STORE : public OUTPUT_CMD {
private:
  std::vector<WAVE*> _wavep;
private:
  explicit OUTPUT_CMD_STORE(const OUTPUT_CMD_STORE&p) : OUTPUT_CMD(p) {}
public:
  OUTPUT_CMD_STORE() : OUTPUT_CMD()	{set_label("store");}
  virtual ~OUTPUT_CMD_STORE()		{}
public: // OUTPUT_CMD
  OUTPUT_CMD* clone() const		{return new OUTPUT_CMD_STORE(*this);}
public: // OUTPUT
  void init(int, const std::string& Label)
  {
    WAVESTASH* data = data_dispatcher[Label];

    if(!data){
      // wrong type or not there, put new one
      data = new WAVESTASH;
      ////BUG//// memory leak
      data_dispatcher.install(Label, data);
    }else{
    }
    assert(data);

    _wavep.resize(0);

    PROBELIST const& pr = probelist();
    for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
      assert(*p);
      trace1("--", (*p)->label());
      WAVE& w = (*data)[(*p)->label()]; // allocate or find
      w.initialize(); // needed if reusing
      _wavep.push_back(&w); // build index
    }
    assert(int(_wavep.size()) == pr.size());
  }

  ////BUG//// It is possible when reusing "data" with a changed probelist
  // that deleted probes are still there, not initialized, so contain old data.
  // Not rebuilt, not seen, here but are still available to users of the stored data,
  // such as "measure".

  ////BUG//// This indexing should be part of WAVESTASH, not done here.
  // Code here is more complex than old code that uses a C style array.
  // use of dispatcher (data_dispatcher) also seems inappropriate.

  void commit(double XX, int Level)
  {
    if (Level < dl_ACCEPTED) {
      // only look at dl_ACCEPTED or better.
    }else{
      trace1("store out", probelist().size());
      std::vector<WAVE*>::iterator ii=_wavep.begin();
      PROBELIST const& pr=probelist();
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p){
	assert(*p);
	trace2("commit", XX,  (*p)->value());
	(*ii)->push(XX, (*p)->value());
	++ii;
      }
      assert(ii==_wavep.end());
    }
  }

}p0; // OUTPUT_CMD_STORE
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "store", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
