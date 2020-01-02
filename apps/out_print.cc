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
 * probe command
 * set up print (select points, maintain probe lists)
 * command line operations
 */
#include "u_sim_data.h"
#include "c_comand.h"
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
extern bool plotset;
extern OMSTREAM plotout;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
// OUTPUT_CMD_PRINT: print the list of results (text form) to out()
// The argument is the first column (independent variable, aka "x")
class OUTPUT_CMD_PRINT : public OUTPUT_CMD {
private:
  OUTPUT_CMD_PRINT(const OUTPUT_CMD_PRINT&p)
    : OUTPUT_CMD(p)
  {
  }
public:
  OUTPUT_CMD_PRINT() : OUTPUT_CMD() {
    set_label("print");
  }
private: // OUTPUT_CMD
  OUTPUT_CMD* clone() const{
    return new OUTPUT_CMD_PRINT(*this);
  }
  void setup(CS& cmd) {
    plotset = false;
    OUTPUT_CMD::setup(cmd);
  }
private: // OUTPUT
  void head(std::string const& s){
    OUTPUT::head(s);
    PROBELIST const& pr=probelist();
    assert(_sim->_axes.size());
    normal_head(_sim->_axes[0]._label, pr);
  }
  void normal_head(const std::string& col1, PROBELIST const& pr)
  {
    trace1("print head", col1);

    if(CKT_BASE::_sim->_mode==s_DC){
    }else if(CKT_BASE::_sim->_mode==s_OP){
      // print anyway
    }else if(!pr.size()){
      // nothing to do.
      return;
    }else if (plotout.any()) {
      // plotting is active, suppress any other output
      return;
    }else{
    }

    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);

    out().form(format, '#', col1.c_str());

    for (PROBELIST::const_iterator
	p=pr.begin(); p!=pr.end(); ++p) {
      out().form(format, ' ', (*p)->label().c_str());
    }
    out() << '\n';
  }
  // OUTPUT_CMD_PRINT::
  void commit(int flags)
  {
    double x = coord(0);
    PROBELIST const& pr=probelist();
    trace2("print outdata", pr.size(), flags);

    if(!(flags & ( ofPRINT | ofTRACE ))){
    }else if (plotout.any() /*&& plt.has_probes()*/
      && !(CKT_BASE::_sim->_mode==s_OP)){
      // this is a hack from s_ac..
    }else{
      OMSTREAM o=out();
      o.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
      assert(x != NOT_VALID);
      if(flags & ofTRACE){
	o << -static_cast<double>(_sim->iteration_number());
      }else{
	o << x;
      }
      for (PROBELIST::const_iterator
	    p=pr.begin(); p!=pr.end(); ++p) {
	o << (*p)->value();
      }
      o << '\n';
    }
  }
};
OUTPUT_CMD_PRINT p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "iprint|print|probe", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
