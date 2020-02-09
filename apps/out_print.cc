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
//testing=script,complete 2020.01.15
#include "u_prblst.h"
#include "globals.h"
#include "u_out.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
// OUTPUT_CMD_PRINT: print the list of results (text form) to out()
// The argument is the first column (independent variable, aka "x")
class OUTPUT_CMD_PRINT : public OUTPUT_CMD {
private:
  int _threshold;
private:
  OUTPUT_CMD_PRINT(const OUTPUT_CMD_PRINT&p) : OUTPUT_CMD(p), _threshold(dl_NONE) {}
public:
  OUTPUT_CMD_PRINT() : OUTPUT_CMD()	{set_label("print");}
private: // OUTPUT_CMD
  OUTPUT_CMD* clone() const		{return new OUTPUT_CMD_PRINT(*this);}
  void setup(CS& cmd)			{IO::plotset = false; OUTPUT_CMD::setup(cmd);}

  ////BUG//// IO::plotset still sucks.

private: // OUTPUT
  void init(int Level, const std::string&)		{_threshold=Level;}

  void head(double, double, const std::string& col1)
  {
    trace1("print head", col1);
    if (IO::plotout.any()) {
      // plotting is active, suppress any other output
      ////BUG//// need a better way to do this.
    }else{
      int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
      char format[20];
      //sprintf(format, "%%c%%-%u.%us", width, width);
      sprintf(format, "%%c%%-%us", width);
      
      out().form(format, '#', col1.c_str());
      
      PROBELIST const& pr = probelist();
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
	out().form(format, ' ', p->label().c_str());
      }
      out() << '\n';
    }
  }

  void commit(double XX, int Level)
  {
    if (Level < _threshold) {
      // user specified, trace option
    }else if (IO::plotout.any()) {
      // plotting is active, suppress any other output
      ////BUG//// need a better way to do this.
    }else{
      OMSTREAM o=out();
      o.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
      o << XX;

      PROBELIST const& pr = probelist();
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
	o << p->value();
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
