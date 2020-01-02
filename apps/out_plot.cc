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
 * plot command
 * set up ascii plot (select points, maintain probe lists)
 * command line operations
 */
#include "u_sim_data.h"
#include "c_comand.h"
#include "u_prblst.h"
#include "globals.h"
#include "declare.h" // plopen
#include "u_out.h"
#include "s__.h"
/*--------------------------------------------------------------------------*/
OMSTREAM plotout; // plot.cc
bool plotset; // plot.cc
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
	void	plottr(double,const PROBELIST&);
	int	plopen(double,double,const PROBELIST&);
	void	plclose(void);
	//void	plclear(void);
static	void	plborder(void);
static	void	calibrate(const PROBE_BASE&);
static	int	round_to_int(double);
static	void	plhead(const PROBELIST&);
static	int	point(double,double,double,int,int,int);
static	void	plotarg(double,double,double,double,double,double,
			 double,double,double);
/*--------------------------------------------------------------------------*/
#define MAXWIDTH 512
#define OUTWIDTH (std::min(static_cast<int>(OPT::outwidth), MAXWIDTH))
#define INDENT 8                    		/* beware OMSTERAM::form!   */
#define CONSSCALE (OUTWIDTH - INDENT - 2)	/*console scale size in chr */
static bool active;                 /* flag: plotting has opened            */
static double xstart, xstop;
static char border[MAXWIDTH+1];     /* border string (keep, repeat at end)  */
static char emptydata[MAXWIDTH+1];  /* empty data, to copy then insert data */
/*--------------------------------------------------------------------------*/
void plottr(double xx, const PROBELIST& plotlist) /* plot a data point,	    */
{
  if (active) {
    int ii = 0;
    double lo[2] = {0.};
    double hi[2] = {0.};
    double val[2] = {0.};
    
    for (PROBELIST::const_iterator
	 i =  plotlist.begin();
	 i != plotlist.end();
	 ++i) {
      val[ii] = (*i)->value();
      RANGE_PROBE const* P=dynamic_cast<RANGE_PROBE const*>(*i);
      if(!P){
	// user did not provide bounds, use defaults
	lo[ii] = -5.;
	hi[ii] = 5.;
      }else{
	if (P->range() != 0.) {
	  lo[ii] = P->lo();
	  hi[ii] = P->hi();
	}else{
	  lo[ii] = -5.;
	  hi[ii] = 5.;
	}
      }
      ++ii;
      if (ii >= 2) {
	break;
      }
    }
    if (ii <= 1) {
      val[1] = NOT_VALID;
    }
    plotarg(xx, val[0], val[1],
	    xstart, lo[0], lo[1],
	    xstop,  hi[0], hi[1]);
  }
}
/*--------------------------------------------------------------------------*/
/* plopen: begin the plot.  any type
 */
int plopen(double start, double stop, const PROBELIST& plotlist)
{
  if (start == stop) {
    plotout = OMSTREAM();
  }
#if 0
  if (!IO::plotout.any()) {untested();
    plclear();
    return false;
  }else{
  }
#endif
  xstart  = start;
  xstop   = stop;
  plhead(plotlist);
  assert(active == false);
  active = true;
  return true;
}
/*--------------------------------------------------------------------------*/
/* plclose: finish up the plot (any type)
 */
void plclose(void)
{
  if (!active) {
    return;
  }
  plborder();
  active = false;
  plotout = OMSTREAM();
}
/*--------------------------------------------------------------------------*/
#if 0
/* plclear: clear graphics mode
 */
void plclear(void)
{
  if (active) {
    untested();
  }
  active = false;
}
#endif
/*--------------------------------------------------------------------------*/
/* plborder: draw the border -- Ascii graphics
 */
static void plborder(void)
{
  plotout.tab(INDENT) << border << '\n';
}
/*--------------------------------------------------------------------------*/
/* calibrate: calibrate the y axis.  ascii plot.
 */
static void calibrate(PROBE_BASE const& prb)
{
  static char nums[20];		/* this label string        */
  static char highs[20];	/* the last label string    */
  int cal;			/* char position within line                */
  int stop;			/* location of last label, stop printing    */
  int filled;			/* how far (characters) have been printed   */
  int numsize;			/* number of characters in this label       */
  int start;			/* starting position of this label          */
  double markno;		/* loop counter                             */
 
  double hi, lo;
  if(RANGE_PROBE const* A=dynamic_cast<RANGE_PROBE const*>(&prb)){
    if (A->range() == 0) {
      hi = 5;
      lo = -5;
    }else{
      hi = A->hi();
      lo = A->lo();
    }
  }else{
    hi = 5;
    lo = -5;
  }
  double range = hi - lo;
  
  strcpy(highs, ftos(hi, 0, 5, IO::formaat));
  highs[8] = '\0';					  /* trim to 8 chrs */
  /* *strchr(&highs[2],' ') = '\0'; */	    /* make the top label, and save */
  stop = OUTWIDTH - static_cast<int>(strlen(highs)) - 1; /* space for it. */
  
  plotout << prb.label();
  range = hi - lo;
  filled = 0;
  for (markno = 0.;  markno < OPT::ydivisions;  markno++) {
    double number = lo + range * markno/OPT::ydivisions ;
    if (std::abs(number) < std::abs(range)/(10.*CONSSCALE)) {
      number = 0.;
    }						/* label to put on this div. */
    strcpy(nums, ftos(number, 0, 5, IO::formaat));
    nums[8] = '\0';				/* trim to 8 chrs */
    numsize = static_cast<int>(strlen(nums));	/* center it over the mark */
    cal = round_to_int(INDENT + CONSSCALE * (markno/OPT::ydivisions));
    start = cal - (numsize+1)/2;
    if (start > filled  &&  start+numsize < stop) {
      plotout.tab(start) << nums;		 /* if it fits, print it */
      filled = start + numsize ;
    }else{untested();
    }
  }
  plotout.tab(stop) << highs << '\n';    /* print the last calibration */
}
/*--------------------------------------------------------------------------*/
static int round_to_int(double x)
{
  return static_cast<int>(floor(x+.5));
}
/*--------------------------------------------------------------------------*/
/* plhead: begin ascii graphics
 * print opening border, calibrations, etc.
 */ 
static void plhead(const PROBELIST& plotlist)
{
  for (PROBELIST::const_iterator
       i =  plotlist.begin();
       i != plotlist.end();
       ++i) {
      calibrate(**i);
  }
  for (int ii = 0;  ii < CONSSCALE; ii++) {		/* build strings */
    border[ii] = '-';
    emptydata[ii] = ' ';
  }
  double incr = static_cast<double>(CONSSCALE) / OPT::ydivisions;
  for (double
       place = 0.;
       place < static_cast<double>(CONSSCALE);
       place += incr) {
    border[round_to_int(place)] = '+';
    emptydata[round_to_int(place)] = '.';		/* tics in emptydata */
  }
  border[CONSSCALE] = '+';			/* fix ends of the strings */
  border[CONSSCALE+1] = '\0';
  emptydata[CONSSCALE] = emptydata[0] = '|';
  emptydata[CONSSCALE+1] = '\0';
  
  plborder();					/* print the border */
}
/*--------------------------------------------------------------------------*/
/* point: return coordinate to plot in pixel #
 */
static int point(
	double yy,	/* raw data */
	double lo,
	double hi,	/* limits: both ends of the plot */
	int scale,	/* length of scale in pixels */
	int offset,	/* pixel offset of start of plot area */
	int linswp)	/* flag: linear scale (else log scale) */
{
  int place;
  
  if (linswp) {
    place = round_to_int( scale*(yy-lo)/(hi-lo));
  }else{untested();
    place = round_to_int( scale*(log(yy/lo))/(log(hi/lo)));
  }
  
  if (place < 0) {
    place = 0;
  }
  if (place > scale) {itested();
    place = scale;
  }
  return  place + offset;
}
/*--------------------------------------------------------------------------*/
/* plotarg: plot all 2 selected probes at one time, freq, etc. point.
 */
/*ARGSUSED*/
static void plotarg(
	double xx,  /* values */
	double yy,
	double zz,
	double , /* lower limits */
	double ylo,
	double zlo,
	double , /* upper limits */
	double yhi,
	double zhi)
{
  char adata[MAXWIDTH+1];     /* actual data. copy emptydata, insert */
  char *xxs;				     /* string representation of xx */
  memcpy(adata, emptydata, MAXWIDTH); /* copy prototype */
  xxs = ftos( xx, 11, 5, IO::formaat );
  if (zz != NOT_VALID) {
    adata[point(zz,zlo,zhi,CONSSCALE,0,1)] = '+';/* zap data into string */
  }
  adata[point(yy,ylo,yhi,CONSSCALE,0,1)] = '*';
  plotout.form( "%-8.8s%s", xxs, adata );
  plotout << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class OUTPUT_CMD_PLOT : public OUTPUT_CMD {
private: // types
  typedef RANGE_PROBE probe_type;
private:
  OUTPUT_CMD_PLOT(const OUTPUT_CMD_PLOT&p)
    : OUTPUT_CMD(p)
  {
  }
public:
  OUTPUT_CMD_PLOT() : OUTPUT_CMD() {
    set_label("plot");
  }
  virtual ~OUTPUT_CMD_PLOT(){
  }
  OUTPUT_CMD* clone() const{
    return new OUTPUT_CMD_PLOT(*this);
  }
private: // OUTPUT_CMD
  void setup(CS& cmd) {
    plotset = true;
    OUTPUT_CMD::setup(cmd);
  }
  virtual PROBE_BASE const* probe_proto() const{
    return &_probe_proto;
  }
  void init(){
    plotout = (plotset) ? IO::mstdout : OMSTREAM();
  }
private: // OUTPUT
  void head(std::string const& label){
    OUTPUT::head(label);
    PROBELIST const& pr=probelist();
    // if(0&& !plotout.any()){
    // }else
    double start=_sim->_axes[0]._min;
    double stop=_sim->_axes[0]._max;
    if(pr.size()){
      plopen(start, stop, pr);
    }
  }
  void commit(int flags){
    double x=coord(0); // can only plot univariate
    if(!(flags & ofPRINT)){
    }else if(probelist().size()){
      plottr(x, probelist());
    }else{ untested();
    }
  }
  void flush(){
    plclose();
  }
private:
  static probe_type _probe_proto;
}p2;
OUTPUT_CMD_PLOT::probe_type OUTPUT_CMD_PLOT::_probe_proto(PROBE_BASE::_STATIC);
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "iplot|plot", &p2);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
