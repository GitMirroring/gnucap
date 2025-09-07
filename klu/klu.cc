/*                             -*- C++ -*-
 * Copyright (C) 2025 Felix Salfelder
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
 * KLU wrapper
 * -------------------------------------------------
 */
#include "globals.h"
#include "l_smallset.h"
#include "m_matrix_solver.h"
#include "u_sim_data.h"
#include "e_base.h"
#include "c_comand.h"
#include <suitesparse/klu.h>
/*--------------------------------------------------------------------------*/
template<class T>
int count_nz(T const& t)
{
  size_t nz = 0;
  for(auto const& i : t){
    nz += i.size();
  }
  return int(nz);
}
/*--------------------------------------------------------------------------*/
// serialize BS adjacency list. row oriented:
// row idx are at diag[i]   .. diag[i]+rowsize-1
// col idx are at diag[i]-1 .. diag[i]-colsize
// ordered ascending from the diagonal
//
// swap args for col orientation
template<class container_t>
int serialize_bs_adj(container_t const& rows_, container_t const& cols_,
    int*& _idx, int*& _didx)
{
  int unz = count_nz(cols_)
          + count_nz(rows_);

  _idx = new int[unz + cols_.size() + 1];
  _didx = new int[cols_.size()];
  int sep = 0;
  _idx[sep] = 0;

  for(int i = 0; i < int(cols_.size()); ++i ){
    _didx[i] = sep + 1 + cols_[i].size();
    int s = 0;
    for(int x : cols_[i]){
      _idx[_didx[i] - s - 1] = x;
      ++s;
    }
    s = 0;
    for(int x : rows_[i]){
      _idx[_didx[i] + s] = x;
      ++s;
    }
    sep = _didx[i] + s;
    _idx[sep] = 0;
  }

  assert(sep == unz + int(cols_.size()));
  return unz;
}
/*--------------------------------------------------------------------------*/
class BUCKETS{
  std::vector<int> _values;
  std::vector<int> _next;
  std::vector<int> _prev;
  int* _head;
  enum invalid_{invalid = 0};
public:
  class bucket{
    int _id;
    int* _next;
    int* _prev;
    int* _head;
  public:
    bucket(int id, int* next, int* prev, int* h)
      : _id(id), _next(next), _prev(prev), _head(h) { }
    bool empty()const {return _head[_id] == invalid;}
    int top()const {
      assert(_head[_id] != invalid);
      return _head[_id];
    }
    void push(int x) {
      assert(x != invalid);
      _prev[x] = _id + int(_head - _next);
      _next[x] = _head[_id];
      _head[_id] = x;
    }
    void pop() {
      int current = _head[_id];
      assert(current != invalid);
      int next = _next[current];
      _head[_id] = next;
      _prev[next] = _id + int(_head - _next);
    }
  };
public:
  BUCKETS(BUCKETS const&) = delete;
  explicit BUCKETS(int n) :
    _values(n, invalid),
    _next(2*n, invalid),
    _prev(n, invalid),
    _head(_next.data() + n) {
  }
  ~BUCKETS() {
    _values.clear();
    _next.clear();
    _prev.clear();
    _head = nullptr;
  }
 // const_bucket operator[](const int& i) const ...
  bucket operator[](const int& i) {
    assert(i < int(_next.size()));
    return bucket(i, _head, &_next[0], &_prev[0]);
  }

  void push(int a, int b) {
    _values[a] = b;
    (*this)[b].push(a);
  }
  void update(int a) { untested();
    int val = _values[a];
    _prev[a] = val + int(_head - _next.data());
    _next[a] = _head[val];
    _head[val] = a;
  }
};
/*--------------------------------------------------------------------------*/
template<class container_t, class set_t>
void build_ccidx(container_t const& rows, container_t const& cols,
    set_t const& inodes,
    int const* raw_idx, int const* raw_didx, int*& _ap, int*& _ai)
{
  assert(!_ap);
  assert(!_ai);

  int n = int(cols.size()+1);
  int nz = count_nz(cols)
         + count_nz(rows)
	 + n - int(inodes.size());
  _ap = new int32_t[n+1];
  _ai = new int32_t[nz];
  _ap[0] = 0;
  _ai[0] = 0;
  _ap[1] = 1;

  int* rf = new int32_t[n+1];
  BUCKETS cb(n+1);

  for(int r=1; r<n; ++r){
    rf[r-1] = raw_didx[r-1] - 1;
    if(int c = raw_idx[rf[r-1]]) {
      // insert r into bucket c;
      cb.push(r, c);
      assert(!cb[c].empty());
    }else{
    }
  }

  int seek = 1;
  auto inodeit = inodes.begin();
  int32_t* sp = _ap + 1;
  for(int c=1; c<n; ++c) {
    int colstart = seek;
    // upper
    while(!cb[c].empty()){
      int r = cb[c].top();
      assert(c>r);
      _ai[seek++] = r;
      cb[c].pop();

      --rf[r-1];
      if(int cc = raw_idx[rf[r-1]]) {
	cb.push(r, cc);
      }else{
      }
    }

    // not strictly needed. just keep order for now.
    std::sort(_ai+colstart, _ai+seek);

    if(inodeit == inodes.end()){
      _ai[seek++] = c;
    }else if(*inodeit != c){ untested();
      assert(*inodeit > c);
      _ai[seek++] = *inodeit;
    }else{ untested();
      ++inodeit;
    }

    // paste lower. already ordered.
    for(int cs=raw_didx[c-1]; raw_idx[cs]; ++cs) {
      _ai[seek++] = raw_idx[cs];
    }
    ++sp;
    *sp = seek;
  }

  assert(nz == _ap[n]);

  for(int col=0; col<n; ++col) {
    for(int id=_ap[col]; id<_ap[col+1]; ++id){
      int row = _ai[id];
      if(col==row){
	assert(!inodes.count(col));
      }else if(col<row){
	assert(cols[col-1].count(row));
	assert(!rows[row-1].count(col));
      }else{
	assert(rows[row-1].count(col));
	assert(!cols[col-1].count(row));
      }
    }
  }

  delete [] rf;
}
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class FOOTPRINT : public MATRIX_STAMP {
  typedef int value_type;
  typedef SMALL_SET<value_type> set_t;
  typedef std::vector<set_t> container_t;
  typedef container_t::const_iterator const_iterator;

private:
  int _nreq{0};
  int _nstamp{0};
  int* _raw_idx{nullptr};
  int* _raw_didx{nullptr};
  int* _ap{nullptr};
  int* _ai{nullptr};
  std::vector<int> _ulow;
  std::vector<int> _llow;
  int _numcomp{0};
public: // BUG
  container_t _rows;
  container_t _cols;
  set_t _inodes;
public:
  explicit FOOTPRINT() {}

  void iwant_point(int, int)override;
  void iwant_quad(int i, int j)override { iwant_point(i,j), iwant_point(j,i);}
  void iwant_inode(int i, int j)override { untested();
    if(i){ untested();
      iwant_quad(i,j);
    }else{ untested();
    }
    assert(j);
    _inodes.insert(j);
  }

  int size()const {
    int i = 0;
    for(auto d : _rows){
      i += int(d.size());
    }
    for(auto d : _cols){
      i += int(d.size());
    }
    return i;
  }

  void init(int s)override {
    assert(_cols.empty());
    assert(_rows.empty());
    _cols.resize(s);
    _rows.resize(s);
    assert(!size());

    if(_ulow.size()){
    }else{
    }

    _ulow.resize(s+1);
    _llow.resize(s+1);
    for (int ii = 0;  ii <= s;  ++ii) {
      _ulow[ii] = _llow[ii] = ii;
    }
    _nreq = 0;
  }
  void allocate() {
    if(_raw_idx){
      // keep it
    }else{
      _nstamp = serialize_bs_adj(cols(), rows(), _raw_idx, _raw_didx);
      build_ccidx_co();
    }
  }
  void build_ccidx_co() {
    build_ccidx(rows(), cols(), _inodes, _raw_idx, _raw_didx, _ap, _ai);
  }
  void unallocate() {
  }
  void uninit()override {
    delete _raw_idx;
    delete _raw_didx;
    _raw_idx = _raw_didx = nullptr;

    _cols.clear();
    _rows.clear();
  }

  container_t const& cols()const {return _cols;}
  container_t const& rows()const {return _rows;}
private: // BSMATRIX_DATA
  int const* ulownode()const override {return _ulow.data();}
  int const* llownode()const override {return _llow.data();}
public: // layout
  int const* ap()const {return _ap;}
  int const* ai()const {return _ai;}
};
/*--------------------------------------------------------------------------*/
inline void FOOTPRINT::iwant_point(int i, int j)
{
  assert(i);
  assert(j);
  if(i<j){
    // upper
    assert(i-1 < int(_rows.size()));
    _rows[i-1].insert(j);
  }else if(j<i){
    // lower
    assert(j-1 < int(_cols.size()));
    _cols[j-1].insert(i);
  }else{
  }
  if(i!=j){
    ++_nreq;
  }else{
  }

// needed in BSMATRIX_DATA
  if (i <= 0  ||  j <= 0) { untested();
    // node 0 is ground, and doesn't count as a connection
    // negative is invalid, not used but still may be in a node list
  }else if (i < _ulow[j]) {
    _ulow[j] = i;
  }else if (j < _llow[i]) {
    _llow[i] = j;
  }else{
  }

}
/*--------------------------------------------------------------------------*/
template<class T>
class KLU;
/*--------------------------------------------------------------------------*/
inline int klu_solve_ (
    klu_symbolic const* S, klu_numeric const* N,
    int n, int nrhs, double* ax, klu_common* C)
{
  klu_symbolic* mS = const_cast<klu_symbolic*>(S); // yikes
  klu_numeric* mN = const_cast<klu_numeric*>(N); // yikes
  return klu_solve (mS, mN, n, nrhs, ax, C);
}
/*--------------------------------------------------------------------------*/
inline int klu_solve_ (
    klu_symbolic const* S, klu_numeric const* N,
    int n, int nrhs, COMPLEX* ax_, klu_common* C)
{
  klu_symbolic* mS = const_cast<klu_symbolic*>(S); // yikes
  klu_numeric* mN = const_cast<klu_numeric*>(N); // yikes
  double* ax = reinterpret_cast<double*>(ax_);
  return klu_z_solve (mS, mN, n, nrhs, ax, C);
}
/*--------------------------------------------------------------------------*/
template<class T>
class KLU : public BSMATRIX_SOLVER<T> {
  BSMATRIX_DATA<T>& _aa;
  FOOTPRINT _fp;
  mutable klu_common _klu_common;
  klu_symbolic* _klu_symbolic{nullptr};
  klu_numeric* _klu_numeric{nullptr};
  T* _ax{nullptr};
  T _min_pivot{0.};
  mutable unsigned* _changed{nullptr};// flag: this node changed value

  int* _idx{nullptr};
  int* _didx{nullptr};

  int _nzcount{0};
public:
  using BSMATRIX_SOLVER<T>::size;
public:
  explicit KLU(BSMATRIX<T>& aa)
   : BSMATRIX_SOLVER<T>(aa) , _aa(BSMATRIX_SOLVER<T>::rw_data_(aa)) {
     BSMATRIX_SOLVER<T>::_data.set_stamp(&_fp);
     aa.set_stamp(&_fp); // receive iwant calls
  }

private: // KLU
  void set_stamp(MATRIX_STAMP* s)override { untested();
    assert(s);
    BSMATRIX_SOLVER<T>::set_stamp(s);
  }
  void init(int ss) override;
  void allocate()override;
  void unallocate()override;
  void uninit()override;

  void set_min_pivot(double x)override {_min_pivot = x;}
//  void zero() override;
  void lu_decomp(bool do_partial) override;
//  void lu_decomp() override{ untested();unreachable();}
  void load_diagonal_point(int i, T value)override;
  void load_point(int i, int j, T value)override;
  void load_couple(int i, int j, T value)override;
  void load_symmetric(int i, int j, T value)override;
  void load_asymmetric(int r1, int r2, int c1, int c2, T value)override;

  void set_changed(int, bool)const override{ untested();
    unreachable(); // obsolete
  }
  void set_changed(int i, int j)const {
    return; // not in use
    assert(_changed);
    assert(i>=0);
    assert(j>=0);
    if(i==0 || j==0){ untested();
      // reset.
      _changed[j] = i;
      _changed[i] = j;
    }else if(i<j){
      // upper
      _changed[j] = std::min(unsigned(i-1), _changed[j]-1)+1;
      _changed[i] = std::min(unsigned(i-1), _changed[i]-1)+1; // diag
    }else if(i>j){
      // lower
      _changed[i] = std::min(unsigned(j-1), _changed[i]-1)+1;
      _changed[j] = std::min(unsigned(j-1), _changed[j]-1)+1; // diag
    }else if(i==j){
      _changed[i] = std::min(unsigned(j-1), _changed[i]-1)+1;
    }else{ untested();
    }

    assert(i==0 || int(_changed[i]) <= i );
    assert(j==0 || int(_changed[j]) <= j );

  }

private:
  void lu_iwant(int i, int j);

private: // aa data xs
  T& aam(int i, int j){ return BSMATRIX_SOLVER<T>::m_(_aa, i, j); }
  T& aal(int i, int j){ untested(); return BSMATRIX_SOLVER<T>::l_(_aa, i, j); }
  T& aau(int i, int j){ untested(); return BSMATRIX_SOLVER<T>::u_(_aa, i, j); }
  T& aad(int i       ){ return BSMATRIX_SOLVER<T>::d_(_aa, i); }

private:
  unsigned is_changed(int n)const { untested();
    return _changed[n];
  }

private: // implementation
  void fbsub(T* v) const override{
    trace1("fbsub", size());
    int n = size()+1;
    klu_solve_ (_klu_symbolic, _klu_numeric, n, 1/*nrhs*/, v, &_klu_common);
  }
  void fbsub(T* x, const T* b, T* = nullptr) const override {
    if(x == b){ untested();
      return fbsub(x);
    }else{
      // .fbsub(_sim->_v0, _sim->_i, _sim->_v0);
      int n = size()+1;
      memcpy(x, b, n*sizeof(T));
      x[0] = 0.;
      fbsub(x);
    }
    // return _lu.fbsub(x, b, c);
  }
  void fbsubt(T*)const override { untested(); incomplete(); }

private:
  T lu_tag() const{ untested();
    return -1.;
  }
  union one{
    one(){ untested();
      t = 0.;
      x[0] = 1;
    }
    T t;
    char x[sizeof(T)];

    operator T() const{ untested();return t;}
  };
  // T const& one() const;
  bool is_one(T const&) const;
  bool nz(T const& t) const{ untested(); return bool(t); }
  void propagate(int m);

public:
  int fpsize()const { untested(); return fp().size() + size() - ninode(); }
  int fpnumcomp()const { untested(); return fp().numcomp(); }
  int nnz()const { untested(); return _nzcount; }
  int ninode()const { untested(); return fp().ninode(); }
  FOOTPRINT const& fp()const { untested();return _fp;}
private:
  FOOTPRINT& fp() {return _fp;}
}; // KLU
/*--------------------------------------------------------------------------*/
template<class T>
void KLU<T>::init(int ss)
{
  _changed = new unsigned[ss+1];
  assert(_changed);
  std::fill_n(_changed, ss+1, 0);
//  assert(_zero == 0.);
}
/*--------------------------------------------------------------------------*/
template <class T>
void KLU<T>::allocate()
{
  fp().allocate();
  serialize_bs_adj(fp().cols(), fp().rows(), _idx, _didx);
  assert(size() == int(fp().cols().size()));
  int n = size() + 1;

  klu_defaults(&_klu_common);
  int* ap = const_cast<int32_t*>(fp().ap()); // yikes.
  int* ai = const_cast<int32_t*>(fp().ai()); // yikes.
  _klu_symbolic = klu_analyze (n, ap, ai, &_klu_common);
  _aa.zero();
  _ax = new T[ap[n]];
  _ax[0] = 1.;
}
/*--------------------------------------------------------------------------*/
template <class T>
void KLU<T>::unallocate()
{
  fp().unallocate();
  klu_free_symbolic (&_klu_symbolic, &_klu_common);
  _klu_symbolic = nullptr;

  klu_free_numeric (&_klu_numeric, &_klu_common);
  _klu_numeric = nullptr;

  delete [] _ax;
  _ax = nullptr;
}
/*--------------------------------------------------------------------------*/
template <class T>
void KLU<T>::uninit()
{
  unallocate();

  _fp.uninit();

  delete [] _changed;
  _changed = nullptr;
}
/*--------------------------------------------------------------------------*/
template <class T>
void KLU<T>::propagate(int m)
{ untested();
  int diag = _didx[m-1];
  int const* it = &_idx[diag];
  while(*(--it)){ untested();
    set_changed(m, *it);
  }
  it = &_idx[diag];
  while(*it){ untested();
    set_changed(*it, m);
    ++it;
  }
}
/*--------------------------------------------------------------------------*/
inline klu_numeric* klu_factor_ (
    int32_t const* ap_, int32_t const* ai_, double* ax,
    klu_symbolic* S, klu_common* C)
{
  int* ap = const_cast<int32_t*>(ap_); // yikes.
  int* ai = const_cast<int32_t*>(ai_); // yikes.
  return klu_factor (ap, ai, ax, S, C);
}
/*--------------------------------------------------------------------------*/
inline klu_numeric* klu_factor_ (
    int32_t const* ap_, int32_t const* ai_, COMPLEX* ax_,
    klu_symbolic* S, klu_common* C)
{
  int* ap = const_cast<int32_t*>(ap_); // yikes.
  int* ai = const_cast<int32_t*>(ai_); // yikes.
  double* ax = reinterpret_cast<double*>(ax_);
  return klu_z_factor (ap, ai, ax, S, C);
}
/*--------------------------------------------------------------------------*/
/* return TRUE if successful, FALSE otherwise */
int klu_refactor_ (
    int32_t const* ap_, int32_t const* ai_, double* ax,
    klu_symbolic const* S, klu_numeric *N, klu_common *C)
{
  int* ap = const_cast<int32_t*>(ap_); // yikes.
  int* ai = const_cast<int32_t*>(ai_); // yikes.
  klu_symbolic* mS = const_cast<klu_symbolic*>(S); // yikes
  return klu_refactor(ap, ai, ax, mS, N, C);
}
/*--------------------------------------------------------------------------*/
int klu_refactor_ (
    int32_t const* ap_, int32_t const* ai_, COMPLEX* ax_,
    klu_symbolic const* S, klu_numeric *N, klu_common *C)
{
  int* ap = const_cast<int32_t*>(ap_); // yikes.
  int* ai = const_cast<int32_t*>(ai_); // yikes.
  double* ax = reinterpret_cast<double*>(ax_);
  klu_symbolic* mS = const_cast<klu_symbolic*>(S); // yikes
  return klu_z_refactor(ap, ai, ax, mS, N, C);
}
/*--------------------------------------------------------------------------*/
template <class T>
void KLU<T>::lu_decomp(bool /*do_partial*/)
{
  int const* ap = fp().ap();
  int const* ai = fp().ai();
  assert(_ax[0] == 1.);
  int n = size();

  for(int col=1; col<=n; ++col) {
    for(int id=ap[col]; id<ap[col+1]; ++id){
      int row = ai[id];
      assert(id);
      _ax[id] = aam(row, col);
      trace3("grab", row, col, _ax[id]);
    }
  }

  if(_klu_numeric){
    klu_refactor_ (ap, ai, _ax, _klu_symbolic, _klu_numeric, &_klu_common);
  }else{
    _klu_numeric = klu_factor_ (ap, ai, _ax, _klu_symbolic, &_klu_common);
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
void KLU<T>::load_point(int i, int j, T value)
{ untested();
  if (i > 0 && j > 0) { untested();
    set_changed(i, j);
    aam(i,j) += value;
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value);
template <class T>
void KLU<T>::load_diagonal_point(int i, T value)
{
  if (i > 0) {
    set_changed(i, i);
    aad(i) += value;
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void KLU<T>::load_couple(int i, int j, T value)
{untested();
  if (j > 0) {untested();
    if (i > 0) {untested();
      set_changed(i, j);
      aam(i,j) -= value;
      set_changed(j, i);
      aam(j,i) -= value;
    }else{untested();
    }
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(i, i, value); or load_diagonal_point(i, value);
// load_point(j, j, value); or load_diagonal_point(j, value);
// load_point(i, j, -value);
// load_point(j, i, -value);
template <class T>
void KLU<T>::load_symmetric(int i, int j, T value)
{
  if (j > 0) {
    set_changed(j, j);
    aad(j) += value;
    if (i > 0) {
      set_changed(i,i);
      set_changed(i,j);
      set_changed(j,i);
      aad(i) += value;
      aam(i,j) -= value;
      aam(j,i) -= value;
    }else{
    }
  }else if (i > 0) {
    set_changed(i,i);
    aad(i) += value;
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
// load_point(r1, c1, value);
// load_point(r2, c2, value);
// load_point(r1, c2, -value);
// load_point(r2, c1, -value);
template <class T>
void KLU<T>::load_asymmetric(int r1,int r2,int c1,int c2,T value)
{
  if (r1 > 0) {
    if (c1 > 0) {
      aam(r1,c1) += value;
      set_changed(r1, c1);
    }else{
    }
    if (c2 > 0) {
      aam(r1,c2) -= value;
      set_changed(r1, c2);
    }else{
    }
  }else{untested();
  }
  if (r2 > 0) {
    if (c1 > 0) {
      set_changed(r2, c1);
      aam(r2,c1) -= value;
    }else{untested();
    }
    if (c2 > 0) {
      set_changed(r2, c2);
      aam(r2,c2) += value;
    }else{untested();
    }
  }else{
  }
}
/*--------------------------------------------------------------------------*/
KLU<double>* m;
KLU<COMPLEX>* mm;
struct set{
  set(){
    m = new KLU<double>(CKT_BASE::_sim->_aa);
    mm = new KLU<COMPLEX>(CKT_BASE::_sim->_acx);
    CKT_BASE::_sim->_aa.set_solver(m);
    CKT_BASE::_sim->_acx.set_solver(mm);
  }
  ~set(){
    CKT_BASE::_sim->_aa.set_solver(nullptr);
    CKT_BASE::_sim->_acx.set_solver(nullptr);
  }
} s;
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
