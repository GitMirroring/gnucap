/*$Id: s__.h 2016/09/22 $ -*- C++ -*-
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
 * base class for simulation methods
 */
//testing=script,complete 2006.07.14
#ifndef S___H
#define S___H
#include "u_opt.h"
#include "c_comand.h"
/*--------------------------------------------------------------------------*/
class CARD;
class CARD_LIST;
class CS;
class PROBELIST;
class COMPONENT;
class WAVE;
class OUTPUT;
/*--------------------------------------------------------------------------*/
class SIM : public CMD {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
  enum TRACE { // how much diagnostics to show
    tNONE      = 0,	/* no extended diagnostics			*/
    tUNDER     = 1,	/* show underlying analysis, important pts only	*/
    tALLTIME   = 2,	/* show every time step, including hidden 	*/
    tREJECTED  = 3,	/* show rejected time steps			*/
    tITERATION = 4,	/* show every iteration, including nonconverged	*/
    tVERBOSE   = 5	/* show extended diagnostics			*/
  };
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  CARD_LIST* _scope;
private: // places to send the results
  OUTPUT* _output;
public:
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
private:
  const std::string long_label()const {unreachable(); return "";}
private:
  virtual void	setup(CS&)	= 0;
  virtual void	sweep()		= 0;
  virtual void	finish();
  virtual bool	is_step_rejected()const {return false;}

  explicit SIM(const SIM&)
    : CMD(),_scope(NULL), _output(NULL) {unreachable(); incomplete();}
protected:
  explicit SIM()
    : CMD(), _scope(NULL), _output(NULL) {
    }
public:
  ~SIM();
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:
  	 void	command_base(CS&);	/* s__init.cc */
	 void	reset_timers();	
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected: // OUTPUT interface, s__out.cc
  void outinit();
  bool outset(CS&);
  void outreset();
  void outcommit(int flags);
  void outhead();
  void outflush();
  PROBELIST const* outprobes() const; // hack
  // ...
protected: // obsolete wrappers.
  void outdata(double const&, int);
  void he_ad(double,double,const std::string&);
public:
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 
protected:				/* s__solve.cc */
  bool	solve(OPT::ITL,TRACE);
  bool	solve_with_homotopy(OPT::ITL,TRACE);
  void	advance_time();
private:
	void	finish_building_evalq();
	void	set_flags();
	void	clear_arrays();
	void	evaluate_models();
	void	set_damp();
	void	load_matrix();
	void	solve_equations();
public:
  OUTPUT* attach_output(OUTPUT&);
  void detach_output(OUTPUT&);
}; // SIM
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
