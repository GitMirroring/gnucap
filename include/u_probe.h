/*$Id: u_probe.h 2015/01/21 al $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
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
 * single probe item
 */
//testing=script,complete 2006.07.14
#ifndef U_PROBE_H
#define U_PROBE_H
#include "io_trace.h"
#include "e_base.h"
#include "l_compar.h" // inorder
#include "l_lib.h" // wmatch
#include "constant.h" // NOT_VALID
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class INTERFACE PROBE_BASE : public CKT_BASE {
public:
  enum STATIC {_STATIC};
public:
  explicit PROBE_BASE(STATIC)
    : _what("proto"),
      _brh(NULL)
  {
  }

protected:
  explicit PROBE_BASE() {unreachable(); incomplete();}
  explicit PROBE_BASE(const std::string& What, CKT_BASE const* Brh)
    : _what(What),
      _brh(Brh)
  {
    trace2("construct baseprobe", _what, _brh);
    if (_brh) {
      _brh->inc_probes();
      set_label( _what + '(' + _brh->long_label() + ')');
    }else{ untested();
      unreachable();
    }
  }
  explicit PROBE_BASE(const PROBE_BASE& p)
    : CKT_BASE(p),
      _what(p._what),
      _brh(p._brh)
  { untested();
    if (_brh) { untested();
      _brh->inc_probes();
    }else{ untested();
    }
  }
public:
  virtual ~PROBE_BASE(){
    detach();
  }
  virtual PROBE_BASE* new_wrap(PROBE_BASE* n) const{ untested();
    return n;
  }
public:
  virtual int param_count() const{
    return 0;
  }
  virtual std::string param_value(int) const{ untested();
    unreachable();
    return "NA";
  }
  virtual void set_param_by_index(int, double);
//  virtual std::string label() const;
  virtual COMPLEX cvalue()const{unreachable(); return COMPLEX(NOT_VALID);}
  virtual double value()const = 0;
  std::string const& label() const{return short_label();}
public: // compare probes.
  bool operator==(const PROBE_BASE& p)const { untested();
    return ( ( _what == p._what )
           &&(  _brh == p._brh  ));
  }
public: // compare (for STL)
  bool operator==(const CKT_BASE& brh)const;
  bool operator!=(const CKT_BASE& b)const { untested();
    return (brh() != &b);
  }
  bool operator==(const std::string& par)const {
    return wmatch(label(), par);
  }
  bool operator!=(const std::string& par)const { untested();
    return !( *this == par);
  }
protected:
  void detach();
  std::string const& what()const{
    return _what;
  }
  CKT_BASE const* brh()const{
    return _brh;
  }

private:
  std::string _what;
  CKT_BASE const* _brh;
};
/*--------------------------------------------------------------------------*/
// used in out_plot and out_alarm
class RANGE_PROBE : public PROBE_BASE{
public:
  explicit RANGE_PROBE(STATIC x) : PROBE_BASE(x) {}
private:
  explicit RANGE_PROBE() : PROBE_BASE() {unreachable(); incomplete();}
  explicit RANGE_PROBE(RANGE_PROBE const&x);
public:
  explicit RANGE_PROBE(std::string const& what, PROBE_BASE const*brh);
  ~RANGE_PROBE(){
  }
private: // PROBE_BASE
  // static, actually. but then can not override
  PROBE_BASE* new_wrap(PROBE_BASE* n) const{
    RANGE_PROBE* x=new RANGE_PROBE(what(), n);
    return x;
  }
  int param_count() const{
    return 2;
  }
  void set_param_by_index(int i, double d);
  double value() const{
    if(PROBE_BASE const* p=prechecked_cast<PROBE_BASE const*>(brh())){
      return p->value();
    }else{ unreachable();
      // range probes cannot be attached to components.
      // PROBELIST->RANGE_PROBE->some_PROBE->COMPONENT
      // where some_PROBE is determined by COMPONENT
      return 99;
    }
  }
  virtual std::string param_value(int i) const{
    switch(i){
      case 0:
	return to_string(_lo);
      case 1:
	return to_string(_hi);
      default: untested();
        return "NA";
    }
  }
public:
  double range()const {
    return hi() - lo();
  }
  double lo()const {
    return _lo;
  }
  double hi()const {
    return _hi;
  }
  bool in_range()const{
    return in_order(lo(), value(), hi());
  }

private:
  double _lo, _hi;
};
/*--------------------------------------------------------------------------*/
// currently used in GROUND_NODE. useful everywhere.
// T needs to be convertible to double.
template<class T>
class PTR_PROBE : public PROBE_BASE{
private:
  explicit PTR_PROBE(PTR_PROBE const& x) : PROBE_BASE(x) {}
public:
  explicit PTR_PROBE(std::string const& What, CKT_BASE const* Brh, T const* Value)
  : PROBE_BASE(What, Brh), _value(Value){
    assert(Value);
  }
public:
  double value() const{
    assert(_value);
    return double(*_value);
  }
private:
  T const* _value;
}; // PTR_PROBE
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
