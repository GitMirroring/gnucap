/*                            -*- C++ -*-
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
 */
//testing=script 2020.01.11
#ifndef U_OUT_H
#define U_OUT_H
#include "io_.h"
#include "c_comand.h"
#include <set>
/*--------------------------------------------------------------------------*/
class PROBELIST;
class SIM;
/*--------------------------------------------------------------------------*/
// attached to SIM, can store probelist, do whatever output.
class INTERFACE OUTPUT : public CKT_BASE {
public: // construct
  OUTPUT(): _prb(NULL)			{}
  virtual ~OUTPUT()			{}
protected:
  OUTPUT(OUTPUT const& p) : CKT_BASE(p), _prb(p._prb)  {}
public:
  virtual OUTPUT* clone()const = 0;
  OUTPUT& setup(std::string const& reason);
private:
public: // friend OUTPUT_CMD?
  static PROBELIST& prblist(std::string const& reason, CMD const* sim);
  PROBELIST const& probelist() const	{assert(_prb); return *_prb;}
  PROBELIST&	   probelist()		{assert(_prb); return *_prb;}
public:
  virtual void reset()			{_out = IO::mstdout; _out.reset();} // bug? check if needed
  virtual OUTPUT* set(CS& cmd)		{::outset(cmd, &_out); return this;}
  void set(OMSTREAM const& o)		{_out = o;}

  static void attach(OUTPUT* o, OUTPUT*& to){
    if(to){ untested();
      to->attach_output(o);
    }else{untested();
      to = o;
    }
  }

  static void detach(OUTPUT* o, OUTPUT*& from){
    if(from){ untested();
      from->detach_output(o);
    }else{untested();
      from = NULL;
    }
  }
  virtual void attach_output(OUTPUT*)		{unreachable();}
  virtual void detach_output(OUTPUT*)		{unreachable();}

  virtual void init(int, const std::string&)		{}
  virtual void head(double, double, const std::string&)	{}
  virtual void commit(double X, int Level)		=0;
  virtual void flush()					{}
  static  void purge(CKT_BASE*);
public:
  std::string const& simname() const	{return _simname;}
  void set_simname(const std::string& s){_simname = s;}
protected:
  OMSTREAM out()			{return _out;}
private:
  OMSTREAM _out;
  std::string _simname;
protected:
  PROBELIST* _prb;
}; // OUTPUT
/*--------------------------------------------------------------------------*/
class INTERFACE OUTPUT_TEE : public OUTPUT {
public:
  typedef std::set<OUTPUT*> outputs_type;
private:
  OUTPUT_TEE(OUTPUT_TEE const&) : OUTPUT() {unreachable();}
  OUTPUT* clone()const {unreachable(); return NULL;}
public: // construct
  OUTPUT_TEE()				{}
  ~OUTPUT_TEE();
private:
  void attach_output(OUTPUT* o);
  void detach_output(OUTPUT* o);
public: // OUTPUT. u_out.cc
  OUTPUT* set(CS& cmd);
  void init(int, const std::string&);
  void head(double, double, std::string const& label);
  void commit(double X, int Level);
  void flush();
private:
  void do_it(CS&, CARD_LIST*) { unreachable(); }
  outputs_type _outputs;
}; // OUTPUT_TEE
/*--------------------------------------------------------------------------*/
class INTERFACE OUTPUT_CMD : public CMD {
protected: // types
  typedef std::map<CMD*, OUTPUT*> container_type;
public:
  OUTPUT_CMD(OUTPUT const* p, PROBE_BASE const* pp=NULL)
    : CMD(), _outproto(p), _probe_proto(pp) {}
private: // inhibited
  OUTPUT_CMD(OUTPUT_CMD const& p) : CMD(p) {}
public:
  virtual ~OUTPUT_CMD() {
    detach_sinks();
    for(container_type::const_iterator i=_sinks.begin();
        i!=_sinks.end(); ++i){
      assert(!i->second);
    }
  }
protected:
  void setup_probelist(std::string const& reason, CMD const* sim);
  virtual /*?*/ OUTPUT& new_output(std::string const& reason){
    assert(_outproto);
    OUTPUT* o = _outproto->clone();
    assert(o);
    return o->setup(reason);
  }
  virtual void setup(CS&);
public:
  void do_it(CS&, CARD_LIST*);
private:
  PROBE_BASE const* probe_proto() const{return _probe_proto;}
private: // OUTPUT
  void detach_sinks();
private:
  OUTPUT const* _outproto;
  PROBE_BASE const* _probe_proto;
protected:
  container_type _sinks;
protected:
  PROBELIST* _prb; // OUTPUT?
}; // OUTPUT_CMD
/*--------------------------------------------------------------------------*/
inline void OUTPUT_CMD::setup_probelist(std::string const& reason, CMD const* sim)
{
  _prb = &OUTPUT::prblist(reason, sim);
}
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
