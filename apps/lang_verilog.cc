/*$Id: lang_verilog.cc $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
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
 */
//testing=script 2016.09.10
#define DO_TRACE
#include "u_nodemap.h"
#include "globals.h"
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "e_subckt.h"
#include "e_model.h"
#include "u_lang.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class LANG_VERILOG : public LANGUAGE {
  enum MODE {mDEFAULT, mPARAMSET} _mode;
  mutable int arg_count;
  enum {INACTIVE = -1};
public:
  LANG_VERILOG() : arg_count(INACTIVE) {}
  ~LANG_VERILOG() {}
  std::string name()const {return "verilog";}
  bool case_insensitive()const {return false;}
  UNITS units()const {return uSI;}

public: // override virtual, used by callback
  std::string arg_front()const {untested();
    switch (_mode) {
    case mPARAMSET:untested(); return " .";			    break;
    case mDEFAULT:untested();  return (arg_count++ > 0) ? ", ." : "."; break;
    }
    unreachable();
    return "";
  }
  std::string arg_mid()const {untested();
    switch (_mode) {
    case mPARAMSET:untested(); return "="; break;
    case mDEFAULT:untested();  return "("; break;
    }
    unreachable();
    return "";
  }
  std::string arg_back()const {untested();
    switch (_mode) {
    case mPARAMSET:untested(); return ";"; break;
    case mDEFAULT:untested();  return ")"; break;
    }
    unreachable();
    return "";
  }

public: // override virtual, called by commands
  void		parse_top_item(CS&, CARD_LIST*);
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*);
  DEV_DOT*	parse_command(CS&, DEV_DOT*);
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*);
  BASE_SUBCKT*  parse_module(CS&, BASE_SUBCKT*);
  COMPONENT*	parse_instance(CS&, COMPONENT*);
  std::string	find_type_in_string(CS&);

private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*);
  void print_module(OMSTREAM&, const BASE_SUBCKT*);
  void print_instance(OMSTREAM&, const COMPONENT*);
  void print_comment(OMSTREAM&, const DEV_COMMENT*);
  void print_command(OMSTREAM& o, const DEV_DOT* c);
private: // local
  void print_args(OMSTREAM&, const MODEL_CARD*);
  void print_args(OMSTREAM&, const COMPONENT*);
} lang_verilog;

DISPATCHER<LANGUAGE>::INSTALL
	d(&language_dispatcher, lang_verilog.name(), &lang_verilog);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
static void parse_type(CS& cmd, CARD* x)
{ untested();
  assert(x);
  std::string new_type;
  cmd >> new_type;
  trace1("setting dev_type", new_type);
  x->set_dev_type(new_type);
}
/*--------------------------------------------------------------------------*/
static void parse_args_paramset(CS& cmd, MODEL_CARD* x)
{ untested();
  assert(x);

  while (cmd >> '.') { untested();
    unsigned here = cmd.cursor();
    std::string name, value;
    try{ untested();
      cmd >> name >> '=' >> value >> ';';
      x->set_param_by_name(name, value);
    }catch (Exception_No_Match&) { untested();
      cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
    }
  }
}
/*--------------------------------------------------------------------------*/
static void parse_args_instance(CS& cmd, CARD* x)
{ untested();
  assert(x);

  if (cmd >> "#(") { untested();
    if (cmd.match1('.')) { untested();
      // by name
      while (cmd >> '.') { untested();
	unsigned here = cmd.cursor();
	std::string name  = cmd.ctos("(", "", "");
	std::string value = cmd.ctos(",)", "(", ")");
	cmd >> ',';
	try{ untested();
	  x->set_param_by_name(name, value);
	}catch (Exception_No_Match&) { untested();
	  cmd.warn(bDANGER, here, x->long_label() + ": bad parameter " + name + " ignored");
	}
      }
    }else{ untested();
      // by order
      int index = 1;
      while (cmd.is_alnum() || cmd.match1("+-.")) { untested();
	unsigned here = cmd.cursor();
	try{ untested();
	  std::string value = cmd.ctos(",)", "", "");
	  x->set_param_by_index(x->param_count() - index++, value, 0/*offset*/);
	}catch (Exception_Too_Many& e) {untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
    }
    cmd >> ')';
  }else{ untested();
    // no args
  }
}
/*--------------------------------------------------------------------------*/
static void parse_label(CS& cmd, CARD* x)
{ untested();
  assert(x);
  std::string my_name;
  if (cmd >> my_name) { untested();
    x->set_label(my_name);
  }else{ untested();
    x->set_label(x->id_letter() + std::string("_unnamed")); //BUG// not unique
    cmd.warn(bDANGER, "label required");
  }
}
/*--------------------------------------------------------------------------*/
static void parse_ports(CS& cmd, COMPONENT* x, bool all_new)
{ untested();
  assert(x);

  if (cmd >> '(') { untested();
    if (cmd.is_alnum()) { untested();
      // by order
      int index = 0;
      while (cmd.is_alnum()) { untested();
	unsigned here = cmd.cursor();
	try{ untested();
	  std::string value;
	  cmd >> value;
	  x->set_port_by_index(index, value);
	  if (all_new) { untested();
	    if (x->node_is_grounded(index)) { untested();
	      cmd.warn(bDANGER, here, "node 0 not allowed here");
	    }else if (x->subckt() && x->subckt()->nodes()->how_many() != index+1) { untested();
	      cmd.warn(bDANGER, here, "duplicate port name, skipping");
	    }else{ untested();
	      ++index;
	    }
	  }else{ untested();
	    ++index;
	  }
	}catch (Exception_Too_Many& e) { untested();
	  cmd.warn(bDANGER, here, e.message());
	}
      }
      if (index < x->min_nodes()) { untested();
	cmd.warn(bDANGER, "need " + to_string(x->min_nodes()-index) +" more nodes, grounding");
	for (int iii = index;  iii < x->min_nodes();  ++iii) { untested();
	  x->set_port_to_ground(iii);
	}
      }else{ untested();
      }
    }else{ untested();
      // by name
      while (cmd >> '.') { untested();
	unsigned here = cmd.cursor();
	try{ untested();
	  std::string name, value;
	  cmd >> name >> '(' >> value >> ')' >> ',';
	  x->set_port_by_name(name, value);
	}catch (Exception_No_Match&) {untested();
	  cmd.warn(bDANGER, here, "mismatch, ignored");
	}
      }
      for (int iii = 0;  iii < x->min_nodes();  ++iii) { untested();
	if (!(x->node_is_connected(iii))) {untested();
	  cmd.warn(bDANGER, x->port_name(iii) + ": port unconnected, grounding");
	  x->set_port_to_ground(iii);
	}else{ untested();
	}
      }
    }
    cmd >> ')';
  }else{ untested();
    cmd.warn(bDANGER, "'(' required (parse ports) (grounding)");
    for (int iii = 0;  iii < x->min_nodes();  ++iii) { untested();
      if (!(x->node_is_connected(iii))) { untested();
	cmd.warn(bDANGER, x->port_name(iii) + ": port unconnected, grounding");
	x->set_port_to_ground(iii);
      }else{ untested();
	unreachable();
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DEV_COMMENT* LANG_VERILOG::parse_comment(CS& cmd, DEV_COMMENT* x)
{ untested();
  assert(x);
  x->set(cmd.fullstring());
  return x;
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_VERILOG::parse_command(CS& cmd, DEV_DOT* x)
{ untested();
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;

  cmd.reset();
  CMD::cmdproc(cmd, scope);
  delete x;
  return NULL;
}
/*--------------------------------------------------------------------------*/
/* "paramset" <my_name> <base_name> ";"
 *    <paramset_item_declaration>*
 *    <paramset_statement>*
 *  "endparamset"
 */
//BUG// no paramset_item_declaration, falls back to spice mode

MODEL_CARD* LANG_VERILOG::parse_paramset(CS& cmd, MODEL_CARD* x)
{ untested();
  assert(x);
  cmd.reset();
  cmd >> "paramset ";
  parse_label(cmd, x);
  bool args_allowed=true;
  //try{
    parse_type(cmd, x);
  //}catch(Exception_Cant_Find const&){ untested();
  //  args_allowed=false;
  //}
  cmd >> ';';

  for (;;) { untested();
    if(args_allowed){ untested();
      parse_args_paramset(cmd, x);
    }else{ untested();
    }
    if (cmd >> "endparamset ") { untested();
      break;
    }else if (cmd.umatch("param{eter} ")) { untested();
      args_allowed = false;

#if 0
      if(is BASE_SUBCKT and x->subckt()){ untested();
  //void new__instance(CS& cmd, BASE_SUBCKT* owner, CARD_LIST* Scope);
	new__instance(cmd, x, x->subckt());
      }else{ untested();
	cmd.check(bWARNING, "what's this?");
      }
#endif

    }else if (!cmd.more()) { untested();
      cmd.get_line("verilog-paramset>");
    }else{ untested();
      cmd.check(bWARNING, "what's this?");
      break;
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
/* "module" <name> "(" <ports> ")" ";"
 *    <declarations>
 *    <netlist>
 * "endmodule"
 */
//BUG// strictly one device per line

BASE_SUBCKT* LANG_VERILOG::parse_module(CS& cmd, BASE_SUBCKT* x)
{ untested();
  assert(x);

  // header
  cmd.reset();
  (cmd >> "module |macromodule ");
  parse_label(cmd, x);
  parse_ports(cmd, x, true/*all new*/);
  cmd >> ';';

  // body
  for (;;) { untested();
    cmd.get_line("verilog-module>");

    if (cmd >> "endmodule ") { untested();
      break;
    }else{ untested();
      new__instance(cmd, x, x->subckt());
    }
  }
  return x;
}
/*--------------------------------------------------------------------------*/
COMPONENT* LANG_VERILOG::parse_instance(CS& cmd, COMPONENT* x)
{ untested();
  assert(x);
  cmd.reset();
  parse_type(cmd, x);
  parse_args_instance(cmd, x);
  parse_label(cmd, x);
  parse_ports(cmd, x, false/*allow dups*/);
  cmd >> ';';
  cmd.check(bWARNING, "what's this?");
  return x;
}
/*--------------------------------------------------------------------------*/
std::string LANG_VERILOG::find_type_in_string(CS& cmd)
{ untested();
  unsigned here = cmd.cursor();
  std::string type;
  if ((cmd >> "//")) { untested();
    assert(here == 0);
    type = "dev_comment";
  }else{ untested();
    cmd >> type;
  }
  cmd.reset(here);
  return type;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::parse_top_item(CS& cmd, CARD_LIST* Scope)
{ untested();
  cmd.get_line("gnucap-verilog>");
  new__instance(cmd, NULL, Scope);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_args(OMSTREAM& o, const MODEL_CARD* x)
{ untested();
  assert(x);
  if (x->use_obsolete_callback_print()) {untested();
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{ untested();
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) { untested();
      if (x->param_is_printable(ii)) { untested();
	std::string arg = " ." + x->param_name(ii) + "=" + x->param_value(ii) + ";";
	o << arg;
      }else{ untested();
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_args(OMSTREAM& o, const COMPONENT* x)
{ untested();
  assert(x);
  o << " #(";
  if (x->use_obsolete_callback_print()) { untested();
    arg_count = 0;
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
    arg_count = INACTIVE;
  }else{ untested();
    std::string sep = ".";
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) { untested();
      if (x->param_is_printable(ii)) { untested();
	o << sep << x->param_name(ii) << "(" << x->param_value(ii) << ")";
	sep = ",.";
      }else{ untested();
      }
    }
  }
  o << ") ";
}
/*--------------------------------------------------------------------------*/
static void print_type(OMSTREAM& o, const COMPONENT* x)
{ untested();
  assert(x);
  o << x->dev_type();
}
/*--------------------------------------------------------------------------*/
static void print_label(OMSTREAM& o, const COMPONENT* x)
{ untested();
  assert(x);
  o << x->short_label();
}
/*--------------------------------------------------------------------------*/
static void print_ports_long(OMSTREAM& o, const COMPONENT* x)
{ untested();
  // print in long form ...    .name(value)
  assert(x);

  o << " (";
  std::string sep = ".";
  for (int ii = 0;  x->port_exists(ii);  ++ii) { untested();
    o << sep << x->port_name(ii) << '(' << x->port_value(ii) << ')';
    sep = ",.";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
    o << sep << x->current_port_name(ii) << '(' << x->current_port_value(ii) << ')';
    sep = ",.";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
static void print_ports_short(OMSTREAM& o, const COMPONENT* x)
{ untested();
  // print in short form ...   value only
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) { untested();
    o << sep << x->port_value(ii);
    sep = ",";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
    o << sep << x->current_port_value(ii);
    sep = ",";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{ untested();
  assert(x);
  _mode = mPARAMSET;
  o << "paramset " << x->short_label() << ' ' << x->dev_type() << ";\\\n";
  print_args(o, x);
  if(x->subckt()){ untested();
    incomplete();
    // print params...
  }
  o << "\\\n"
    "endparamset\n\n";
  _mode = mDEFAULT;
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_module(OMSTREAM& o, const BASE_SUBCKT* x)
{ untested();
  assert(x);
  assert(x->subckt());

  o << "module " <<  x->short_label();
  print_ports_short(o, x);
  o << ";\n";

  PARAM_LIST const* p=x->subckt()->params();

  for (PARAM_LIST::const_iterator ci=p->begin(); ci!=p->end(); ++ci) { untested();
    o << "parameter " << ci->first;
    if(ci->second.has_good_value()){ untested();
      o << "={" << ci->second.string() << "}";
    }else{ untested();
    }
    o << ";\n";
  }
  
  for (CARD_LIST::const_iterator 
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) { untested();
    print_item(o, *ci);
  }
  
  o << "endmodule // " << x->short_label() << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_instance(OMSTREAM& o, const COMPONENT* x)
{ untested();
  print_type(o, x);
  print_args(o, x);
  print_label(o, x);
  print_ports_long(o, x);
  o << ";\n";
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{ untested();
  assert(x);
  if ((x->comment().compare(0, 2, "//")) != 0) {untested();
    o << "//";
  }else{ untested();
  }
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_VERILOG::print_command(OMSTREAM& o, const DEV_DOT* x)
{untested();
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
class PARAMSET : public MODEL_CARD{
public:
  PARAMSET(COMPONENT const*c):MODEL_CARD(c) {}
  ~PARAMSET(){ untested();
    if(dynamic_cast<MODEL_CARD* const>( _component_proto)){ untested();
      // nested model card is not ours.
    }else{ untested();
      delete _component_proto;
    }
  }
private: // overrides
  CARD* clone()const{untested();
    return new PARAMSET(*this);
  }
  CARD*	clone_instance()const{ untested();
    assert(_component_proto);
    return _component_proto->clone_instance();
  }
  std::string dev_type()const { untested();
    if(_component_proto){ untested();
      return _component_proto->dev_type();
    }else{ untested();
      return "";
    }
  }
  int param_count()const { untested();
    if(_component_proto){
      return _component_proto->param_count();
    }else{
      // something went wrong with parsing?
      return 0;
    }
  }
  void set_param_by_index(int i, std::string& value, int offset){ untested();
    assert(_component_proto);
    _component_proto->set_param_by_index(i, value, offset);
  }
  bool param_is_printable(int i)const{ untested();
    assert(_component_proto);
    return _component_proto->param_is_printable(i);
  }
  std::string param_name(int i)const{ untested();
    assert(_component_proto);
    return _component_proto->param_name(i);
  }
  std::string param_name(int i, int j)const{ untested();
    assert(_component_proto);
    return _component_proto->param_name(i, j);
  }
  std::string param_value(int i)const{ untested();
    assert(_component_proto);
    return _component_proto->param_value(i);
  }
  void set_dev_type(std::string const& s) { untested();
    trace1("DEV_PARAMSET::set_dev_type", s);
    assert(OPT::language);
    const CARD* p=OPT::language->find_proto(s, this);

    if(dynamic_cast<MODEL_CARD const*>(p)){ untested();
      _component_proto = p->clone();
    }else if(dynamic_cast<COMPONENT const*>(p)){ untested();
      //		  new_subckt(); // to store parameters (later?)
      _component_proto = p->clone();
    }else{ untested();
      throw Exception_Cant_Find(long_label(), s);
      unreachable(); // really?
    }
  }
};
PARAMSET ps(NULL);
DISPATCHER<CARD>::INSTALL psd(&device_dispatcher, "paramset", &ps);
/*--------------------------------------------------------------------------*/
class CMD_PARAMSET : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  { untested();
    // already got "paramset"
    std::string my_name, base_name;
    cmd >> my_name;
    unsigned here = cmd.cursor();    
    cmd >> base_name;

    // model_dispatcher? does it matter?
    CARD* n=device_dispatcher.clone("paramset");
    assert(n);
    MODEL_CARD* new_card=dynamic_cast<MODEL_CARD*>(n);
    assert(new_card);

    try{ untested();
      assert(!new_card->owner());
      assert(!new_card->subckt());
      assert(new_card->scope());
      lang_verilog.parse_paramset(cmd, new_card);
    }catch(...){ untested();
      incomplete();
      cmd.warn(bDANGER, here, "paramset: no match");
      delete new_card;
      new_card=NULL;
    }

    if(!new_card){
      // something went wrong.
    }else if(MODEL_CARD const* proto=dynamic_cast<MODEL_CARD const*>(
	  new_card->component_proto()))
    { untested();
      MODEL_CARD* mproto = const_cast<MODEL_CARD*>(proto);
      mproto->set_label(new_card->short_label());

      Scope->push_back(const_cast<CARD*>(new_card->component_proto()));
      delete new_card; // will not delete proto, as it is a MODEL_CARD
    }else{
      Scope->push_back(new_card);
    }
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "paramset", &p1);
/*--------------------------------------------------------------------------*/
class CMD_MODULE : public CMD {
  void do_it(CS& cmd, CARD_LIST* Scope)
  { untested();
    BASE_SUBCKT* new_module = dynamic_cast<BASE_SUBCKT*>(device_dispatcher.clone("subckt"));
    assert(new_module);
    assert(!new_module->owner());
    assert(new_module->subckt());
    assert(new_module->subckt()->is_empty());
    assert(!new_module->is_device());
    lang_verilog.parse_module(cmd, new_module);
    Scope->push_back(new_module);
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "module|macromodule", &p2);
/*--------------------------------------------------------------------------*/
class CMD_VERILOG : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)
  { untested();
    command("options lang=verilog", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "verilog", &p8);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
