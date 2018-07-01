/*$Id: d_logic.h,v 26.133 2009/11/26 04:58:04 al Exp $ -*- C++ -*-
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
 * data structures and defaults for logic model.
 */
//testing=script,sparse 2006.07.17
#ifndef D_LOGIC_H
#define D_LOGIC_H
#include "e_model.h"
#include "e_elemnt.h"
#include <set>
/*--------------------------------------------------------------------------*/
enum {PORTS_PER_GATE = 10};
/*--------------------------------------------------------------------------*/
// DEV_LOGIC_GATE?
class DEV_LOGIC : public ELEMENT {
public:
  enum {OUTNODE=0,GND_NODE=1,PWR_NODE=2,ENABLE=3,BEGIN_IN=4}; //node labels
private:
  int		_lastchangenode;
  int		_quality;
  std::string	_failuremode;
  smode_t	_oldgatemode;
  smode_t	_gatemode;
  static int	_count;
protected:
  node_t	nodes[PORTS_PER_GATE];	/* PORTS_PER_GATE <= PORTSPERSUBCKT */
public:
  explicit	DEV_LOGIC();
  explicit	DEV_LOGIC(const DEV_LOGIC& p);
		~DEV_LOGIC()		{}
private: // override virtuals
  char	   id_letter()const	{return 'U';}
  std::string value_name()const	{return "#";}

public: // lang_spice quirks
  bool	      print_type_in_spice()const {return true;}
  std::string dev_type()const {assert(has_common());
    if(!_evaluator){
      return "dunno";
      // this is printed in spice netlists
    }else if(common()->modelname() == ""){
      return _modelname + " " + _evaluator->dev_type();
    }else{
      return common()->modelname() + " " + _evaluator->dev_type();
    }
  }
  std::string name() const{
    if(_evaluator){
      return _evaluator->dev_type();
    }else{
      return "dunno";
    }
  }
  int	   tail_size()const	{return 2;}
private: // more overrides
  int	   max_nodes()const	{return PORTS_PER_GATE;}
  int	   min_nodes()const	{return BEGIN_IN+1;}
  int	   matrix_nodes()const	{return 2;}
  int	   net_nodes()const	{return _net_nodes;}
private:
  bool	param_is_printable(int)const{ return false;}
  CARD*	   clone()const		{return new DEV_LOGIC(*this);}
  void	   precalc_first() {ELEMENT::precalc_first(); if (subckt()) {subckt()->precalc_first();}}
  void	   expand();
  void	   precalc_last() {ELEMENT::precalc_last(); if (subckt()) {subckt()->precalc_last();}}
  //void   map_nodes();

  void	   tr_iwant_matrix();
  void	   tr_begin();
  void	   tr_restore();
  void	   dc_advance();
  void	   tr_advance();
  void	   tr_regress();
  bool	   tr_needs_eval()const;
  void	   tr_queue_eval();
  bool	   do_tr();
  void	   tr_load();
  TIME_PAIR tr_review();
  void	   tr_accept();
  void	   tr_unload();
  double   tr_involts()const		{unreachable(); return 0;}
  //double tr_input()const		//ELEMENT
  double   tr_involts_limited()const	{unreachable(); return 0;}
  //double tr_input_limited()const	//ELEMENT
  //double tr_amps()const		//ELEMENT
  double   tr_probe_num(const std::string&)const;

  void	   ac_iwant_matrix();
  void	   ac_begin();
  void	   do_ac()	{untested();  assert(subckt());  subckt()->do_ac();}
  void	   ac_load()	{untested();  assert(subckt());  subckt()->ac_load();}
  COMPLEX  ac_involts()const		{unreachable(); return 0.;}
  COMPLEX  ac_amps()const		{unreachable(); return 0.;}
  XPROBE   ac_probe_ext(const std::string&)const;

  std::string port_name(int i)const { itested();
    switch (i){
      case 0:
	return "out";
      case 1:
	return "gnd";
      case 2:
	return "vdd";
      case 3:
	return "en";
      default:
	  return "in"+to_string(i-3);
    }
  }
protected:
  unsigned incount() const{
    trace1("incount", net_nodes());
    assert(net_nodes()>=4);
    return unsigned(net_nodes()-4);
  }
public:
//  static int count()			{return _count;}
private:
  // spice hack. was in common previously.
  // now, gates are standalone COMPONENTS.
  virtual LOGICVAL logic_eval(const node_t* n, unsigned incount)const{
    assert(_evaluator);
    return _evaluator->logic_eval(n, incount);
  }

  bool	   tr_eval_digital();
  bool	   want_analog()const;
  bool	   want_digital()const;
public: // spice hack.
  DEV_LOGIC const* _evaluator;
  std::string _modelname;
};
/*--------------------------------------------------------------------------*/
class MODEL_LOGIC : public COMMON_COMPONENT {
private: // types
  struct less_logic{
    bool operator()(MODEL_LOGIC const* a, MODEL_LOGIC const* b) {
      assert(a);
      assert(b);
      return *a<*b;
    }
  };
  typedef std::set<MODEL_LOGIC*, less_logic> container_type;
  typedef container_type::iterator iterator;
protected:
  explicit MODEL_LOGIC(const MODEL_LOGIC& p);
  explicit MODEL_LOGIC(int x)
    : COMMON_COMPONENT(x),
      delay(1e-9),
      vmax(5.),
      vmin(0.),
      unknown((vmax+vmin)/2),
      rise(delay / 2),
      fall(delay / 2),
      rs(100.),
      rw(1e9),
      th1(.75),
      th0(.25),
      mr(5.),
      mf(5.),
      over(.1),
      tnom(27.),
      range(vmax - vmin),
      _deflated(false)
  {
  }
  ~MODEL_LOGIC(){
    if(_deflated){
      if(_commons.erase(this)){
      }else{ untested();
	unreachable();
      }
    }else{
    }
  }
public:
  COMMON_COMPONENT* deflate(){
    trace2("deflate", delay.string(), name());
    _deflated = true;
    std::pair<iterator, bool> i=_commons.insert(this);
    _deflated = i.second; // "false" triggers attach...
    COMMON_COMPONENT* ret=*i.first;
    trace3("deflated", this, ret, _commons.size());
    return ret;
  }
  bool operator==(const COMMON_COMPONENT&)const;
  bool operator<(MODEL_LOGIC const&)const;
public: // move to BUILTIN_LOGIC?
			/* ----- digital mode ----- */
  PARAMETER<double> delay;	/* propagation delay */
			/* -- conversion parameters both ways -- */
  PARAMETER<double> vmax;	/* nominal volts for logic 1 */
  PARAMETER<double> vmin;	/* nominal volts for logic 0 */
  PARAMETER<double> unknown;	/* nominal volts for unknown (bogus) */
			/* ---- D to A conversion ---- */
  PARAMETER<double> rise;	/* rise time (time in slope) */
  PARAMETER<double> fall;	/* fall time (time in slope) */
  PARAMETER<double> rs; 	/* series resistance -- strong */
  PARAMETER<double> rw; 	/* series resistance -- weak */
			/* ---- A to D conversion ---- */
  PARAMETER<double> th1;	/* threshold for 1 as fraction of range */
  PARAMETER<double> th0;	/* threshold for 0 as fraction of range */
  	        	/* ---- quality judgement parameters ---- */
  PARAMETER<double> mr;		/* margin rise - how much worse rise can be */
  PARAMETER<double> mf;		/* margin fall - how much worse fall can be */
  PARAMETER<double> over;	/* overshoot limit - as fraction of range */
public: // MODEL parameters
  PARAMETER<double> tnom;
public: // calculated parameters
  double range;			/* vmax - vmin */
private:
  bool _deflated;
  static container_type _commons;
};
/*--------------------------------------------------------------------------*/
// this is a COMMON_COMPONENT.
class BUILTIN_LOGIC : public MODEL_LOGIC {
public:
  explicit	BUILTIN_LOGIC(int x);
private:
  explicit	BUILTIN_LOGIC(const BUILTIN_LOGIC& p);
private: // override virtuals
  std::string	dev_type()const		{return "logic";}
  void precalc_first(const CARD_LIST*);
  void		set_param_by_index(int, std::string&, int);
  bool		param_is_printable(int)const;
  std::string	param_name(int)const;
  std::string	param_name(int,int)const;
  std::string	param_value(int)const;
  int		param_count()const	{return (14 + COMMON_COMPONENT::param_count());}
public:
  void expand(COMPONENT const* a){
    set_modelname(a->common()->modelname());
  }
public:
  COMMON_COMPONENT* clone()const {
    COMMON_COMPONENT* r= new BUILTIN_LOGIC(*this);
    return r;
  }
  static int	count()			{return _count;}
  std::string name()const{ itested(); return "builtin"; }
  const std::string long_label()const{ itested(); return "builtin"; }
public: // move parameters back to here?
private:
  static int _count;
};
/*--------------------------------------------------------------------------*/
class LOGIC_AND : public DEV_LOGIC {
private:
  COMPONENT* clone()const { return new LOGIC_AND(*this);}
  std::string dev_type()const {return "and";}
public:
  explicit LOGIC_AND() : DEV_LOGIC() {}
  LOGICVAL logic_eval(const node_t* n, unsigned incount)const {itested();
    LOGICVAL out(n[0]->lv());
    for (unsigned ii=1; ii<incount; ++ii) {itested();
      out &= n[ii]->lv();
    }
    return out;
  }
};
/*--------------------------------------------------------------------------*/
class LOGIC_NAND : public DEV_LOGIC {
private:
  COMPONENT* clone()const {untested(); return new LOGIC_NAND(*this);}
  std::string dev_type()const {return "nand";}
public:
  explicit LOGIC_NAND() : DEV_LOGIC() {}
  LOGICVAL logic_eval(const node_t* n, unsigned incount)const {itested();
    LOGICVAL out(n[0]->lv());
    for (unsigned ii=1; ii<incount; ++ii) {untested();
      out &= n[ii]->lv();
    }
    return ~out;
  }
};
/*--------------------------------------------------------------------------*/
class LOGIC_OR : public DEV_LOGIC {
private:
  COMPONENT* clone()const {untested(); return new LOGIC_OR(*this);}
  std::string dev_type()const {return "or";}
public:
  explicit LOGIC_OR() : DEV_LOGIC() {itested();}
  LOGICVAL logic_eval(const node_t* n, unsigned incount)const {itested();
    LOGICVAL out(n[0]->lv());
    for (unsigned ii=1; ii<incount; ++ii) {untested();
      out |= n[ii]->lv();
    }
    return out;
  }
};
/*--------------------------------------------------------------------------*/
class LOGIC_NOR : public DEV_LOGIC {
private:
  COMPONENT* clone()const {untested(); return new LOGIC_NOR(*this);}
  std::string dev_type()const {return "nor";}
public:
  explicit LOGIC_NOR() : DEV_LOGIC() {}
  LOGICVAL logic_eval(const node_t* n, unsigned incount)const {itested();
    LOGICVAL out(n[0]->lv());
    for (unsigned ii=1; ii<incount; ++ii) {
      out |= n[ii]->lv();
    }
    return ~out;
  }
};
/*--------------------------------------------------------------------------*/
class LOGIC_XOR : public DEV_LOGIC {
private:
  COMPONENT* clone()const {untested(); return new LOGIC_XOR(*this);}
  std::string dev_type()const {return "xor";}
public:
  explicit LOGIC_XOR() : DEV_LOGIC() {}
  LOGICVAL logic_eval(const node_t* n, unsigned incount)const {itested();
    LOGICVAL out(n[0]->lv());
    for (unsigned ii=1; ii<incount; ++ii) {untested();
      out ^= n[ii]->lv();
    }
    return out;
  }
};
/*--------------------------------------------------------------------------*/
class LOGIC_XNOR : public DEV_LOGIC {
private:
  COMPONENT* clone()const {untested(); return new LOGIC_XNOR(*this);}
  std::string dev_type()const {return "xnor";}
public:
  explicit LOGIC_XNOR() : DEV_LOGIC() {untested();}
  LOGICVAL logic_eval(const node_t* n, unsigned incount)const {itested();
    LOGICVAL out(n[0]->lv());
    for (unsigned ii=1; ii<incount; ++ii) {untested();
      out ^= n[ii]->lv();
    }
    return ~out;
  }
};
/*--------------------------------------------------------------------------*/
class LOGIC_INV : public DEV_LOGIC {
private:
  COMPONENT* clone()const { return new LOGIC_INV(*this); }
  std::string	dev_type()const		{return "inv";}
public:
  explicit LOGIC_INV() : DEV_LOGIC() {}
  LOGICVAL logic_eval(const node_t* n, unsigned)const {
    return ~n[0]->lv();
  }
};
#if 0
/*--------------------------------------------------------------------------*/
class LOGIC_NONE : public DEV_LOGIC {
private:
  explicit LOGIC_NONE(const LOGIC_NONE&p):COMMON_LOGIC(p){itested();++_count;}
  COMMON_COMPONENT* clone()const {itested(); return new LOGIC_NONE(*this);}
public:
  explicit LOGIC_NONE(int c=0)		  :COMMON_LOGIC(c) {}
  LOGICVAL logic_eval(const node_t*)const {untested();
    return lvUNKNOWN;
  }
  virtual std::string name()const	  {untested();return "error";}
};
#endif
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif

// vim:ts=8:sw=2:noet:
