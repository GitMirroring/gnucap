/*$Id: c_comand.h 2014.11.25 $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * all of the commands
 */
//testing=trivial 2006.07.17
#ifndef C_COMAND_H
#define C_COMAND_H
#include "e_card.h"
#include "d_dot.h"
/*--------------------------------------------------------------------------*/
class CS;
/*--------------------------------------------------------------------------*/
class INTERFACE CMD : public CARD {
  std::string _type;
protected:
  explicit CMD() : CARD() {}
  explicit CMD(CMD const& p) : CARD(p) {}
public:
  std::string value_name()const {return "";}
  virtual void do_it(CS&, CARD_LIST*) = 0;
  virtual ~CMD() {}
  static  void  cmdproc(CS&, CARD_LIST*, CMD* c=nullptr);
  static  void	command(const std::string&, CARD_LIST*);
  CARD*   clone()const override { return new DEV_DOT;} // //BUG// memory leak ?
public:	 // not a "dev". but still a type...
  void set_dev_type(std::string const& s)override;
  std::string dev_type()const override {return _type;}
  virtual void uninit() {unreachable();}
};
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
