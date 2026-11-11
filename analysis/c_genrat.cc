/*$Id: c_genrat.cc 2015.01.08 al $ -*- C++ -*-
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
 * set up generator for transient analysis
 */
//testing=script,complete 2015.01.08
//BUG// no step control
#include "u_sim_data.h"
#include "globals.h"
#include "c_comand.h"
#include "e_compon.h"
/*--------------------------------------------------------------------------*/
static std::string freq = "0.";
static std::string ampl = "1.";
static std::string phaz = "0.";
static std::string maxv = "1.";
static std::string minv = "0.";
static std::string offset = "0.";
static std::string init_ = "0.";
static std::string rise = "1e-12";
static std::string fall = "1e-12";
static std::string delay = "0.";
static std::string width = "0.";
static std::string period = "0.";
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_ : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)override {
    OMSTREAM where = (cmd.more())  ?  OMSTREAM()  :  IO::mstdout;
    size_t here = cmd.cursor();
    COMMON_COMPONENT* gen = bm_dispatcher["gen"];
    assert(gen);
    do{
      ONE_OF
	|| (::Get(cmd, "f{requency}",&freq)
	      && (gen->set_param_by_name("freq", freq), true))
	|| (::Get(cmd, "a{mplitude}",&ampl)
	      && (gen->set_param_by_name("ampl", ampl), true))
	|| (::Get(cmd, "p{hase}",    &phaz)
	      && (gen->set_param_by_name("phase", phaz), true))
	|| (::Get(cmd, "ma{x}",      &maxv)
	      && (gen->set_param_by_name("max", maxv), true))
	|| (::Get(cmd, "mi{n}",      &minv)
	      && (gen->set_param_by_name("min", minv), true))
	|| (::Get(cmd, "o{ffset}",   &offset)
	      && (gen->set_param_by_name("offset", offset), true))
	|| (::Get(cmd, "i{nitial}",  &init_)
	      && (gen->set_param_by_name("init", init_), true))
	|| (::Get(cmd, "r{ise}",     &rise)
	      && (gen->set_param_by_name("rise", rise), true))
	|| (::Get(cmd, "f{all}",     &fall)
	      && (gen->set_param_by_name("fall", fall), true))
	|| (::Get(cmd, "d{elay}",    &delay)
	      && (gen->set_param_by_name("delay", delay), true))
	|| (::Get(cmd, "w{idth}",    &width)
	      && (gen->set_param_by_name("width", width), true))
	|| (::Get(cmd, "pe{riod}",   &period)
	      && (gen->set_param_by_name("period", period), true))
	;
    }while (cmd.more() && !cmd.stuck(&here));
    cmd.check(bWARNING, "what's this");
    
    // where.setfloatwidth(7);
    where <<   "freq="	<< freq;
    where << "  ampl="	<< ampl;
    where << "  phase="	<< phaz;
    where << "  max="	<< maxv;
    where << "  min="	<< minv;
    where << "  offset="<< offset;
    where << "  init="	<< init_;
    where << "  rise="	<< rise;
    where << "  fall="	<< fall;
    where << "  delay="	<< delay;
    where << "  width="	<< width;
    where << "  period="<< period;
    where << "\n";
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "generator", &p);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
