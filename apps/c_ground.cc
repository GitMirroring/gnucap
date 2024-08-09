/*                 -*- C++ -*-
 * Copyright (C) 2024 Felix Salfelder
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
 * ground command
 */
#include "globals.h"
#include "c_comand.h"
#include "d_dot.h"
#include "u_node.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class DOT_GROUND : public DEV_DOT {
public:
  CARD_LIST* _scope{NULL}; // DEV_DOT?
public:
 // NODE_P _np;
  explicit DOT_GROUND() : DEV_DOT() {
    set_label("ground");
  }
  explicit DOT_GROUND(DOT_GROUND const& p) : DEV_DOT(p) {
    if(p._node.is_connected()){
      _node = p._node;
    }else{
    }
  }
  ~DOT_GROUND(){
   // delete _np.n_();
  }
  //int min_nodes()const override { return 1; }
 //  int max_nodes()const override { return 1; }
 //  int net_nodes()const override{ return _node; }
  DOT_GROUND* clone()const override {return new DOT_GROUND(*this);}
  void set_port_by_index(int i, std::string name) {
    if(i){
      throw Exception_Too_Many(i,1,0);
    }else{
      _node.new_node(name, this);
      _node.set_to_ground(this);
    }
  }
  NODE_P _node;
 //  CARD_LIST* scope()override { unreachable(); return NULL; }
 //  CARD_LIST const* scope()const override{
 //    return _scope;
 //  }
  void set_param_by_index(int i, std::string& what, int) {
    assert(!i); // for now.
    set_port_by_index(i, what);
    set("ground " + what + ";");
  }


}gnd;
DISPATCHER<CMD>::INSTALL d1(&device_dispatcher, "ground", &gnd);
/*--------------------------------------------------------------------------*/
class CMD_GROUND : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override {
    std::string name;
    cmd >> name >> ";";
    DOT_GROUND* dd = gnd.clone();
    dd->_scope = Scope;

    NODE_P& np = dd->_node;
    np.new_node(name, dd);

    if(np.is_grounded()){
    }else{
      _sim->uninit();
      np.set_to_ground(dd);
    }
    trace1("CMD_GROUND gnd", np->short_label());

    assert(np.is_grounded());
    assert(np->is_grounded());
    assert(np.is_connected());

    if(name == "0" && cmd.fullstring()[0]=='s'){
      // spice
    }else{
      dd->set(cmd.fullstring());
    }
    Scope->push_back(dd);

    if(cmd.more()){ untested();
      cmd.warn(bDANGER, "trailing characters");
    }else{
    }
  }
} p12;
DISPATCHER<CMD>::INSTALL d12(&command_dispatcher, "ground|.ground|spiceground", &p12);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
