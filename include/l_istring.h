/*                           -*- C++ -*-
 * Copyright (C) 2016-18 Felix Salfelder
 * Authors: Felix Salfelder <felix@salfelder.org>
 *          Albert Davis <aldavis@gnu.org>
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
 * Characters and Strings with optional case (in)sensitivity
 */

#ifndef L_ISTRING_H
#define L_ISTRING_H
/*--------------------------------------------------------------------------*/
#include "u_opt.h"
#include "l_stlextra.h"
/*--------------------------------------------------------------------------*/
struct Ichar_{
  Ichar_() : _c('\0') {}
  Ichar_(const Ichar_& c) : _c(c._c) {}
  explicit Ichar_(char c) : _c(c) {}
  bool operator==(char o) const {
    if(OPT::case_insensitive){
      return tolower(_c)==tolower(o);
    }else if(o==_c){
      return true;
    }else{
      return false;
    }
  }
  bool operator==(Ichar_ o) const {
    if(OPT::case_insensitive){
      return tolower(_c)==tolower(o._c);
    }else{
      return o._c == _c;
    }
  }
  bool operator!=(char o) const {
    return(!operator==(o));
  }
  bool operator!=(Ichar_ o) const {
    return(!operator==(o));
  }
  bool operator<(const Ichar_& o) const {
    return((!OPT::case_insensitive && tolower(_c)==tolower(o._c))
      ? _c<o._c : tolower(_c)<tolower(o._c));
  }
  bool operator>(const Ichar_& o) const {
    return((!OPT::case_insensitive && tolower(_c)==tolower(o._c))
      ? _c>o._c : tolower(_c)>tolower(o._c));
  }
  bool operator!() const{
    return !bool(_c);
  }
  char const& to_char() const{
    return _c;
  }
  char to_lower() const{
    return (char)tolower(_c);
  }
private:
  char _c;
};
/*--------------------------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& o, const Ichar_* s)
{untested();
  o << (char const*) s;
  return o;
}
/*--------------------------------------------------------------------------*/
namespace detail{
struct ichar_traits : std::char_traits<Ichar_>{
  typedef char_traits<char> base;
  typedef Ichar_ char_type;
  typedef int int_type;
  typedef std::streamoff off_type;
  typedef std::streampos pos_type;
  // inherited
  // static bool lt(const Ichar_& a, const Ichar_& b);
  // static bool eq(const Ichar_& a, const Ichar_& b);
  // static void copy(Ichar_* x, const Ichar_* y, size_t s);
  // static size_t length(const Ichar_* x);

  // compare needs to be different. default to insensitive order.
  // if enabled, use sensitive order as a tie break.
  // return +-1 if substrings compare to less or more.
  // zero means, they are equal
  // result multiplied by two if tie break is not required.
  //
  // e.g. compare("v", "V") returns 1, "v" is weakly greater than "V"
  static int compare (const char_type* i, const char_type* j, size_t n,
      bool insens=OPT::case_insensitive)
  {
    typedef enum{
      llt =-2,
      lt  =-1,
      same= 0,
      gt  = 1,
      ggt = 2
    }ord_t;

    ord_t try_ord = same;
    for (unsigned I=0; I<n; ++I) {
      assert (*i!='\0');
      assert (*j!='\0');

      if (i->to_char() == j->to_char()) {
	// sensitive match, move on
	++i;
	++j;
      }else if (i->to_char() == j->to_lower()) {
	// insensitive match, move on, but remember
	if (try_ord==same){ itested();
	  try_ord = gt;
	}else{ itested();
	  // don't touch. the left most difference decides
	}
	++i;
	++j;
      }else if (i->to_lower() == j->to_char()) {
	// insensitive match, move on, but remember
	if (try_ord==same){ itested();
	  try_ord = lt;
	}else{ itested();
	  // don't touch. the left most difference decides
	}
	++i;
	++j;
      }else if (*i < *j) {
	try_ord = llt;
	break;
      }else{
	assert (*i > *j);
	try_ord = ggt;
	break;
      }
    }

    // assert(try_ord==-1 || try_ord==0 || try_ord==1);
    if(!insens){
      return try_ord;
      // give the full answer.
    }else{
      // truncate, i.e. map +-1 to zero. leave +-2
      return (try_ord/2)*2;
    }
  }
}; // ichar_traits
} // detail
/*--------------------------------------------------------------------------*/
class IString_;
inline std::ostream& operator<< (std::ostream& o, IString_ const& s);
/*--------------------------------------------------------------------------*/
class IString_ : public std::basic_string<Ichar_, detail::ichar_traits> {
private:
  typedef std::basic_string<Ichar_, detail::ichar_traits> base;
public: // construct
  IString_() {}
  IString_(const IString_& s) : base(s) {}
  IString_(const base& s) : base(s) {}
  explicit IString_(const char* s) : base((const Ichar_*)s) {}
//  IString_(const char* s, size_type t) : base((const Ichar_*)s, t) {}
  explicit IString_(const std::string& s) :
    base((const Ichar_*)s.data(), s.size()) {}
public: // ops
  IString_& operator=(Ichar_ s){ untested();
    base::operator=(s);
    return *this;
  }
  IString_& operator=(const std::string& s){
    base::operator=(IString_(s));
    return *this;
  }
  IString_& operator=(const char* s){
    base::operator=((Ichar_ const*)s);
    return *this;
  }
  bool operator==(const IString_& s) const{
    return base(*this)==base(s);
  }
  IString_& operator+=(const std::string& s){
    base::operator+=(IString_(s));
    return *this;
  }
  bool operator==(char c) const {untested();
    return size()==1 && base::operator[](0).to_char()==c;
  }
  bool operator!=(char c) const {untested();
    return !(*this==c);
  }
  bool operator==(char const* c) const {
    return (*this==IString_(c));
  }
  bool operator!=(char const* c) const {
    return !(operator==(c));
  }
  std::string operator+(char x) const
  {
    return to_string() + x;
  }
  std::string operator+(const char* x) const
  {
    return to_string() + x;
  }
  std::string operator+(std::string x) const
  {
    return to_string() + x;
  }
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
public: // more conventional type bridge
  size_type find(char x, size_type y) const {
    return base::find(Ichar_(x), y);
  }
  size_type find(char x) const {
    return base::find(Ichar_(x));
  }
  size_type find_first_of(char const* x) const {
    return base::find_first_of((Ichar_ const*)x);
  }
public: // explicit conversion
  std::string const& to_string() const
  {
    return reinterpret_cast<std::string const&>(*this);
  }
private: // cleanup later.
public: // more compare logic
  int compare(const IString_& str, bool insens=OPT::case_insensitive) const { itested();
    const size_type tsize = this->size();
    const size_type osize = str.size();
    const size_type len = std::min(tsize, osize);

    int r=traits_type::compare(data(), str.data(), len, insens);

    trace1("strcmp", *this);
    trace3("strcmp", str, len, r);
    if (r == 2 || r == -2){
      // traits_type::compare is really sure
      return r;
    }else if(tsize < osize){
      return -2;
    }else if(tsize > osize){
      return 2;
    }else{
      assert(tsize == osize);
      assert(!insens || r==0);
      // same length, use tie break
      return r;
    }
  }
}; // IString_
/*--------------------------------------------------------------------------*/
inline bool operator<(const IString_& lhs, const IString_& rhs)
{ itested();
  return lhs.compare(rhs) < 0;
}
/*--------------------------------------------------------------------------*/
template<typename CharT>
inline bool operator<(const IString_ lhs, const CharT* rhs)
{ untested();
  return lhs.compare(rhs) < 0;
}
/*--------------------------------------------------------------------------*/
inline bool operator>(const IString_& lhs, const IString_& rhs)
{ untested();
  return lhs.compare(rhs) > 0;
}
/*--------------------------------------------------------------------------*/
template<typename CharT>
inline bool operator>(const IString_ lhs, const CharT* rhs)
{ untested();
  return lhs.compare(rhs) > 0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
inline std::string operator+(char x, IString_ s)
{
  return x + s.to_string();
}
/*--------------------------------------------------------------------------*/
inline std::string operator+(const char* x, IString_ s)
{
    return x + s.to_string();
}
/*--------------------------------------------------------------------------*/
inline std::string operator+(std::string x, IString_ s)
{
  return x + s.to_string();
}
/*--------------------------------------------------------------------------*/
inline std::ostream& operator<< (std::ostream& o, IString_ const& s)
{
  o << s.to_string();
  return o;
}
/*--------------------------------------------------------------------------*/
inline OMSTREAM& operator<< (OMSTREAM& o, IString_ s)
{
  o << s.to_string();
  return o;
}
/*--------------------------------------------------------------------------*/
inline bool Umatch(const IString_&s, const std::string&t)
{
  return Umatch(s.to_string(), t);
}
/*--------------------------------------------------------------------------*/
inline bool wmatch(const std::string& s1, const IString_& s2)
{
  return wmatch(s1, s2.to_string());
}
/*--------------------------------------------------------------------------*/
inline bool wmatch(const IString_& s1, const std::string& s2)
{
  return wmatch(s1.to_string(), s2);
}
/*--------------------------------------------------------------------------*/
inline bool wmatch(const IString_& s1, const IString_& s2)
{
  return wmatch(s1.to_string(), s2.to_string());
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// implement legacy behaviour, but can't just use std::string.
class legacy_string : public std::string{
public:
  typedef std::string base;
  legacy_string(const std::string& s="") : base(s) {}
public:
  legacy_string& operator=(const std::string& s){
    base::operator=(s);
    return *this;
  }
  std::string const& to_string() const{ return *this; }
};
/*--------------------------------------------------------------------------*/
inline const char* strchr(const char *s, Ichar_ const& c)
{
  return strchr(s, c.to_char());
}
/*--------------------------------------------------------------------------*/
inline Ichar_ fix_case(Ichar_ c)
{
  return c;
}
/*--------------------------------------------------------------------------*/
inline int tolower(Ichar_ c)
{
  return c.to_char();
}
/*--------------------------------------------------------------------------*/
namespace notstd{
/*--------------------------------------------------------------------------*/
inline void to_lower(IString_*) { }
/*--------------------------------------------------------------------------*/
} // nostd
/*--------------------------------------------------------------------------*/
#if 1
typedef IString_ IString;
typedef Ichar_ Ichar;
#else
typedef legacy_string IString;
typedef char Ichar;
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif // guard
// vim:ts=8:sw=2:noet:

