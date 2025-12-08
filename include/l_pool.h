/*$Id: l_cache.h $ -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 * pool objects with ordering
 */
#ifndef L_POOL_H
#define L_POOL_H
#include "md.h"
/*--------------------------------------------------------------------------*/
template <class T>
class POOL {
private:
  std::set<T*> _cache;
  POOL(const POOL&) = delete;
public:
  POOL() : _cache() {}
  ~POOL() {assert(!_cache.size());}

  T* operator[](T* x) { untested();
    auto p = _cache.insert(x);
    if(p.second) { untested();
      return x;
    }else{ untested();
      delete x;
      return *p.first;
    }
  }
  size_t unlink(T* x) {
    return _cache.erase(x);
  }
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
