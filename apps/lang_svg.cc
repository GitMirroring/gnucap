/*                       -*- C++ -*-
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
 */
#include "u_nodemap.h"
#include "globals.h"
#include "c_comand.h"
#include "d_dot.h"
#include "d_coment.h"
#include "e_subckt.h"
#include "e_model.h"
#include "u_lang.h"
#include "u_attrib.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class get_attrib : public CKT_BASE {
public:
  std::string pin_pos(char what, int idx, COMPONENT const* c, std::string slot="S0") {
    assert(idx<c->net_nodes());
    std::string port_name = c->port_name(idx);

    ATTRIB_LIST_p a0 = attributes(c->id_tag());
    std::string attrib_name = slot + "_" + what + "_" + port_name;
    if(!a0){ untested();
      throw Exception("no attributes " + c->long_label());
    }else{
      std::string a = a0.lookup(attrib_name);
      if(a != "0"){
	return a;
      }else{
      }
      attrib_name = slot + "_" + what + to_string(idx+1);
      a = a0.lookup(attrib_name);
      if(a.size()){
	return a;
      }else{
      }
    }
    throw Exception("can't find " + attrib_name);
  }
  std::string port_pos(char what, int idx, COMPONENT const* c, std::string slot="S0") {
    assert(idx<c->net_nodes());
    std::string port_name = c->port_name(idx);

    ATTRIB_LIST_p a0 = attributes(c->port_id_tag(idx));
    std::string attrib_name = slot + "_" + what + "_" + port_name;
    if(!a0){ untested();
      throw Exception("no attributes " + c->long_label() + ":" + port_name);
    }else{
      std::string a = a0.lookup(attrib_name);
      if(a != "0"){
	return a;
      }else{
      }
      attrib_name = slot + "_" + what + "1";
      a = a0.lookup(attrib_name);
      if(a.size()){
	return a;
      }else{
      }
    }
    throw Exception("can't find " + attrib_name);
  }
}ga;
/*--------------------------------------------------------------------------*/
class LANG_SVG : public LANGUAGE {
  enum MODE {mDEFAULT, mPARAMSET} _mode;
  mutable int arg_count;
  enum {INACTIVE = -1};
public:
  LANG_SVG() : arg_count(INACTIVE) {}
  ~LANG_SVG() {}
  std::string name()const override {return "svg";}
  bool case_insensitive()const override {return false;}
  UNITS units()const override {return uSI;}

private: // obsolete/unused
  std::string arg_front()const override { return ""; }
  std::string arg_mid()const override { return ""; }
  std::string arg_back()const override { return ""; }

public: // override virtual, called by commands
  void		parse_top_item(CS&, CARD_LIST*)override;
  DEV_COMMENT*	parse_comment(CS&, DEV_COMMENT*x)override {incomplete(); return x;}
  DEV_DOT*	parse_command(CS&, DEV_DOT*x)override;
  MODEL_CARD*	parse_paramset(CS&, MODEL_CARD*x)override {incomplete(); return x;}
  BASE_SUBCKT*  parse_module(CS&, BASE_SUBCKT*x)override {incomplete(); return x;}
  COMPONENT*	parse_instance(CS&, COMPONENT*x)override {incomplete(); return x;}
  std::string	find_type_in_string(CS&)override;

private: // override virtual, called by print_item
  void print_paramset(OMSTREAM&, const MODEL_CARD*)override {incomplete();}
  void print_module(OMSTREAM&, const BASE_SUBCKT*)override;
  void print_instance(OMSTREAM&, const COMPONENT*)override;
  void print_comment(OMSTREAM&, const DEV_COMMENT*)override {incomplete();}
  void print_command(OMSTREAM&, const DEV_DOT*)override {incomplete();}
private:
  void print_ports(OMSTREAM& o, const COMPONENT*) const;
} lang_svg;

DISPATCHER<LANGUAGE>::INSTALL
	d(&language_dispatcher, lang_svg.name(), &lang_svg);
/*--------------------------------------------------------------------------*/
std::string LANG_SVG::find_type_in_string(CS& cmd)
{
  size_t here = cmd.cursor();
  std::string type;
  if ((cmd >> "//")) { untested();
    //assert(here == 0);
    type = "dev_comment";
  }else{
    trace1("ftis", cmd.fullstring());
    cmd >> type;
  }
  cmd.reset(here); // where the type is.
  return type;
}
/*--------------------------------------------------------------------------*/
void LANG_SVG::parse_top_item(CS& cmd, CARD_LIST* Scope)
{
  cmd.get_line("gnucap-verilog>");
  new__instance(cmd, nullptr, Scope);
}
/*--------------------------------------------------------------------------*/
DEV_DOT* LANG_SVG::parse_command(CS& cmd, DEV_DOT* x)
{
  assert(x);
  x->set(cmd.fullstring());
  CARD_LIST* scope = (x->owner()) ? x->owner()->subckt() : &CARD_LIST::card_list;
  cmd.reset();
  cmd.skipbl();
  if(cmd.peek() == '`'){ untested();
  }else{
    // "module" etc gets here.
  }
  CMD::cmdproc(cmd, scope);
  x->purge();
  delete x;
  return nullptr;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SVG::print_ports(OMSTREAM& o, const COMPONENT* x) const
{
  assert(x);

  std::string pins;
  int cnt = 0;
  double x_;
  double y_;
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    try{
      std::string cx = ga.pin_pos('x', ii, x);
      std::string cy = ga.pin_pos('y', ii, x);

      x_ += atof(cx.c_str());
      y_ += atof(cy.c_str());

      pins += " <circle cx=\"" + cx + "\" cy=\"" + cy + "\" r=\"1\" fill=\"red\"/>\n";
      ++cnt;
    }catch(Exception const&){
    }
  }
  if(cnt){
    o << "<circle cx=\"" << int(x_/cnt) << "\" cy=\"" << int(y_/cnt) << "\" r=\"2\" fill=\"black\"/>\n";
  }else{
  }
  o << pins;
  // for (int ii = 0;  x->current_port_exists(ii);  ++ii) {untested();
  //   o << sep;
  //   //////print_attributes(o, x->port_id_tag(ii));
  //   o << mangle(x->current_port_value(ii));
  //   sep = ',';
  // }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SVG::print_module(OMSTREAM& o, const BASE_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  // double w = 100;
  // double h = 100;

  o << "<svg viewBox=\"-50 -50 150 100\"" // width=\"" << w << "\"" <<     " height=\"" << h << "\""
    << ">\n";

  std::string ports;

  COMPONENT const* c = x;
  for(int i=0; i<c->net_nodes(); ++i) {
	
      std::string cx = ga.port_pos('x', i, x);
      std::string cy = ga.port_pos('y', i, x);

//      x_ += atof(cx.c_str());
//      y_ += atof(cy.c_str());

      ports += " <circle cx=\"" + cx + "\" cy=\"" + cy + "\" r=\"1\" fill=\"green\"/>\n";
  }
  o << ports;
  o << "<!-- -->\n";

  for (CARD_LIST::const_iterator ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }

  o << "</svg>\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SVG::print_instance(OMSTREAM& o, const COMPONENT* x)
{
//  print_attributes(o, x->id_tag());
//  print_type(o, x);
//  print_args(o, x);
//  print_label(o, x);
  print_ports(o, x);
}
/*--------------------------------------------------------------------------*/
class CMD_SVG : public CMD {
public:
  void do_it(CS&, CARD_LIST* Scope)override {
    if(Scope == &CARD_LIST::card_list) {
    }else{ untested();
    }
    command("options lang=svg", Scope);
  }
} p8;
DISPATCHER<CMD>::INSTALL d8(&command_dispatcher, "svg", &p8);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
