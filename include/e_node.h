/*$Id: e_node.h 2018/05/27 al$ -*- C++ -*-
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
 * circuit node class
 */
//testing=script,sparse 2006.07.11
#ifndef E_NODE_H
#define E_NODE_H
#include "u_sim_data.h"
#include "e_card.h"
#include "u_xprobe.h" // BUG
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC;
/*--------------------------------------------------------------------------*/
enum {
  OUT1 = 0,
  OUT2 = 1,
  IN1 = 2,
  IN2 = 3,
  NODES_PER_BRANCH = 4,
  INVALID_NODE = -1
};
#define	qBAD	 (0)
#define qGOOD	 (OPT::transits)
/*--------------------------------------------------------------------------*/
enum _LOGICVAL {lvSTABLE0,lvRISING,lvFALLING,lvSTABLE1,lvUNKNOWN};
enum {lvNUM_STATES = lvUNKNOWN+1};
/*--------------------------------------------------------------------------*/
class INTERFACE LOGICVAL {
private:
  _LOGICVAL _lv;
  static const _LOGICVAL or_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL xor_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL and_truth[lvNUM_STATES][lvNUM_STATES];
  static const _LOGICVAL not_truth[lvNUM_STATES];
public:
  LOGICVAL() :_lv(lvUNKNOWN)			{}
  LOGICVAL(const LOGICVAL& p)	:_lv(p._lv)	{}
  LOGICVAL(_LOGICVAL p)		:_lv(p)		{}
  ~LOGICVAL() {}

  operator _LOGICVAL()const {return static_cast<_LOGICVAL>(_lv);}
  
  LOGICVAL& operator=(_LOGICVAL p)	 {_lv=p; return *this;}
  LOGICVAL& operator=(const LOGICVAL& p) {_lv=p._lv; return *this;}

  LOGICVAL& operator&=(LOGICVAL p)
	{untested(); _lv = and_truth[_lv][p._lv]; return *this;}
  LOGICVAL& operator|=(LOGICVAL p)
	{_lv = or_truth[_lv][p._lv]; return *this;}
  LOGICVAL  operator^=(LOGICVAL p)
	{untested(); _lv = xor_truth[_lv][p._lv]; return *this;}
  LOGICVAL  operator~()const	{return not_truth[_lv];}
  
  bool is_unknown()const	{return _lv == lvUNKNOWN;}
  bool lv_future()const		{assert(_lv!=lvUNKNOWN); return _lv & 1;}
  bool lv_old()const		{assert(_lv!=lvUNKNOWN); return _lv & 2;}

  bool is_rising() const	{return _lv == lvRISING;}
  bool is_falling()const	{return _lv == lvFALLING;}

  LOGICVAL& set_in_transition(LOGICVAL newval);
};
/*--------------------------------------------------------------------------*/
class NODE_DATA{
private:
  NODE_DATA(): _user_number(INVALID_NODE) {unreachable();}
public:
  explicit NODE_DATA(int n): _user_number(n) {}
  explicit NODE_DATA(NODE_DATA const& p): _user_number(p._user_number) {}
public:
  virtual ~NODE_DATA(){}
  int user_number()const       {return _user_number;}
  NODE_DATA& set_user_number(int n)    {_user_number = n; return *this;}

public: // PORT?
  virtual COMPLEX     vac()const = 0;
  virtual COMPLEX&     iac() = 0;
//  virtual std::string  short_label() const{incomplete(); return "NODE_DATA"; }
private:
  int _user_number;
};
/*--------------------------------------------------------------------------*/
class NODE : public CARD, public NODE_DATA {
private: // CARD overrides
  CARD* clone() const;
  std::string value_name()const {return "";} // pure in CARD
  bool	is_device()const		{return false;}
private:
  int	_flat_number;
protected:
  explicit NODE();
private: // inhibited
  explicit NODE(const NODE& p);
public:
  explicit NODE(const NODE* p); // u_nodemap.cc:49 (deep copy)
  explicit NODE(const std::string& s, int n);
  ~NODE() {}

  CARD* new_card();

public: // raw data access (rvalues)
  int	flat_number()const	{itested();return _flat_number;}
public: // simple calculated data access (rvalues)
  int	matrix_number()const	{untested(); return _sim->_nm[_flat_number];}
  int	m_()const		{return matrix_number();}
public: // maniputation
  NODE& set_flat_number(int n) {itested();_flat_number = n; return *this;}
  //NODE& set_matrix_number(int n){untested();_matrix_number = n;return *this;}
public: // virtuals
  double	tr_probe_num(const std::string&)const;
  XPROBE	ac_probe_ext(const std::string&)const;

  double      v0()const	{ untested();
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

public:
  NODE_DATA* data();
private:
  LOGIC_NODE* _nnn;
}; // NODE
extern NODE ground_node;
typedef NODE NODE_CARD; // TODO
/*--------------------------------------------------------------------------*/
class INTERFACE node_t {
private:
  static bool node_is_valid(NODE const*);
  static bool node_is_valid(int i) {
    if (i == INVALID_NODE) {
    }else if (i < 0) {
      unreachable();
    }else if (i > NODE::_sim->_total_nodes) {
      unreachable();
    }else{
    }
    return i>=0 && i<=NODE::_sim->_total_nodes;
  }
  static int  to_internal(int n) {
    assert(node_is_valid(n));
    assert(NODE::_sim->_nm);
    return NODE::_sim->_nm[n];
  }

private:
  NODE_DATA* _nnn;
  int _m;		// mapped, after reordering

public:
  int	      m_()const	{return _m;}

  int	      t_()const {
    if(_nnn){
      assert(n_());
      int i = n_()->flat_number();
      assert (i <= NODE::_sim->_total_nodes);
      return i;
    }else{
      return INVALID_NODE;
    }
  }	// e_cardlist.cc:CARD_LIST::map_subckt_nodes:436 and
	// e_node.h:node_t::map:263,265 only

  // number in parent scope
  int	      e_()const {
    return ((_nnn) ? _nnn->user_number() : INVALID_NODE);
  }
  const NODE* n_()const {return prechecked_cast<NODE const*>(_nnn);}
  NODE*	      n_()	{return prechecked_cast<NODE*>(_nnn);}

  
  const std::string  short_label()const {return ((n_()) ? (n_()->short_label()) : "?????");}
  void	set_to_ground(CARD*);
  void	new_node(const std::string&, const CARD*);
  void	new_model_node(const std::string& n, CARD* d);
  void	map_subckt_node(NODE** map_array, const CARD* d);
  bool	is_grounded()const {return (e_() == 0);}
  bool	is_connected()const {return (e_() != INVALID_NODE);}

  node_t&     map(){
    if (_nnn == &ground_node) {
      _m = 0;
    }else if (t_() != INVALID_NODE) {
      assert(_nnn);
      NODE_CARD* nn = prechecked_cast<NODE_CARD*>(_nnn);
      if(nn){ untested();
	_nnn = nn->data();
	_m = to_internal(t_());
      }else{ untested();
	_m=0; // gnd node hack. BUG
      }
      assert(_nnn);
//      assert(prechecked_cast<LOGIC_NODE*>(_nnn));
    }else{
      assert(_m == INVALID_NODE);
    }
    return *this;
  } // e_compon.cc:COMPONENT::map_nodes:522
/*--------------------------------------------------------------------------*/

  explicit    node_t();
	      node_t(const node_t&);
  explicit    node_t(NODE*);
	      ~node_t() {}

private: // raw data access (lvalues)
  NODE_DATA&	data()const;

public:
  NODE_DATA&	    operator*()const	{return data();}
  const NODE_DATA* operator->()const	{return &data();}
  NODE_DATA*	    operator->()	{return &data();}

  node_t& operator=(const node_t& p);
  node_t& operator=(NODE* p);

  bool operator==(const node_t& p) const;

public:
  double      v0()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    //assert(n_()->m_() == m_());
    //assert(n_()->v0() == NODE::_sim->_v0[m_()]);
    return NODE::_sim->_v0[m_()];
  }
  double      vt1()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    return NODE::_sim->_vt1[m_()];
  }
  
  COMPLEX     vac()const {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    assert(n_());
    //assert(n_()->m_() == m_());
    //assert(n_()->vac() == NODE::_ac[m_()]);
    return NODE::_sim->_ac[m_()];
  }
  
  double&     i() {
    assert(m_() >= 0);
    assert(m_() <= NODE::_sim->_total_nodes);
    return NODE::_sim->_i[m_()];
  }
#if 1
  COMPLEX&    iac() {untested();
    if(n_()){
      assert(n_()->m_() == m_());
      // assert(n_()->iac() == NODE::_sim->_ac[m_()]);
    }
    assert(m_()!=INVALID_NODE);
    //return n_()->iac();
    return NODE::_sim->_ac[m_()];
  }
#endif
}; // node_t
/*--------------------------------------------------------------------------*/
INTERFACE double volts_limited(const node_t& n1, const node_t& n2);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
// vim:ts=8:sw=2:noet:
