/*                     -*- C++ -*-
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
 * std::multimap but case sensitivity may change
 */
/*--------------------------------------------------------------------------*/
#ifndef L_MULTIMAP_H
#define L_MULTIMAP_H
/*--------------------------------------------------------------------------*/
#include <set>

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
template<class INNER>
struct insensitive_compare{
  using is_transparent=std::true_type;
  bool operator()(INNER const& a, INNER const& b) const{
    assert(!a.empty());

    return (*this)(a.key(), b);
  }
  bool operator()(IString const& x, INNER const& b) const{
    return (*this)(&x, b);
  }
  bool operator()(IString const* x, INNER const& b) const{
    assert(x);
    assert(!b.empty());
    IString y(b.key());

    return x->compare(y, true) < 0;
  }
  bool operator()( INNER const& a, IString const* y) const{
    assert(y);
    assert(!a.empty());
    IString x(a.key());

    return x.compare(*y, true) < 0;
  }
  bool operator()(const INNER& a, const IString& y) const{
    return (*this)(a, &y);
  }
};
/*--------------------------------------------------------------------------*/
template<class V>
class MULTIMAP{
public:
private: // internal types
  struct card_compare{
    typedef V value_type;
    using is_transparent=std::true_type;
    bool operator()(V a, V b) const{
      assert(*a);
      IString x((*a)->short_label());

      return (*this)(x, b);
    }
    bool operator()(CARD const* a, CARD const* b) const{
      assert(a);
      IString x(a->short_label());

      return (*this)(x, b);
    }
    bool operator()(IString const& s, CARD const* b) const{
      assert(b);
      IString y(b->short_label());
      return s.compare(y /*, OPT::insensitive */) < 0;
    }
    bool operator()(CARD const* a, IString const& s) const{
      assert(a);
      IString x(a->short_label());
      return x.compare(s /*, OPT::insensitive */) < 0;
    }
    bool operator()( IString const& s, std::_List_iterator<CARD*> const& b) const{
      assert(*b);
      IString y((*b)->short_label());
      return s.compare(y /*, OPT::insensitive */) < 0;
    }
    bool operator()( std::_List_iterator<CARD*> const& a, const IString& y) const{
      assert(*a);
      IString x((*a)->short_label());
      return x.compare(y /*, OPT::insensitive */) < 0;

    }
  };
  class INNER{
  public: // types
    typedef std::multiset<V, card_compare> container_type;
    typedef typename container_type::iterator iterator;
    typedef typename container_type::const_iterator const_iterator;

  public: // construct
    INNER(V v){
      _m.insert(v);
    }

  public:
    IString const& key() const{
      assert(!_m.empty());
      CARD const* c=**_m.begin();
      return reinterpret_cast<IString const&>(c->short_label());
      // return *static_cast<IString const*>(&c->short_label());
    }
    bool empty() const{
     return _m.empty();
    }
    bool singleton() const{
      return _m.size()==1;
    }
    void erase(iterator i){
      assert(_m.size());
      _m.erase(i);
    }
    iterator begin(){
      return _m.begin();
    }
    const_iterator begin() const{
      return _m.begin();
    }
    iterator end(){
      return _m.end();
    }
    const_iterator end() const{
      return _m.end();
    }
    iterator find(IString const& k){
      return _m.find(k);
    }
    const_iterator find(IString const& k) const{
      return _m.find(k);
    }
    iterator insert(V v){
	_m.insert(v);
      return iterator();
    }

  private:
    container_type _m;
  }; // INNER
  typedef typename INNER::iterator inner_iterator;
  typedef typename INNER::const_iterator inner_const_iterator;

  typedef std::set<INNER, insensitive_compare<INNER> > OUTER;
  typedef typename OUTER::iterator outer_iterator;
  typedef typename OUTER::const_iterator outer_const_iterator;

public: // MULTIMAP iterators
  class iterator{
  public:
    explicit iterator(outer_iterator o)
      : _o(o){ itested(); }
    explicit iterator( inner_iterator i, outer_iterator o )
      : _i(i), _o(o){ itested(); }
  public:
    bool operator==(iterator const& x) const{
      // incomplete(); // end?
      return _o==x._o
	&& _i==x._i;
    }
    iterator& operator++(){
      incomplete();
      return *this;
    }
    V operator*(){
      return *_i;
    }
    V const& operator*() const{
      return *_i;
    }
    std::pair<bool, outer_iterator> erase(){
      assert(!_o->empty());

      if(_o->singleton()){
	return std::make_pair(true, _o);
      }else{
	INNER const& const_bag = *_o;
	INNER& bag = const_cast<INNER&>(const_bag); // yikes.

	bag.erase(_i);
	return std::make_pair(false, _o);
      }
    }

  private:
    inner_iterator _i, _e;
    outer_iterator _o;

  public:
    friend class const_iterator;
  };

  class const_iterator{
  public:
    explicit const_iterator(outer_const_iterator o)
      : _o(o){ }
    explicit const_iterator( inner_const_iterator i, outer_const_iterator o )
      : _i(i), _o(o){ }
    const_iterator( iterator i) : _i(i._i), _o(i._o){ untested(); }
  public: // compare
    bool operator==(const_iterator const& x) const{
      if(_o!=x._o){
	return false;
      }else{
	// if(_o != end)?
	return _i == x._i;
      }
    }
    bool operator!=(const_iterator const& x) const{
      return !((*this)==x);
    }
    V const& operator*() const{
      return *_i;
    }
  private:
    inner_const_iterator _i;
    outer_const_iterator _o;
  };

  const_iterator begin() const{
    outer_const_iterator o=_m.begin();
    if(o==_m.end()){
      return const_iterator(o);
    }else{
      return const_iterator(o->begin(), o);
    }
  }
  iterator end(){
    return iterator(_m.end());
  }
  const_iterator end() const{
    return const_iterator(_m.end());
  }

  const_iterator find(IString const& key) const{
    outer_const_iterator o=_m.find(&key);
    if(o==_m.end()){
      return end();
    }else{
      inner_const_iterator i=o->find(key);
      if(i==o->end()){
	return end();
      }else{
	return const_iterator(i, o);
      }
    }
  }
  iterator find(V v){
    IString a((*v)->short_label());
    return find(a);
  }
  iterator find(CARD const* v){
    IString a(v->short_label());
    return find(a);
  }
  iterator find(IString const& key){
    outer_iterator o=_m.find(&key);
    if(o==_m.end()){
      return end();
    }else{
      inner_iterator i=o->find(key);
      if(i==o->end()){
	return end();
      }else{
	return iterator(i, o);
      }
    }
  }
  iterator insert(V v){
    assert(*v);
    IString l((*v)->short_label());

    outer_iterator o=_m.find(l);
    if(o==_m.end()){
      o=_m.emplace(INNER(v)).first;
      return iterator(o->begin(), o);
    }else{
      INNER const& const_bag = *o;
      INNER& bag = const_cast<INNER&>(const_bag); // yikes.

      inner_iterator i=bag.insert(v);
      return iterator(i, o);
    }
  }
  void erase(iterator i){
    auto x=i.erase();
    if(x.first){
      // it is a singleton. get rid of it.
      _m.erase(x.second);
    }else{
    }
  }

private:
  // typedef std::set<INNER, insensitive_compare<INNER> > OUTER;
  OUTER _m;
}; // MULTIMAP
#endif
// vim:ts=8:sw=2:noet:
