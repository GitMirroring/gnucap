/*                     -*- C++ -*-
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
 */
#ifndef L_FIND_CACHE_H
#define L_FIND_CACHE_H
/*--------------------------------------------------------------------------*/
#include <e_card.h>
#include <set>
/*--------------------------------------------------------------------------*/
template<class CONTAINER>
class FIND_CACHE {
  struct compare {
    typedef void is_transparent;
    bool operator()(std::string const& a, std::string const& b)const {
      return a<b;
    }
    bool operator()(CARD const* a, std::string const& b)const {
      assert(a);
      return a->short_label()<b;
    }
    bool operator()(std::string const& a, CARD const* b)const {
      assert(b);
      return a < b->short_label();
    }
    bool operator()(CARD const* a, CARD const* b)const {
      assert(a);
      assert(b);
      return a->short_label() < b->short_label();
    }
  };
private:
  typedef typename CONTAINER::value_type value_type;
  typedef std::multiset<value_type const*, compare> cache;
  cache _cache;
  std::set<std::string> _miss;
  CONTAINER const& _c;
public:
  typedef typename cache::const_iterator const_iterator;
  FIND_CACHE(CONTAINER const& c) : _c(c) {}
  ~FIND_CACHE() {}

public:
  void add(CARD* c);
  const_iterator find(std::string const& key);
  const_iterator find_again(const_iterator current);
  const_iterator end()const { return _cache.end(); }
};
/*--------------------------------------------------------------------------*/
template<class C>
void FIND_CACHE<C>::add(CARD* c)
{
  std::string key = c->short_label();
  if(_miss.erase(key)){ untested();
    assert(_cache.find(key) == _cache.end());
  }else{
    const_iterator pos = _cache.lower_bound(key);
    if(pos == _cache.end()){
    }else if((*pos)->short_label() == key){
      pos = _cache.upper_bound(key);
     _cache.insert(pos, c); // insert before pos
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
template<class C>
typename FIND_CACHE<C>::const_iterator FIND_CACHE<C>::find(std::string const& key)
{
  if(_miss.count(key)){
    // tried before. it's not there.
    return _cache.end();
  }else{
    const_iterator pos = _cache.lower_bound(key);
    bool is_cached;
    if(pos == _cache.end()){
      is_cached = false;
    }else if((*pos)->short_label() == key){
      is_cached = true;
    }else{
      is_cached = false;
    }
    if(is_cached) {
      return pos;
    }else{
      // unknown key. fill cache as needed.
      CARD_LIST::const_reverse_iterator it = _c.find_reverse_(key);
      if(it == _c.rend()){
	_miss.insert(key);
	return end();
      }else{
	while(it != _c.rend()){
	  pos = _cache.insert(pos, *it); // insert before pos
	  it = _c.find_again(key, ++it);
	}

	return pos;
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
