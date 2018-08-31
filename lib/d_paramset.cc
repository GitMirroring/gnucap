
#define DO_TRACE
#include "e_compon.h"
#include "e_subckt.h"
#include "globals.h"
#include "u_lang.h"
#include "u_opt.h"
#include "e_node.h"
#include "e_paramlist.h"
// apps/paramset.cc?
namespace{
static COMMON_PARAMLIST Default_PARAMSET(CC_STATIC);
/*--------------------------------------------------------------------------*/
// similar to DEV_SUBCKT, but just forward to nested COMPONENT
// (does not work yet)
class DEV_PARAMSET : public COMPONENT {
private:
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

public:
  explicit	DEV_PARAMSET()
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
  // skipping assert(_comp) here.
  void      tr_iwant_matrix(){ _comp->tr_iwant_matrix(); }
  double    tr_probe_num(const std::string&s)const{ untested();
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
	  PARAM_LIST const* p=_parent->subckt()->params();
	  assert(p);

	  if (subckt()) { untested();
		 COMMON_PARAMLIST* c = prechecked_cast<COMMON_PARAMLIST*>(mutable_common());
		 assert(c);
		 subckt()->attach_params(&(c->_params), scope());
		 subckt()->precalc_first();

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
	  _comp->precalc_first();
	  _scopehack = nullptr;
//	  _comp->set_owner(owner());
	  }else{ untested();
	  }
  }
  bool makes_own_scope()const  {untested(); return false;}

  // use common
//  void set_param_by_name(std::string n, std::string v){ untested();
//	  _instance_params.set(n,v);
//  }

  void expand(){ untested();
	  if(!subckt()){ untested();
		  new_subckt();
	  }
    assert(_comp);
    return(_comp->expand());
  }
private:
  void precalc_last(){ untested();
    assert(_comp);
	  _scopehack = subckt();
    return(_comp->precalc_last());
	  _scopehack = nullptr;
  }
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}
public:
  static int count(){untested(); return _count;}
private:
  std::string _dev_type;
  COMPONENT* _comp;
  static int _count; // todo.

  CARD_LIST* _scopehack;
public:
  BASE_SUBCKT const* _parent;
  PARAM_LIST _instance_params;
} p1; // DEV_PARAMSET
int DEV_PARAMSET::_count;
/*--------------------------------------------------------------------------*/
// similar to DEV_SUBCKT_PROTO
class PARAMSET : public BASE_SUBCKT {
private:
  explicit	PARAMSET(const PARAMSET&p)
    : BASE_SUBCKT(p), _comp(p._comp)
  { untested();
    new_subckt();
	 set_label("dontknow");
  }
public:
  explicit PARAMSET()
    : BASE_SUBCKT(), _comp(NULL)
  { untested();
    new_subckt();
	 set_label("dontknow");
  }
  ~PARAMSET(){}
private: // override virtual
  void set_param_by_name(std::string n, std::string v){ untested();
	  trace3("PARAMSET::set_param_by_name", n, v, _comp);
	  PARAM_LIST* pl = scope()->params();
	  assert(pl);
	  assert(_comp);
	  _comp->set_param_by_name(n, v);
  }
  char		id_letter()const	{untested();return 'y';}
  CARD*		clone_instance()const;

  bool		print_type_in_spice()const {unreachable(); return true;}
  std::string   value_name()const	{untested();incomplete(); return "";}
  void set_dev_type(std::string const& s) { untested();
	  // why is this called twice??
    trace2("PARAMSET::set_dev_type", dev_type(), s);
    //CARD const* p = LANGUAGE::find_proto(s, NULL); // Scope?
    //CARD const* p = find_looking_out(s);
	 assert(OPT::language);
    CARD const* p = OPT::language->find_proto(s, NULL); // Scope?

    COMPONENT const* c=dynamic_cast<COMPONENT const*>(p);
    assert(c);  //for now.
    _comp = prechecked_cast<COMPONENT*>(c->clone());

	 if(subckt()->begin() == subckt()->end()){ untested();
		 _comp->set_label(std::string(1,_comp->id_letter())+"_");
		 subckt()->push_back(_comp); // will be picked up here for spice "netlisting"
	 }else{ untested();
		 incomplete();
	 }

    // yikes. needed for port_value. used in lang_spice
    _n = &_comp->n_(0);


	 for(unsigned i=0; i<net_nodes(); ++i){ untested();
		 // spice "list" needs this.
		 _n[i].new_node(_comp->port_name(i), this);
	 }
	 if(scope()->is_empty()){ untested();
		 // yuck. clone?
		 trace2("PARAMSET::set_dev_type proto", c->net_nodes(), c->id_letter());
		 CARD* cl=c->clone();
		 if(cl->id_letter()){ untested();
			 cl->set_label(std::string(1, cl->id_letter()) + "_");
		 }else{ untested();
			 cl->set_label("_");
		 }
		 scope()->push_back(cl);
	 }else{ untested();
		 // why?
	 }

    trace2("PARAMSET::set_dev_type proto", c->net_nodes(), c);
    _dev_type = s;
  }
  std::string dev_type()const		{ untested();
    return _dev_type;
  }
  int		max_nodes()const	{incomplete(); return 0;}
  //int		min_nodes()const	{incomplete(); return 0;}
  //int		matrix_nodes()const	{incomplete(); untested();return 0;}
  int net_nodes()const{ untested();
    if(_comp){ untested();
		 return(_comp->net_nodes());
	 }else{ incomplete();
		 return 0;
	 }
  }
  CARD*		clone()const		{return new PARAMSET(*this);}
  bool		is_device()const	{return false;}
  bool		makes_own_scope()const  {return true;}
  CARD_LIST*	   scope()		{return subckt();}
  const CARD_LIST* scope()const		{return subckt();}
private: // no-ops for prototype. same as DEV_SUBCKT_PROTO
  void precalc_first(){}
  void expand(){}
  void precalc_last(){}
  void map_nodes(){}
  void tr_begin(){}
  void tr_load(){}
  TIME_PAIR tr_review(){ return TIME_PAIR(NEVER, NEVER);}
  void tr_accept(){}
  void tr_advance(){}
  void tr_restore(){}
  void tr_regress(){}
  void dc_advance(){}
  void ac_begin(){}
  void do_ac(){}
  void ac_load(){}
  bool do_tr(){ return true;}
  bool tr_needs_eval()const{untested(); return false;}
  void tr_queue_eval(){}
  std::string port_name(int i)const{ untested();
    trace3("port_name", i, _comp->dev_type(), _comp);
    if(_comp){ untested();
		 return "AAA"; // _comp->port_name(i);
	 }else{ untested();
		 incomplete();
		 return "";
	 }
  }
  bool port_exists(int i)const { untested();
    return i < net_nodes();
  }
private:
  std::string _dev_type;
  COMPONENT* _comp;
} pm;
/*--------------------------------------------------------------------------*/
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "paramset", &pm);
/*--------------------------------------------------------------------------*/
CARD* PARAMSET::clone_instance()const{ untested();
  assert(_comp);
  DEV_PARAMSET* new_instance = dynamic_cast<DEV_PARAMSET*>(p1.new_wrap(_comp));
  // COMPONENT* new_instance = dynamic_cast<COMPONENT*>(_comp->clone());
  //assert(!new_instance->subckt());

  if (this == &pm){ incomplete();
    // cloning from static, empty model
    // look out for _parent in expand
  }else{ untested();
    new_instance->_parent = this;
  }

//  new_instance->set_owner(owner());
  assert(new_instance->is_device());
  assert(_comp);
//  new_instance.set_proto(_comp);
  return new_instance;
}
/*--------------------------------------------------------------------------*/
} // d_paramset
