
#include "e_compon.h"
#include "e_subckt.h"
#include "globals.h"
#include "u_lang.h"
#include "u_opt.h"
#include "e_node.h"
// apps/paramset.cc?
namespace{
/*--------------------------------------------------------------------------*/
// similar to DEV_SUBCKT, but just forward to nested COMPONENT
// (does not work yet)
class DEV_PARAMSET : public COMPONENT {
private:
  explicit	DEV_PARAMSET(const DEV_PARAMSET& p)
    : COMPONENT(p), _comp(p._comp) { untested();
  }
  // maybe don't clone here..
  explicit DEV_PARAMSET(COMPONENT const* x)
    : COMPONENT(), _comp(prechecked_cast<COMPONENT*>(x->clone())) {
      trace3("cloned", _comp, _comp->max_nodes(), _comp->port_name(0));
    assert(_comp);
    _comp->set_owner(owner());
    // yikes. needed for port_value
    // maybe with this hack, some of the other overrides are not needed...
    _n = &_comp->n_(0);
//	 _n[0].new_node("foo", this);
//	 trace1("portvalue", port_value(0));
  }
public:
  explicit	DEV_PARAMSET()
    : _dev_type(""),
		_comp(NULL),
	   _parent(NULL)
  {
//    _n = &_comp->n_(0);
//	 _n[0].new_node("foo", this);
//	 trace1("portvalue", port_value(0));
  }
		~DEV_PARAMSET()		{--_count;}
  CARD*		clone()const		{untested(); return new DEV_PARAMSET(*this);}
  CARD*	new_wrap(COMPONENT const* x)const{
    assert(x);
    return new DEV_PARAMSET(x);
  }
private: // override virtual
  char		id_letter()const	{ return '\0';}
  bool		print_type_in_spice()const { return false;}
  std::string   value_name()const	{ return "#";}
  void   set_dev_type(std::string const& s) {
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
  void set_port_by_index(int num, std::string& ext_name) {
    assert(_comp);
    _comp->set_port_by_index(num, ext_name);
  }
  // necessary?
  std::string port_name(int i)const{
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
  double    tr_probe_num(const std::string&s)const{
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

  std::string dev_type() const{
    return _dev_type;
  }
  int max_nodes()const{
    assert(_comp);
    return(_comp->max_nodes());
  }
  int min_nodes()const{
    assert(_comp);
    return(_comp->min_nodes());
  }
  int matrix_nodes()const{untested();
    assert(_comp);
    return(_comp->matrix_nodes());
  }
  int net_nodes()const{
    assert(_comp);
    return(_comp->net_nodes());
  }
  void precalc_first(){
	  assert(_parent);
	  assert(_parent->subckt());
	  PARAM_LIST const* p=_parent->subckt()->params();
	  assert(p);

	  for (PARAM_LIST::const_iterator ci=p->begin(); ci!=p->end(); ++ci) {
		  _comp->set_param_by_name(ci->first, ci->second.string());
	  }
	  assert(_comp);
	  return(_comp->precalc_first());
  }
  bool makes_own_scope()const  {untested(); return false;}

  void expand(){
    assert(_comp);
    return(_comp->expand());
  }
private:
  void precalc_last(){
    assert(_comp);
    return(_comp->precalc_last());
  }
  int param_count_dont_print()const {return common()->COMMON_COMPONENT::param_count();}
public:
  static int count(){untested(); return _count;}
private:
  std::string _dev_type;
  COMPONENT* _comp;
  static int _count; // todo.
public:
  BASE_SUBCKT const* _parent;
} p1; // DEV_PARAMSET
int DEV_PARAMSET::_count;
/*--------------------------------------------------------------------------*/
// similar to DEV_SUBCKT_PROTO
class PARAMSET : public BASE_SUBCKT {
private:
  explicit	PARAMSET(const PARAMSET&p)
    : BASE_SUBCKT(p), _comp(p._comp)
  {
    new_subckt();
  }
public:
  explicit PARAMSET()
    : BASE_SUBCKT(), _comp(NULL)
  {
    new_subckt();
  }
  ~PARAMSET(){}
private: // override virtual
  void set_param_by_name(std::string n, std::string v){
    trace2("PARAMSET::set_param_by_name", n, v);
    PARAM_LIST* pl = scope()->params();
    assert(pl);
    pl->set(n, v);
  }
  char		id_letter()const	{untested();return 'y';}
  CARD*		clone_instance()const;

  bool		print_type_in_spice()const {unreachable(); return true;}
  std::string   value_name()const	{untested();incomplete(); return "";}
  void set_dev_type(std::string const& s) {
	  // why is this called twice??
    trace1("PARAMSET::set_dev_type", s);
    //CARD const* p = LANGUAGE::find_proto(s, NULL); // Scope?
    //CARD const* p = find_looking_out(s);
	 assert(OPT::language);
    CARD const* p = OPT::language->find_proto(s, NULL); // Scope?

    COMPONENT const* c=dynamic_cast<COMPONENT const*>(p);
    assert(c);  //for now.
    _comp = c;
    // yikes. needed for port_value. used in lang_spice
    _n = &_comp->n_(0);


	 for(unsigned i=0; i<net_nodes(); ++i){
		 // spice "list" needs this.
		 _n[i].new_node(_comp->port_name(i), this);
	 }
	 if(scope()->is_empty()){
		 // yuck. clone?
		 trace2("PARAMSET::set_dev_type proto", c->net_nodes(), c->id_letter());
		 CARD* cl=c->clone();
		 if(cl->id_letter()){
			 cl->set_label(std::string(1, cl->id_letter()) + "_");
		 }else{
			 cl->set_label("_");
		 }
		 scope()->push_back(cl);
	 }else{
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
  int net_nodes()const{
    if(_comp){
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
    if(_comp){
		 return "AAA"; // _comp->port_name(i);
	 }else{
		 incomplete();
		 return "";
	 }
  }
  bool port_exists(int i)const { untested();
    return i < net_nodes();
  }
private:
  std::string _dev_type;
  COMPONENT const* _comp;
} pm;
/*--------------------------------------------------------------------------*/
DISPATCHER<CARD>::INSTALL d1(&device_dispatcher, "paramset", &pm);
/*--------------------------------------------------------------------------*/
CARD* PARAMSET::clone_instance()const{
  assert(_comp);
  DEV_PARAMSET* new_instance = dynamic_cast<DEV_PARAMSET*>(p1.new_wrap(_comp));
  // COMPONENT* new_instance = dynamic_cast<COMPONENT*>(_comp->clone());
  assert(!new_instance->subckt());

  if (this == &pm){ incomplete();
    // cloning from static, empty model
    // look out for _parent in expand
  }else{
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
