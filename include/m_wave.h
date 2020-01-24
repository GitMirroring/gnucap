/*$Id: m_wave.h 2014/11/23$ -*- C++ -*-
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
 * "wave" class, for transmission lines and delays
 */
//testing=script 2006.07.13
#ifndef M_WAVE_H
#define M_WAVE_H
#include "l_denoise.h"
#include "m_interp.h"
#include "l_dispatcher.h"
/*--------------------------------------------------------------------------*/
class WAVE : public CKT_BASE {
private:
  std::deque<DPAIR> _w;
  double _delay;
public:
  WAVE* clone()const{return new WAVE(*this);}
  typedef std::deque<DPAIR>::iterator iterator;
  typedef std::deque<DPAIR>::const_iterator const_iterator;

  explicit WAVE(double d=0);
  explicit WAVE(const WAVE&);
	  ~WAVE() {}
  WAVE&	   set_delay(double d);
  WAVE&	   initialize();
  WAVE&	   push(double t, double v);
  FPOLY1   v_out(double t)const;
  double   v_reflect(double t, double v_total)const;
  WAVE&	   operator+=(const WAVE& x);
  WAVE&	   operator+=(double x);
  WAVE&	   operator*=(const WAVE& x);
  WAVE&	   operator*=(double x);
  const_iterator begin()const {return _w.begin();}
  const_iterator end()const {return _w.end();}
};
/*--------------------------------------------------------------------------*/
class WAVESTASH : public CKT_BASE{
public:
  typedef std::string key_type;
  typedef std::map<key_type, WAVE> container_type;
  typedef container_type::const_iterator const_iterator;
private:
  WAVESTASH(const WAVESTASH&x):CKT_BASE(x){ unreachable(); }
public:
  WAVESTASH() : CKT_BASE(), _container() {}
  ~WAVESTASH() {
  }
public:
  const_iterator find(const key_type& k) const{
    return _container.find(k);
  }
  const_iterator end() const{
    return _container.end();
  }
  void clear(){
    _container.clear();
  }
  WAVE& operator[](const std::string& s){
    return _container[s];
  }
//  WAVE const& operator[](const std::string& s) const{
//    return _container[s];
//  }
private:
  container_type _container;
};
////BUG//// too much external hacking needed.
// OUTPUT_CMD_STORE does too much internal manipulating
// WAVESTASH as presented has one advantage over old code .
// .....  faster search in postprocessing.
// but really, it's just a wrapper for map, like a typedef
// linear indexing, if used at all, needs to be part of WAVESTASH.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// push: insert a signal on the "input" end.
// args: t = the time now
//       v = the value to push
//
inline WAVE& WAVE::push(double t, double v)
{
  _w.push_back(DPAIR(t+_delay, v));
  return *this;
}
/*--------------------------------------------------------------------------*/
// initialize: remove all info, fill it with all 0.
//
inline WAVE& WAVE::initialize()
{
  _w.clear();
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE::WAVE(const WAVE& w)
  : CKT_BASE(w),
    _w(w._w),
    _delay(w._delay)
{
}
/*--------------------------------------------------------------------------*/
// constructor -- argument is the delay
//
inline WAVE::WAVE(double d)
  :_w(),
   _delay(d)
{
  initialize();
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::set_delay(double d) 
{
  _delay = d; 
  return *this;
}
/*--------------------------------------------------------------------------*/
// v_out: return the value at the "output" end
// args: t = the time now
//
inline FPOLY1 WAVE::v_out(double t)const
{
  return interpolate(_w.begin(), _w.end(), t, 0., 0.);
}
/*--------------------------------------------------------------------------*/
// reflect: calculate a reflection
// args: t = the time now
//       v_total = actual voltage across the termination
// returns: the value (voltage) to send back as the reflection
//
inline double WAVE::v_reflect(double t, double v_total)const
{
  // return (v_total*2 - v_out(t)); // de-noised
  return dn_diff(v_total*2, v_out(t).f0);
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator+=(const WAVE& x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second += x.v_out((*i).first).f0;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator+=(double x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second += x;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator*=(const WAVE& x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second *= x.v_out((*i).first).f0;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
inline WAVE& WAVE::operator*=(double x)
{
  untested();
  for (std::deque<DPAIR>::iterator
	 i = _w.begin(); i != _w.end(); ++i) {
    untested();
    (*i).second *= x;
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif

// vim:ts=8:sw=2:noet:
