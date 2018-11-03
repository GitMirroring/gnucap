/*                                  -*- C++ -*-
 * Copyright (C) 2018 Felix Salfelder
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
 * nodemapper
 */
#ifndef M_NODEMAP_H
#define M_NODEMAP_H

template<class T>
class BSMATRIX;
class SIM_DATA;
/*--------------------------------------------------------------------------*/
class NODEMAPPER{
public: // construct
  explicit NODEMAPPER(SIM_DATA const& s) : _sim(s)
  {
  }
public:
  void uninit(){
    _nm.resize(0);
  }
  void reinit(unsigned total_nodes){
    _nm.resize(total_nodes+1);

    for (unsigned node = 1; node<=total_nodes; ++node) {
      _nm[node] = unsigned(total_nodes - node) + 1;
    }
  }
  size_t size() const{
    return _nm.size()-1;
  }
  bool empty() const{
    return _nm.empty();
  }

public:
  unsigned const& operator[](int x) const{ return _nm[size_t(x)]; }
  unsigned const& operator[](unsigned x) const{ return _nm[x]; }

private:
  SIM_DATA const& _sim;
  std::vector<unsigned> _nm; // nm 0...total_nodes
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
