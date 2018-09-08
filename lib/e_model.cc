/*$Id: e_model.cc,v 26.137 2010/04/10 02:37:33 al Exp $ -*- C++ -*-
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
 * base class for all models
 */
//testing=script 2006.07.12
#define DO_TRACE
#include "e_compon.h"
#include "e_model.h"
#include "e_paramlist.h"
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD(const COMPONENT* p)
  :CARD(),
   _component_proto(NULL)
{
  if (p) {
    _component_proto = p->clone();
  }else{
    assert(!_component_proto);
  }
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::MODEL_CARD(const MODEL_CARD& p)
  :CARD(p),
   _component_proto(NULL)
{
  if (p._component_proto) {
    _component_proto = p._component_proto->clone();
  }else{
    assert(!_component_proto);
  }
  if (_sim) {
    _sim->uninit();
  }else{untested();
  }
}
/*--------------------------------------------------------------------------*/
MODEL_CARD::~MODEL_CARD()
{
  if (_component_proto) {
    delete _component_proto;
    _component_proto = NULL;
  }else{
  }
  if (_sim) {
    _sim->uninit();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::set_param_by_index(int i, std::string& value, int offset)
{
  if (_component_proto) {
    _component_proto->set_param_by_index(i, value, offset);
  }else{untested();
    CARD::set_param_by_index(i, value, offset);
  }
}
/*--------------------------------------------------------------------------*/
bool MODEL_CARD::param_is_printable(int i)const
{
  if (_component_proto) {
    return _component_proto->param_is_printable(i);
  }else{untested();
    return CARD::param_is_printable(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_name(int i)const
{
  if (_component_proto) {
    return _component_proto->param_name(i);
  }else{untested();
    return CARD::param_name(i);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_name(int i, int j)const
{
  if (_component_proto) {
    return _component_proto->param_name(i, j);
  }else{untested();
    return CARD::param_name(i, j);
  }
}
/*--------------------------------------------------------------------------*/
std::string MODEL_CARD::param_value(int i)const
{
  if (_component_proto) {
    return _component_proto->param_value(i);
  }else{untested();
    return CARD::param_value(i);
  }
}
/*--------------------------------------------------------------------------*/
void MODEL_CARD::precalc_first()
{
  if (_component_proto) {
    _component_proto->precalc_first();
  }else{
    CARD::precalc_first();
  }
}
/*--------------------------------------------------------------------------*/
static COMMON_PARAMLIST Default_PARAMSET(CC_STATIC);
class DEV_PARAMSET : public COMPONENT {
public:
  explicit	DEV_PARAMSET(const DEV_PARAMSET& p)
    : COMPONENT(p), _comp(p._comp) { untested();
      attach_common(&Default_PARAMSET);
    }
  explicit DEV_PARAMSET(COMPONENT const* x)
    : COMPONENT(), _comp(prechecked_cast<COMPONENT*>(x->clone())) { untested();
      trace3("cloned", _comp, _comp->max_nodes(), _comp->port_name(0));
      assert(_comp);
     attach_common(&Default_PARAMSET);
      _scopehack=nullptr;
      _comp->set_owner(this); // "this" needs a scope...
      _n = &_comp->n_(0);
      if(!subckt()){ untested();
	new_subckt();
      }
    }
public:
#if 1
  CARD_LIST* scope(){ untested();
    if( _scopehack){ untested();
      return _scopehack;
    }else{ untested();
      // assert(owner());
      assert( CARD::scope() );
      return CARD::scope();
    }
  }
  CARD_LIST const* scope() const{ untested();
    if( _scopehack){ untested();
      return _scopehack;
    }else{ untested();
      return CARD::scope();
    }
  }
#endif

public:
  explicit DEV_PARAMSET()
    : _dev_type(""),
    _comp(NULL),
    _parent(NULL)
  { untested();
    //    _n = &_comp->n_(0);
    //	 _n[0].new_node("foo", this);
    //	 trace1("portvalue", port_value(0));
  }
  ~DEV_PARAMSET()		{--_count;}
  CARD*		clone()const		{assert(false);untested(); return new DEV_PARAMSET(*this);}
  CARD*	new_wrap(COMPONENT const* x)const{ untested();
    assert(x);
    auto r=new DEV_PARAMSET(x);
    return r;
  }
private: // override virtual
  char		id_letter()const	{ return '\0';}
  bool		print_type_in_spice()const { return false;}
  std::string   value_name()const	{ return "#";}
  void   set_dev_type(std::string const& s) { untested();
    // not optimal. perhaps could use COMPONENT::set_dev_type...
    _dev_type = s;
    // TODO: check if that's what parent is...
    trace1("DEV_PARAMSET::set_dev_type", s);
  }
private: // port overrides
  const std::string port_value(int i)const{ untested();
    unreachable(); // not virtual?!
    trace3("port_value", i, _comp->dev_type(), _comp);
    assert(_comp);
    return _comp->port_value(i);
  }
  void set_port_by_index(int num, std::string& ext_name) { untested();
    assert(_comp);
    _comp->set_owner(this); // ??!
    assert(scope());
    assert(_comp->scope());
    assert(_comp->owner());
    assert(scope());
    _comp->set_port_by_index(num, ext_name);
  }
  // necessary?
  std::string port_name(int i)const{ untested();
    trace3("port_name", i, _comp->dev_type(), _comp);
    assert(_comp);
    incomplete();
    return _comp->port_name(i);
  }
  bool port_exists(int i)const { untested();
    return i < net_nodes();
  }
private: // simulation stuff
         // BUG: adds extra indirection
	 // just for parameters..
  void      tr_iwant_matrix(){assert(_comp); _comp->tr_iwant_matrix(); }
  double    tr_probe_num(const std::string&s)const{ untested();
    assert(_comp);
    return _comp->tr_probe_num(s);
  }
  void ac_iwant_matrix(){ _comp->ac_iwant_matrix(); }
  void tr_begin()	{ assert(_comp); _comp->tr_begin();}
  void tr_restore()	{untested(); assert(_comp); _comp->tr_restore();}
  void dc_advance()	{ assert(_comp); _comp->dc_advance();}
  void tr_advance()	{untested(); assert(_comp); _comp->tr_advance();}
  void tr_regress()	{untested(); assert(_comp); _comp->tr_regress();}
  bool tr_needs_eval()const {untested(); assert(_comp); return _comp->tr_needs_eval();}
  void tr_queue_eval() { assert(_comp); _comp->tr_queue_eval();}
  bool do_tr() {untested(); assert(_comp);set_converged(_comp->do_tr());return converged();}
  void tr_load()	{ assert(_comp); _comp->tr_load();}
  TIME_PAIR tr_review()	{untested(); assert(_comp); return _time_by = _comp->tr_review();}
  void tr_accept() { assert(_comp); _comp->tr_accept();}
  void tr_unload() {untested(); assert(_comp); _comp->tr_unload();}
  void ac_begin() {untested(); assert(_comp); _comp->ac_begin();}
  void do_ac() {untested(); assert(_comp); _comp->do_ac();}
  void ac_load()	{untested(); assert(_comp); _comp->ac_load();}
private:

  std::string dev_type() const{ untested();
    return _dev_type;
  }
  int max_nodes()const{ untested();
    assert(_comp);
    return(_comp->max_nodes());
  }
  int min_nodes()const{ untested();
    assert(_comp);
    return(_comp->min_nodes());
  }
  int matrix_nodes()const{untested();
    assert(_comp);
    return(_comp->matrix_nodes());
  }
  int net_nodes()const{ untested();
    assert(_comp);
    return(_comp->net_nodes());
  }
  void precalc_first(){ untested();
    COMPONENT::precalc_first(); // mfactor...
    assert(_parent);
    assert(_parent->subckt());
    PARAM_LIST* p=_parent->subckt()->params();
    assert(p);
    for(auto i: *p){
      trace2("parent", i.first, i.second);
    }

    if (subckt()) { untested();
      COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
      assert(c);
      subckt()->attach_params(p, scope());
      //subckt()->attach_params(&(c->_params), scope()); // DEV_SUBCKT
      subckt()->precalc_first();

    for(auto i: *subckt()->params()){
      trace2("scktp", i.first, i.second);
    }

      //	  need to evaluate _comp parameters but in subckt()->scope()
      //for (PARAM_LIST::const_iterator ci=p->begin(); ci!=p->end(); ++ci) { untested();
      //   _comp->set_param_by_name(ci->first, ci->second.string());
      //}
      assert(_comp);
      assert(scope());
      assert(subckt());
      _comp->set_owner(this); // "this" needs a scope...

      untested();
      assert(_comp->scope());
      assert(_comp->owner());
      assert(_comp->owner()->scope());
      _scopehack = subckt();
    for(auto i: *subckt()->params()){
      trace2("PF", i.first, i.second);
    }
      _comp->precalc_first();
      _scopehack = nullptr;
      //	  _comp->set_owner(owner());
    }else{ untested();
    }
  }
  void precalc_last(){ untested();
    assert(_comp);
    _scopehack = subckt();
    for(auto i: *subckt()->params()){
      trace2("PL", i.first, i.second);
    }

    return(_comp->precalc_last());
    _scopehack = nullptr;
  }
  bool makes_own_scope()const  {untested(); return false;}

  void expand(){ untested();
    if(!subckt()){ untested();
      new_subckt();
    }
    assert(_comp);
    return(_comp->expand());
  }
private:
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}
public:
  static int count(){untested(); return _count;}
private:
  std::string _dev_type;
  static int _count; // todo.

  CARD_LIST* _scopehack;
public:
  COMPONENT* _comp;
  MODEL_CARD const* _parent;
} p1; // DEV_PARAMSET
int DEV_PARAMSET::_count;
/*--------------------------------------------------------------------------*/
CARD* MODEL_CARD::clone_instance()const
{ untested();
  if (COMPONENT* p=dynamic_cast<COMPONENT*>(_component_proto)) { untested();
    DEV_PARAMSET* x = new DEV_PARAMSET(p);

    //     foo->_params.eval_copy(*subckt()->params(), s);
    x->_parent = this;
    return x;
  }else if (_component_proto) { untested();
    return _component_proto->clone_instance();
  }else{
    return NULL;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
