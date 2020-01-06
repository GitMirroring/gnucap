/*$Id: e_card.cc 2016/09/17 $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
//testing=script 2014.07.04
#include "u_time_pair.h"
#include "e_cardlist.h"
#include "e_node.h"
#include "e_card.h"
#include "e_subckt.h"
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 4;
/*--------------------------------------------------------------------------*/
class ROOT_CARD : public CARD{
public:
  ROOT_CARD(){
    set_label("(root)");
    set_owner(NULL);
  }
private:
  bool makes_own_scope() const{return true;}
  CARD_LIST* scope(){untested(); return NULL;}
  CARD_LIST const* scope()const{untested(); return NULL;}
  CARD*	 clone()const{unreachable(); return NULL;}
  std::string value_name()const { unreachable(); return ""; }
} root_card;
/*--------------------------------------------------------------------------*/
CARD::CARD()
  :CKT_BASE(),
   _evaliter(-100),
   _owner(&root_card),
   _constant(false),
   _net_nodes(0)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const CARD& p)
  :CKT_BASE(p),
   _evaliter(-100),
   _owner(&root_card),
   _constant(p._constant),
   _net_nodes(p._net_nodes)
{
}
/*--------------------------------------------------------------------------*/
CARD::~CARD()
{
}
/*--------------------------------------------------------------------------*/
const std::string CARD::long_label()const
{
  CARD const* brh = owner();
  if(brh && brh->owner() && brh->owner()!=&root_card) {
    return brh->long_label() + '.' + short_label();
  }else{
    return short_label();
  }
}
/*--------------------------------------------------------------------------*/
/* connects_to: does this part connect to this node?
 * input: a node
 * returns: how many times this part connects to it.
 * does not traverse subcircuits
 */
int CARD::connects_to(const node_t& node)const
{untested();
  incomplete();
  return 0;
#if 0
  int count = 0;
  if (is_device()) {untested();
    for (int ii = 0;  ii < net_nodes();  ++ii) {untested();
      if (node.n_() == _n[ii].n_()) {untested();
        ++count;
      }else{untested();
      }
    }
  }else{untested();
  }
  return count;
#endif
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::scope()
{
  if (BASE_SUBCKT* o=dynamic_cast<BASE_SUBCKT*>(owner())) {
    return o->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
const CARD_LIST* CARD::scope()const
{
  if (BASE_SUBCKT const* o=dynamic_cast<BASE_SUBCKT const*>(owner())) {
    trace1("CARD::scope", owner()->long_label());
    return o->subckt();	// normal element, owner determines scope
  }else{ untested();
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
void CARD::set_owner(CARD* o)
{
  assert(_owner==&root_card||_owner==o);
  _owner=o;
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception if can't find.
 */
CARD* CARD::find_in_my_scope(const std::string& name)
{
  assert(name != "");
  assert(scope());

  CARD_LIST::iterator i = scope()->find_(name);
  if (i == scope()->end()) {
    if(owner()){
      throw Exception_Cant_Find(long_label(), name, owner()->long_label());
    }else{
      throw Exception_Cant_Find(long_label(), name, "(root)"); // gah
    }
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception if can't find.
 */
const CARD* CARD::find_in_my_scope(const std::string& name)const
{
  assert(name != "");
  if(!scope()){
    throw Exception_Cant_Find(long_label(), name, "");
  }else{
  }

  CARD_LIST::const_iterator i = scope()->find_(name);
  if (i == scope()->end()) {
    trace2("CARD::find_in_my_scope", long_label(), name);
    if(owner()){
      throw Exception_Cant_Find(long_label(), name, owner()->long_label());
    }else{
      throw Exception_Cant_Find(long_label(), name, "(root)"); // gah
    }
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_in_parent_scope: find in parent's scope
 * parent is what my scope is a copy of.
 * capable of finding my parent, who should be just like me.
 * If there is no parent (I'm an original), use my scope.
 * throws exception if can't find.
 */
const CARD* CARD::find_in_parent_scope(const std::string& name)const
{
  assert(name != "");
  if(!scope()){
    throw Exception_Cant_Find(long_label(), name, "");
  }else{
  }
  const CARD_LIST* p_scope = (scope()->parent()) ? scope()->parent() : scope();

  CARD_LIST::const_iterator i = p_scope->find_(name);
  if (i == p_scope->end()) {
    throw Exception_Cant_Find(long_label(), name);
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_looking_out: find in my or enclosing scope
 * capable of finding me, or anything back to root.
 * throws exception if can't find.
 */
const CARD* CARD::find_looking_out(const std::string& name)const
{
  trace2("find_looking_out", short_label(), name);
  try {
    return find_in_parent_scope(name);
  }catch (Exception_Cant_Find&) {
    if (owner()) {
      trace1("owner?", short_label());
      try{
	return owner()->find_looking_out(name);
      }catch (Exception_Cant_Find&) { untested();
	 throw Exception_Cant_Find(long_label(), name);
      }
    }else if (makes_own_scope()) { untested();
      // probably a subckt or "module"
      CARD_LIST::const_iterator i = CARD_LIST::card_list.find_(name);
      if (i != CARD_LIST::card_list.end()) {
	return *i;
      }else{
	throw;
      }
    }else{ untested();
      throw;
    }
  }
}
/*--------------------------------------------------------------------------*/
TIME_PAIR CARD::tr_review()
{
  return TIME_PAIR(NEVER,NEVER);
}
/*--------------------------------------------------------------------------*/
void CARD::set_param_by_name(std::string Name, std::string Value)
{
  //BUG// ugly linear search
  for (int i = param_count() - 1;  i >= 0;  --i) {
    for (int j = 0;  param_name(i,j) != "";  ++j) { // multiple names
      if (Umatch(Name, param_name(i,j) + ' ')) {
	set_param_by_index(i, Value, 0/*offset*/);
	return; //success
      }else{
	//keep looking
      }
    }
  }
  throw Exception_No_Match(Name);
}
/*--------------------------------------------------------------------------*/
/* set_dev_type: Attempt to change the type of an existing device.
 * Usually, it just throws an exception, unless there is no change.
 * Practical use is to override, so you can set things like NPN vs. PNP.
 */
void CARD::set_dev_type(const std::string& New_Type)
{
  if (!Umatch(New_Type, dev_type() + ' ')) {
    //throw Exception_Cant_Set_Type(dev_type(), New_Type);
  }else{
    // it matches -- ok.
  }
}
/*--------------------------------------------------------------------------*/
bool CARD::evaluated()const
{
  if (_evaliter == _sim->iteration_tag()) {
    return true;
  }else{
    _evaliter = _sim->iteration_tag();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
