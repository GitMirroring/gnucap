/*$Id: d_meter.cc,v 26.138 2013/04/24 02:44:30 al Exp $ -*- C++ -*-
 * Copyright (C) 2010 Albert Davis
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
 * 2-port "meter" device
 * does nothing to the circuit, but has probes
 */
#include "globals.h"
#include "e_elemnt.h"
#include "u_xprobe.h"
#include "u_opt.h"
#include "constant.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
static char fix_case(char c)
{
  return ((OPT::case_insensitive) ? (static_cast<char>(tolower(c))) : (c));
}
/*--------------------------------------------------------------------------*/
class DEV : public ELEMENT {
private:
  explicit DEV(const DEV& p) :ELEMENT(p) {}
public:
  explicit DEV()		:ELEMENT() {}
private: // override virtual
  char	   id_letter()const	{return '\0';}
  std::string value_name()const {return "";}
  std::string dev_type()const	{return "meter";}
  int	   max_nodes()const	{return 4;}
  int	   min_nodes()const	{return 4;}
  int	   matrix_nodes()const	{return 4;}
  int	   net_nodes()const	{return 4;}
  CARD*	   clone()const		{return new DEV(*this);}
  void	   tr_iwant_matrix()	{}
  void	   ac_iwant_matrix()	{}
  void     precalc_last();
  double   tr_involts()const	{return dn_diff(_n[IN1].v0(), _n[IN2].v0());}
  double   tr_involts_limited()const {return tr_involts();}
  COMPLEX  ac_involts()const	{return _n[IN1]->vac() - _n[IN2]->vac();}
  PROBE_BASE const*   new_probe(const std::string&)const;

  std::string port_name(int i)const {
    assert(i >= 0);
    assert(i < 4);
    static std::string names[] = {"outp", "outn", "inp", "inn"};
    return names[i];
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void DEV::precalc_last()
{
  ELEMENT::precalc_last();
  set_constant(true);
  set_converged();
}
/*--------------------------------------------------------------------------*/
class gain_probe : public PROBE_BASE{
public:
  explicit gain_probe(std::string const& what, DEV const* d)
      : PROBE_BASE(what, d)
  {
    // essentially CKT_BASE::ac_probe_num

    size_t length = what.length();
    _modifier = mtNONE;
    _dbscale = 0.;
    char parameter[BUFLEN+1];
    strcpy(parameter, what.c_str());

    if (length > 2  &&  Umatch(&parameter[length-2], "db ")) {
      _dbscale = 20.;
      length -= 2;
    }
    if (length > 1) { // selects modifier based on last letter of parameter
      switch (fix_case(parameter[length-1])) {
	case 'm': _modifier = mtMAG;   length--;	break;
	case 'p': _modifier = mtPHASE; length--;	break;
	case 'r': _modifier = mtREAL;  length--;	break;
	case 'i': _modifier = mtIMAG;  length--;	break;
	default:  _modifier = mtNONE;		break;
      }
    }
    parameter[length] = '\0'; // chop
  }

  double value() const{
    ELEMENT const* d=prechecked_cast<ELEMENT const*>(brh());
    if(brh()->_sim->analysis_is_ac()){ untested();
      return xvalue(d->ac_outvolts() / d->ac_involts());
    }else if(_modifier){ untested();
      // "ac mode"
      return NOT_VALID;
    }else{
      return d->tr_outvolts() / d->tr_involts();
    }
  }

private:
  double xvalue(COMPLEX c) const{
    XPROBE xp(c, _modifier? _modifier:mtMAG, _dbscale );
    return xp(_modifier, _dbscale);
  }

private:
  mod_t   _modifier; // default
  double  _dbscale;  // 20 for voltage, 10 for power, etc.
};
/*--------------------------------------------------------------------------*/
PROBE_BASE const* DEV::new_probe(const std::string& x) const
{
  if (Umatch(x, "gain")) {
    return new gain_probe(x, this);
  }else{
    return ELEMENT::new_probe(x);
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV p1;
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "meter", &p1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet
