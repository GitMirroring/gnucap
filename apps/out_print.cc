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
#include "m_phase.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/* find_max: find the max magnitude in [ Seek, End )
 */
static COMPLEX find_max(COMPLEX const *Seek, COMPLEX const *End)
{
  COMPLEX maxvalue = 0.;
  for (;Seek < End; ++Seek) {
    if (std::abs(*Seek) > std::abs(maxvalue)) {
      maxvalue = *Seek;
    }else{
    }
  }
  return maxvalue;
}
/*--------------------------------------------------------------------------*/
static double db(COMPLEX Value)
{
  return  20. * log10(std::max(std::abs(Value),VOLTMIN));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// OUTPUT_PRINT: print the list of results (text form) to out()
// The argument is the first column (independent variable, aka "x")
class OUTPUT_PRINT : public OUTPUT {
private:
  int _threshold;
private:
  OUTPUT_PRINT(const OUTPUT_PRINT&p) : OUTPUT(p), _threshold(dl_NONE) {}
public:
  OUTPUT_PRINT() : OUTPUT()	{set_label("print");}
private: // OUTPUT_CMD
  OUTPUT* clone() const		{return new OUTPUT_PRINT(*this);}

private: // OUTPUT
  void init(int Level, const std::string&)		{_threshold=Level;}

  void head(double, double, const std::string& col1)
  {
    trace1("print head", col1);
    if (IO::plotout.any()) { untested();
      // plotting is active, suppress any other output
      ////BUG//// need a better way to do this.
    }else{ untested();
      int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
      char format[20];
      //sprintf(format, "%%c%%-%u.%us", width, width);
      sprintf(format, "%%c%%-%us", width);
      
      PROBELIST const& pr = probelist();
      std::string newline;
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
	assert(*p);
	if ((*p)->is_complex_type()){
	  // not printing table header
	}else{
	  out().form(format, '#', col1.c_str());
	  newline = "\n";
	  break;
	}
      }

      _buffer.resize(0);
      _freq.resize(0);
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
	assert(*p);
	bool cplx = (*p)->is_complex_type();
	if(cplx){
	  _buffer.resize(_buffer.size()+1); // c++11 has emplace_back...
	}else{
	  out().form(format, ' ', (*p)->label().c_str());
	}
      }
      out() << newline;
    }
  }

  void commit(double XX, int Level) { untested();
    trace2("print:commit", Level, _threshold);
    if (Level < _threshold) { itested();
      // user specified, trace option
    }else if (IO::plotout.any()) { untested();
      // plotting is active, suppress any other output
      ////BUG//// need a better way to do this.
    }else{ untested();
      OMSTREAM o=out();
      o.setfloatwidth(OPT::numdgt, OPT::numdgt+6);
      PROBELIST const& pr = probelist();

      trace2("commit row", _freq.size(), pr.size());
      bool have_real_probes = _buffer.size()!=size_t(pr.size());
      if(have_real_probes){
	o << XX;
      }else{
      }

      int ii=0;
      for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
	assert(*p);
	bool cplx = (*p)->is_complex_type();
	// _sim->command_is_fourier(); ..?
	if(cplx){
	  // stash complex numbers until "flush"
	  if(!ii){
	    _freq.push_back(XX);
	  }else{
	  }
	  assert(ii<(int)_buffer.size());
	  trace2("got stream", ii, (*p)->value().operator COMPLEX());
	  _buffer[ii].push_back((*p)->value());
	  ++ii;
	}else{
	  o << (*p)->value();
	}
      }
      if(have_real_probes){
	o << '\n';
      }else{
	// all went into the buffer, no print
      }
    }
  }
  void flush(){
    PROBELIST const& pr = probelist();
    int ii=0;
    for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) {
      assert(*p);
      bool cplx = (*p)->is_complex_type();
      if(cplx){ untested();
	fohead(**p);
	foprint(_buffer[ii]);
	++ii;
      }else{untested();
      }
    }
  }
  /* fo_head: print fourier header
   * arg is index into probe array, to select probe name
   * this is the traditional fourier output. (for) now all complex data sets
   * are printed like this. (not reached from default plugins).
   */
  void fohead(const PROBE_BASE& Prob)
  { untested();
    out().form("# %-10s", Prob.label().c_str())
      << "--------- actual ---------  -------- relative --------\n"
      << "#freq       "
      << "value        dB      phase  value        dB      phase\n";
  }
  /* fo_print: print results of fourier analysis
   * for all points at single probe
   * (traditional, see fo_head)
   */
  void foprint(std::vector<COMPLEX> const& Data)
  { untested();
    int numsteps = (int) Data.size();
    trace1("foprint", numsteps);
    COMPLEX const* data = Data.data();
    COMPLEX maxvalue = find_max(data, data+numsteps);
    if (maxvalue == 0.) {
      maxvalue = 1.;
    }else{
    }
    for (int ii=0; ii<numsteps; ++ii) {
      double frequency = _freq[ii];
      COMPLEX unscaled = Data[ii];
      COMPLEX scaled = unscaled / maxvalue;
      unscaled *= 2;
      OMSTREAM o=out();
      o.form("%s%s%7.2f %8.3f %s%7.2f %8.3f\n",
	       ftos(frequency,    11,5,o.format()),
	  ftos(std::abs(unscaled),11,5,o.format()),
	       db(unscaled),
	       phase(unscaled*COMPLEX(0.,1)),
	  ftos(std::abs(scaled),  11,5,o.format()),
	       db(scaled),
	       phase(scaled) ) ;
    }
  }
private:
  // looks ugly, but does exactly what we need, reasonably efficient.
  std::vector<std::vector<COMPLEX> > _buffer;
  std::vector<double> _freq;
}o0;
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD_PRINT : public OUTPUT_CMD{
public:
  OUTPUT_CMD_PRINT(OUTPUT const* o) : OUTPUT_CMD(o) {
    set_label("print");
  }
private: ////BUG//// IO::plotset still sucks.
  void setup(CS& cmd) {IO::plotset = false; OUTPUT_CMD::setup(cmd);}
};
/*--------------------------------------------------------------------------*/
OUTPUT_CMD_PRINT p0(&o0);
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "iprint|print|probe", &p0);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
