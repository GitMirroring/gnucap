/*$Id: e_node.h $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 *               2025 Felix Salfelder
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
 * circuit node class
 */
//testing=script,sparse 2006.07.11
#ifndef E_NODE_H
#define E_NODE_H
#include "u_sim_data.h"
#include "u_node.h"
#include "e_card.h"
/*--------------------------------------------------------------------------*/
enum {
  OUT1 = 0,
  OUT2 = 1,
  IN1 = 2,
  IN2 = 3,
  NODES_PER_BRANCH = 4
};
/*--------------------------------------------------------------------------*/
// base class for various NODEs
// avoiding stuff that is not needed everywere.
// shield from accidental use
class NODE : public CARD {
protected:
  explicit NODE() : CARD() {}
private: // inhibited
  explicit NODE(const NODE& p) : CARD(p) { untested();unreachable();}
public:
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  explicit NODE(const std::string& s, int idx=0)
    : CARD(s) {(void)idx; /*assert(!idx);*/}
  ~NODE() {}

  CARD* clone()const override	{untested(); return new NODE(*this);}

public: // raw data access (rvalues)
  virtual int user_number()const;
  virtual int flat_number()const {return INVALID_NODE;}
public: // simple calculated data access (rvalues)
  virtual int matrix_number()const;
  int	m_()const		{return matrix_number();}
public: // virtuals
  double	tr_probe_num(const std::string&)const override;
  XPROBE	ac_probe_ext(const std::string&)const override;

  double      v0()const	{
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_v0[m_()];
  }
  double      vt1()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_vt1[m_()];
  }
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_ac[m_()];
  }
  //double      vdc()const		{untested();return _vdc[m_()];}

  //double&     i()	{untested();return _i[m_()];}  /* lvalues */
  COMPLEX&    iac() {
    assert(m_() >= 0);
    assert(m_() <= _sim->_total_nodes);
    return _sim->_ac[m_()];
  }
};
/*--------------------------------------------------------------------------*/
#ifndef NDEBUG
extern NODE ground_node;
#endif
/*--------------------------------------------------------------------------*/
inline int NODE::user_number() const
{
  // assert(this==&ground_node);
  return 0;
}
/*--------------------------------------------------------------------------*/
inline int NODE::matrix_number() const
{
  // assert(this==&ground_node);
  return 0;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
