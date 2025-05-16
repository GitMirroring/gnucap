/*                -*- C++ -*-
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
 * discipline and nature
 */
#ifndef E_DISC_H
#define E_DISC_H
#include "e_base.h"
/*--------------------------------------------------------------------------*/
enum {
  dom_default = 0,
  dom_continuous = 0,
  dom_discrete = 1
}domain_type;
/*--------------------------------------------------------------------------*/
class NATURE : public CKT_BASE {
  std::list<std::pair<std::string, std::string> > _attribs;
  double abstol{0.};
};
/*--------------------------------------------------------------------------*/
class DISCIPLINE : public CKT_BASE {
  domain_type _domain{dom_default};
  NATURE* _potential{nullptr}
  NATURE* _flow{nullptr}
public:
  explicit DISCIPLINE() : CKT_BASE {}
  ~DISCIPLINE() {}
};
/*--------------------------------------------------------------------------*/
/*
  connectrules_declaration ::=
  connectrules connectrules_identifier ;
  { connectrules_item }
  endconnectrules
  connectrules_item ::=
  connect_insertion
  | connect_resolution
  connect_insertion ::= connect connectmodule_identifier [ connect_mode ]
  [ parameter_value_assignment ] [ connect_port_overrides ] ;
  connect_mode ::= merged | split
  connect_port_overrides ::=
  discipline_identifier , discipline_identifier
  | input discipline_identifier , output discipline_identifier
  | output discipline_identifier , input discipline_identifier
  | inout discipline_identifier , inout discipline_identifier
  connect_resolution ::= connect discipline_identifier { , discipline_identifier } resolveto
  discipline_identifier_or_exclude ;
  discipline_identifier_or_exclude ::=
  discipline_identifier
  | exclude
*/
/*--------------------------------------------------------------------------*/
class CONNECTRULE{
  std::vector<int> _disciplines;
};
/*--------------------------------------------------------------------------*/
class CONNECTRULE_CM : public CONNECTRULE {
  ELEMENT const* _connectmodule{nullptr};
};
/*--------------------------------------------------------------------------*/
class CONNECTRULES {
  std::list<rule> _list;
public:
  explicit CONNECTRULES();
  void clear() {_list.clear(); }
  void push_back(ELEMENT const* e);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
