/*                -*- C++ -*-
 * Copyright (C) 2025-26 Felix Salfelder
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
 * node type, aka "discipline"
 */
#ifndef E_NODE_TYPE
#define E_NODE_TYPE
#include "e_node.h"
/*--------------------------------------------------------------------------*/
class NODE_TYPE : public NODE {
  enum domain_type {
    dom_unknown = 0,
    dom_mixed = 1,
    dom_continuous = 2,
    dom_discrete = 3
  };
  int _type_number{INVALID_NODE};
  domain_type _domain{dom_unknown};
public:
  explicit NODE_TYPE(std::string const&);
  ~NODE_TYPE();

public:
  void set_continuous() {_domain = dom_continuous;}
  void set_mixed()     {_domain = dom_mixed;}
  void set_discrete()   {untested(); _domain = dom_discrete;}
  bool is_continuous()const  {return _domain == dom_continuous;}
  bool is_mixed()const      {return _domain == dom_mixed;}
  bool is_discrete()const    {untested(); return _domain == dom_discrete;}

private:
  int user_number()const override   {unreachable(); return INVALID_NODE;}
  int flat_number()const override   {unreachable(); return INVALID_NODE;}
  int matrix_number()const override {unreachable(); return INVALID_NODE;}
public:
  int type_number()const override   { return _type_number;}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
