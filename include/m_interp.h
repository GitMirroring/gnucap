/*$Id: m_interp.h,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
 * interpolation on a sorted array
 *
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
 */
//testing=script,sparse 2006.07.13
#ifndef M_INTERP_H
#define M_INTERP_H
#include "m_cpoly.h"
/*--------------------------------------------------------------------------*/
/* interpolate:  linear interpolation on a table.
 * Keys must be sorted in increasing order.
 */
template <class Iterator>
FPOLY1 interpolate(Iterator begin, Iterator end, double x,
		   double below, double above, int order=1)
{
  double f1 = NOT_VALID;
  double f0 = NOT_VALID;
  if (begin == end) { untested();
    throw Exception("interpolate table is empty");
  }
  --end;
  if (begin == end) { // only 1 entry -- constant
    f1 = (x < (*begin).first)
      ? ((below != NOT_INPUT) ? below : 0.)
      : ((above != NOT_INPUT) ? above : 0.);
    f0 = (*begin).second + (x - (*begin).first) * f1;
  }else{
    ++begin;

    // x is the search key
    // xx is the search key converted to a "pair" as required by upper_bound
    // upper might point to a match for the key, exact match
    // if not, it points just above it, no exact match
    // lower points just below where a match would go
    // so the real match is between upper and lower
    DPAIR    xx(x,BIGBIG);
    Iterator upper = upper_bound(begin, end, xx);
    Iterator lower = upper-1;
    Iterator lower1 = upper;

    if(order == 2 && lower1 != begin){
      --lower1;
      --lower1;
    }else{
      --lower1;
      order = 1;
    }

#if 0
      x2 = x1 = x0 = -1e99
      y2 = y1 = y0 = 0.
      for i in self._wave:
         x2, x1, x0 = x1, x0, i[0]
         y2, y1, y0 = y1, y0, i[1]
         if float(i[0]) > x:
            break;

#endif


    // set f1 (derivative)
    if ((upper == end) && (x > (*upper).first) && (above != NOT_INPUT)) {
      // x is out of bounds, above
      lower = upper;
      if (above != 0.) { untested();
	untested();
      }
      f1 = above;
    }else if ((upper == begin) && (x < (*lower).first) && (below!=NOT_INPUT)) { itested();
      // x is out of bounds, below
      if (below != 0.) { itested();
      }else{ itested();
      }
      f1 = below;
    }else if ((*upper).first <= (*lower).first) {itested();
      throw Exception("interpolate table is not sorted or has duplicate keys");
      f1 = 0.;
    }else if(order == 1) {
      // ordinary interpolation
      assert((*upper).first != (*lower).first);
      f1 = ((*upper).second-(*lower).second) / ((*upper).first-(*lower).first);
    }else if(order == 2) {
      incomplete();
    }

    if(order == 1){
      f0 = (*lower).second + (x - (*lower).first) * f1;
    }else if(order == 2){ untested();
      double x0 = upper->first;
      double x1 = lower->first;
      double x2 = lower1->first;
      double y0 = upper->second;
      double y1 = lower->second;
      double y2 = lower1->second;
      double dx = x - x1;
      double t0 = x0 - x1;
      double d = (y0 - y2) / (x0 - x2);
      double v0 = y0 - y1;
      double b = ( v0/t0 - d ) / t0;
      f0 = (d + b * dx) * dx + y1;
    }else{
    }
  }
  assert(f1 != NOT_VALID || order==2);
  assert(f0 != NOT_VALID);
  return FPOLY1(x, f0, f1);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
