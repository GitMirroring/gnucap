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
#ifndef U_OUT_H
#define U_OUT_H
#include "s__.h"
// #include "u_prblst.h"
#include <set>
/*--------------------------------------------------------------------------*/
class PROBELIST;
/*--------------------------------------------------------------------------*/
// attached to SIM, can store probelist, do whatever output.
class INTERFACE OUTPUT : public CMD {
public:
  enum OUTFLAGS { // bit fields
    ofNONE  = 0,
    ofPRINT = 1,
    ofSTORE = 2,
    ofKEEP  = 4,
    ofTRACE = 8
  };
public: // construct
  OUTPUT(){
  }
  virtual ~OUTPUT(){
    assert(empty());
  }
public: // CMD
  OUTPUT* attach_output(OUTPUT& o){
    return &o;
  }
public:
  virtual PROBELIST const* probes() const { untested();
    return NULL;
  }
  virtual void reset(){
    _out = IO::mstdout;
    _out.reset(); // bug? check if needed
  }
  virtual void init();
  virtual bool empty() const{
    return true;
  }
  virtual OUTPUT* set(CS& cmd){
    ::outset(cmd, &_out);
    return this;
  }
  void set(OMSTREAM const& o){
    _out = o;
  }
  // print column headings and draw plot borders
  virtual void head(std::string const& label="");
  // trigger data collection
  virtual void commit(int flags)=0;
  // eject data
  virtual void flush() { }
  static void purge(CKT_BASE*);
public:
  virtual int flags() const{ untested();
    unreachable();
    return -1;
  }
protected:
  double coord(unsigned i) const;
  OMSTREAM out(){ return _out; }
private:
  OMSTREAM _out;
public:
  friend class SIM;
}; // OUTPUT
/*--------------------------------------------------------------------------*/
class INTERFACE OUTPUT_CMD : public OUTPUT {
protected: // types
  typedef std::map<CMD*, OUTPUT*> container_type;
  typedef PROBE_BASE probe_type;
public:
  OUTPUT_CMD()
    : OUTPUT(), _prb(NULL)
  {
  }
protected:
  OUTPUT_CMD(OUTPUT_CMD const& p)
    : OUTPUT(p), _prb(p._prb)
  {
  }
private:
  static PROBELIST& prblist(std::string const& reason);
protected:
  void setup_probelist(std::string const& reason){
    _prb = &prblist(reason);
  }
  virtual ~OUTPUT_CMD() {
    detach_sinks();
    for(container_type::const_iterator i=_sinks.begin();
        i!=_sinks.end(); ++i){
      assert(!i->second);
    }
  }

  virtual OUTPUT* clone() const=0;
protected:
  PROBELIST const& probelist() const {
    assert(_prb);
    return *_prb;
  }
  PROBELIST& probelist() {
    assert(_prb);
    return *_prb;
  }
  virtual void setup(CS&);
public:
  std::string const& simname() const {
    return _simname;
  }
  void set_simname(const std::string& s){
    _simname = s;
  }
public:
  void do_it(CS&, CARD_LIST*);
  virtual PROBE_BASE const* probe_proto() const{
    return NULL;
  }
private: // OUTPUT
  PROBELIST const* probes() const{
    return _prb;
  }
  bool empty() const{
    return false;
  }
  void detach_sinks();
private:
  std::string _simname; // required for listing probes..
  PROBELIST* _prb;
protected:
  container_type _sinks;
}; // OUTPUT_CMD
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void attach_output(OUTPUT& output, SIM& to); // apps/u_out.cc
void detach_output(OUTPUT& output, SIM& to); // apps/u_out.cc
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
