/*     -*- C++ -*-
 * Copyright (C) 2017 Felix Salfelder
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
 * attach tees to simulation commands
 */

#include "u_out.h"
#include "u_sim_data.h"
#include "globals.h"
#include "constant.h"
#include <stdio.h>
/*--------------------------------------------------------------------------*/
extern bool plotset;
namespace{
/*--------------------------------------------------------------------------*/
static const std::string sims_with_tee[] =
  { "tr", "dc", "op", "ac", "fourier" };
static const int number_sims = 5;
/*--------------------------------------------------------------------------*/
class INTERFACE OUTPUT_TEE : public OUTPUT {
public:
  typedef std::set<OUTPUT*> outputs_type;
private:
  OUTPUT_TEE(OUTPUT_TEE const&){ unreachable(); }
public: // construct
  OUTPUT_TEE(){}
  ~OUTPUT_TEE();
private:
  OUTPUT* attach_output(OUTPUT& o){
    _outputs.insert(&o);
    return this; // <= will be attached to parent.
  }
  void detach_output(OUTPUT& o){
    trace1("detach", _outputs.size());
    _outputs.erase(&o);
    trace1("detached", _outputs.size());
  }
  void init();
private: // override OUTPUT
  PROBELIST const* probes() const{
    if(_outputs.empty()){
      return NULL;
    }else{
      // incomplete. but not better in old code.
      return (*_outputs.begin())->probes();
    }
  }
  bool empty() const{
    return _outputs.empty();
  }
public: // OUTPUT. u_out.cc
  OUTPUT* set(CS& cmd);
  void commit(int);
  void h_ead(std::string const& label);
  void flush();
private:
  void do_it(CS&, CARD_LIST*) { unreachable(); }
private:
  outputs_type _outputs;
}; // OUTPUT_TEE
/*--------------------------------------------------------------------------*/
struct T{
  T(){
    for(unsigned i=0; i<number_sims; ++i){
      _p[i] = do_it(sims_with_tee[i], _t[i]);
    }
  }
  ~T(){
    for(unsigned i=0; i<number_sims; ++i){
      cleanup(_p[i], _t[i]);
    }
  }
  CMD* do_it(const std::string& s, OUTPUT_TEE& t){
    CMD* c=command_dispatcher[s];
    if(!c){ untested();
      // link order?
    }else if(SIM* cs=dynamic_cast<SIM*>(c)){
      cs->attach_output(t);
      return c;
    }else{
      // unlikely.
    }
    return NULL;
  }
  void cleanup(CMD* c, OUTPUT_TEE& t){
    if(!c){ untested();
      // link order? gone?
    }else if(SIM* cs=dynamic_cast<SIM*>(c)){
      cs->detach_output(t);
    }else{ untested();
      // unlikely.
    }
  }
private:
  OUTPUT_TEE _t[number_sims];
  CMD* _p[number_sims];
} tees;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::h_ead(std::string const&)
{
  OUTPUT::h_ead();
  if(empty()){
    // print something by default
    // this is to imitate pre-output behaviour
    int width=std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);
    std::string col1=_sim->_axes[0]._label;
    out().form(format, '#', col1.c_str());
    out() << '\n';
  }else{
    for(outputs_type::const_iterator p=_outputs.begin();
        p!=_outputs.end(); ++p){
      (*p)->h_ead();
    }
  }
}
/*--------------------------------------------------------------------------*/
OUTPUT_TEE::~OUTPUT_TEE()
{
  trace1("~tee", _outputs.size());
  assert(empty());
}
/*--------------------------------------------------------------------------*/
OUTPUT* OUTPUT_TEE::set(CS& cs)
{
  trace2("outset TEE", cs.tail(), _outputs.size());

  Get(cs, "pl{ot}", &plotset)
    || OUTPUT::set(cs); // parser hidden here.

  // propagate (necessary?)
  for(outputs_type::iterator p=_outputs.begin();
      p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->set(out());
  }
  return this;
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::init()
{
  for(outputs_type::iterator p=_outputs.begin();
      p!=_outputs.end(); ++p){
    assert(*p);
    (*p)->set(out()); // BUG? here?
    (*p)->init();
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::commit(int Flags)
{
  if(empty()){
    // legacy
    OMSTREAM o=out();
    o.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
    double x=coord(0);
    assert(x != NOT_VALID);
    o << x;
    o << '\n';
  }else{
    for(outputs_type::iterator p=_outputs.begin();
	p!=_outputs.end(); ++p){
      assert(*p);
      (*p)->commit(Flags);
    }
  }
}
/*--------------------------------------------------------------------------*/
void OUTPUT_TEE::flush()
{
  for(outputs_type::iterator p=_outputs.begin();
      p!=_outputs.end(); ++p){
    (*p)->flush();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
