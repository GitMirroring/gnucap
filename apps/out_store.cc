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
class OUTPUT_STORE : public OUTPUT {
private:
  std::vector<WAVE*> _wavep;
private:
  explicit OUTPUT_STORE(const OUTPUT_STORE&p) : OUTPUT(p), _fill(NULL) {}
public:
  OUTPUT_STORE() : OUTPUT(), _fill(NULL) {set_label("store");}
  virtual ~OUTPUT_STORE()		{ assert(!_fill); }
public: // OUTPUT
  OUTPUT* clone() const		{return new OUTPUT_STORE(*this);}
public: // OUTPUT
  void flush(){
    delete _fill;
    _fill = NULL;
  }
  void init(int, const std::string& Label) {
    WAVESTASH* data = data_dispatcher[Label];

    if(!data){
      // wrong type or not there, put new one
      data = new WAVESTASH;
      ////BUG//// memory leak

      /// really? data is intentionally global, so it can be seen by others,
      /// it's only one object per key, and then cleared during shutdown from
      /// "clear" command...

      data_dispatcher.install(Label, data);
    }else{
    }
    assert(data);

    PROBELIST const& pr = probelist();
    _fill = data->init(pr);
  }

  ////BUG//// It is possible when reusing "data" with a changed probelist
  // that deleted probes are still there, not initialized, so contain old data.
  // Not rebuilt, not seen, here but are still available to users of the stored data,
  // such as "measure".

  /// this was intentional, there is a clear command. could as well
  /// always clear when rebuilding the stash.

  // use of dispatcher (data_dispatcher) also seems inappropriate.

  /// where to put the data instead? what should own the data?

  void commit(double XX, int Level)
  {
    if (Level < dl_ACCEPTED) {
      // only look at dl_ACCEPTED or better.
    }else{
      PROBELIST const& pr = probelist();
      int ii=0;
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p){
	assert(*p);
	(*_fill)[ii].push(XX, (*p)->value());
	++ii;
      }
    }
  }
private:
  WAVESTASH::INDEX* _fill;
}o0; // OUTPUT_STORE
OUTPUT_CMD p0(&o0);
DISPATCHER<CMD>::INSTALL d0(&command_dispatcher, "store", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
