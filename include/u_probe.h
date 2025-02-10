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
#include "e_base.h"
#include "l_compar.h" // inorder
/*--------------------------------------------------------------------------*/
class CARD_LIST;
/*--------------------------------------------------------------------------*/
class INTERFACE PROBE : public CKT_BASE {
  std::string _label; // reallt?
private:
  std::string	_what;
  const CKT_BASE* _brh;
  double	_lo,_hi;
  explicit  PROBE() {unreachable(); incomplete();}
public:
  explicit  PROBE(const std::string& what, const CARD *brh);
	    PROBE(const PROBE& p);
	    ~PROBE()				{detach();}

  void	    set_limit(double Lo,double Hi)	{_lo = Lo; _hi = Hi;}
  void	    detach();
  void	    store();
  void restore(CARD_LIST const* scope);
  PROBE&    operator=(const PROBE& p);

public:	// label -- in CKT_BASE
  /*virtual*/ std::string long_label()const final{return _label;}
  std::string const& short_label()const final override {return _label;}
  void	set_label(const std::string& s)final {_label=s;}
  const std::string label()const;

  double	  value()const;
  CKT_BASE const* object()const	 {return _brh;}
  double	  lo()const	 {return _lo;}
  double	  hi()const	 {return _hi;}
  double	  range()const	 {return hi()-lo();}
  bool		  in_range()const{return in_order(lo(),value(),hi());}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
