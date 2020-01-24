/*                          -*- C++ -*-
 * Copyright (C) 2017-18 Felix Salfelder
 * Author: Felix Salfelder
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
 * send simulator output to hdf file
 */
#include "u_sim_data.h"
#include "c_comand.h"
#include "u_prblst.h"
#include "globals.h"
#include "m_wave.h"
#include "m_phase.h"
#include "u_out.h"
#include "declare.h"
#include "s__.h"

#include <hdf5/serial/H5Include.h>
#include <hdf5/serial/H5Cpp.h>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
SIM::~SIM()
{ untested();
  //assert(!_scope);
  if (_sim) { untested();
    _sim->uninit();
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
bool plotset; // hack
OMSTREAM plotout; // hack
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
using H5::DataSet;
using H5::DataSpace;
static const unsigned RANK=2;
static const auto datatype=H5::PredType::NATIVE_DOUBLE;
/*--------------------------------------------------------------------------*/
static hsize_t numrows(DataSet const& d)
{ untested();
  DataSpace fspace = d.getSpace ();
  hsize_t qdims[2];
  unsigned dd=fspace.getSimpleExtentDims(qdims, NULL);
  assert(dd==2);
  return qdims[0];
}
/*--------------------------------------------------------------------------*/
static hsize_t numcols(DataSet const& d)
{ untested();
  DataSpace fspace = d.getSpace ();
  hsize_t qdims[2];
  unsigned dd=fspace.getSimpleExtentDims(qdims, NULL);
  assert(dd==2);
  return qdims[1];
}
/*--------------------------------------------------------------------------*/
class HDF_WRITER {
public:
  HDF_WRITER(){ }
  HDF_WRITER(std::string const& fn);
  HDF_WRITER(std::string const&, std::vector<std::string> const& header){ untested();
    hsize_t numcols( header.size() );
    ++numcols;

    const H5std_string FILE_NAME("file.h5");
    H5::H5File file( FILE_NAME, H5F_ACC_TRUNC );
    const H5std_string DATASET_NAME( "ExtendibleArray" );

    trace2("preparing", FILE_NAME, numcols);

    untested();

      hsize_t      dims[2]  = { 0, numcols};  // dataset dimensions at creation
      hsize_t      maxdims[2] = {H5S_UNLIMITED, numcols};
      DataSpace mspace1( RANK, dims, maxdims);

      H5::DSetCreatPropList cparms;
      /// cparms.setChunk( RANK, chunk_dims );
      double fill_val = 0;
      /// cparms.setFillValue( datatype, &fill_val);
      // double fill_val = 0.;
      cparms.setFillValue( datatype, &fill_val);

      hsize_t      chunk_dims[2] ={2, numcols};
      cparms.setChunk( RANK, chunk_dims );

      _dataset = file.createDataSet( DATASET_NAME, datatype, mspace1, cparms);
      trace1("created", numrows(_dataset));

    { untested();
      hsize_t dims[1];
      H5::StrType str_type(H5::PredType::C_S1, H5T_VARIABLE);
      dims[0] = header.size();
      DataSpace att_datspc(1, dims);
      H5::Attribute att(_dataset.createAttribute("Column_Names" , str_type, att_datspc));
      std::vector<const char *> cStrArray;
      for(unsigned index = 0; index < header.size(); ++index)
      { untested();
	cStrArray.push_back(header[index].c_str());
      }
      att.write(str_type, (void*)&cStrArray[0]);
    }
  }

  void push(double data);
  void push_key(double key);
private:
  DataSet _dataset;
}; // HDF_WRITER
/*--------------------------------------------------------------------------*/
void HDF_WRITER::push(double data)
{ untested();
  hsize_t numprobes=numcols(_dataset);
  hsize_t newlineoffset=numrows(_dataset)-1;
  trace2("push", newlineoffset, numprobes);
  static hsize_t col;
  ++col;
  if(col==numprobes){ untested();
    col=1;
  }

  hsize_t dims2[2];
  dims2[0] = 1;
  dims2[1] = 1;

  DataSpace fspace = _dataset.getSpace ();

  hsize_t      offset[2];
  offset[0] = newlineoffset;
  offset[1] = col;
  fspace.selectHyperslab( H5S_SELECT_SET, dims2, offset );
  DataSpace mspace( RANK, dims2 );

  _dataset.write( &data, datatype, mspace, fspace );
  trace1("", _dataset.getStorageSize());
}
/*--------------------------------------------------------------------------*/
void HDF_WRITER::push_key(double key)
{ untested();
  hsize_t newlineoffset=numrows(_dataset);
  hsize_t numprobes=numcols(_dataset);
  trace2("push_key", newlineoffset, numprobes);

  hsize_t dims2[2];
  dims2[0] = 1;
  dims2[1] = 1;

  hsize_t size[2];
  size[0]   = newlineoffset+1;
  size[1]   = numprobes;

  _dataset.extend( size );

  DataSpace fspace = _dataset.getSpace ();

  hsize_t offset[2];
  offset[0] = newlineoffset;
  offset[1] = 0;
  fspace.selectHyperslab( H5S_SELECT_SET, dims2, offset );
  DataSpace mspace( RANK, dims2 );

  _dataset.write( &key, datatype, mspace, fspace );
  trace1("", _dataset.getStorageSize());
}
/*--------------------------------------------------------------------------*/
class INTERFACE CMD_HDF : public OUTPUT_CMD {
private:
  CMD_HDF(const CMD_HDF& o)
   :OUTPUT_CMD(o),
    _threshold(o._threshold)
  { untested();
  }
public:
  typedef std::map<CMD_HDF*, std::string> name_map;
  typedef name_map::const_iterator const_iterator;
public:
  CMD_HDF() : _threshold(dl_ACCEPTED) {}
  CMD_HDF* clone() const{ untested();
    return new CMD_HDF(*this);
  }
  ~CMD_HDF(){ untested();
    clear_probelists();
    // detach_sinks();
  }
public:
//  PROBELIST& probelist(CMD_HDF* x);
  static const_iterator begin(){ return _n.begin(); }
  static const_iterator end(){ return _n.end(); }

public:
  void setup(CS&);
private: // OUTPUT
  void commit(double X, int);
  void head(double, double, const std::string&);
  void flush(){ }
public:
  PROBELIST const* probes(std::string mode) const{ untested();
    auto p=_map.find(mode);
    if(p==_map.end()){ untested();
      return NULL;
    }else{ untested();
      return p->second;
    }
  }
  PROBELIST* probes(std::string mode){ untested();
    return _map[mode];
  }
private:
  void clear_probelists(){ untested();
    for(auto& i : _map){ untested();
      if(i.second){ untested();
	i.second->clear();
      }else{ untested();
      }
    }
  }
#if 0
  void detach_sinks(){ untested();
    for(auto& i : _sinks){ untested();
      if(SIM* sim=dynamic_cast<SIM*>(i.first)){ untested();
	sim->detach_output(*i.second);
      }else{ untested();
	unreachable();
      }
      delete i.second;
      i.second = NULL;
    }
  }
#endif
private:
  std::map<std::string /*simtype*/, PROBELIST*> _map;
  std::map<CMD*, CMD_HDF*> _sinks;
  static name_map _n;
protected:
  int flags() const{return _threshold;}
private:
  const int _threshold;
  std::string _key; // label?
public:
  CMD_HDF(std::string key);
public: // CMD
  void do_it(CS& cmd, CARD_LIST*);

  // CMD_HDF::
  void do_head(double, double, const std::string&, PROBELIST const&){ untested();
  }
  void normal_head(double, double, const std::string& col1, PROBELIST const& pr)
  { untested();
    trace1("print head", col1);

    int width = std::min(OPT::numdgt+5, BIGBUFLEN-10);
    char format[20];
    //sprintf(format, "%%c%%-%u.%us", width, width);
    sprintf(format, "%%c%%-%us", width);

//    OUTPUT::_out.form(format, '#', col1.c_str());

    std::vector<std::string> header;

    for (PROBELIST::const_iterator p=pr.begin(); p!=pr.end(); ++p) { untested();
//      std::cout << (*p)->label().c_str();
      header.push_back((*p)->label());
    }

    out() << "dumping " << pr.size() << " into file.h5\n";
    _writer = HDF_WRITER("file.h5", header);

  }
  // CMD_HDF::
  void do_outdata(double x)
  { untested();
    PROBELIST& pr = probelist();
    trace1("print outdata", pr.size());

    assert(x != NOT_VALID);
    _writer.push_key(x);
    for (PROBELIST::const_iterator
	p=pr.begin(); p!=pr.end(); ++p) { untested();
      incomplete();
      _writer.push((*p)->value());
    }
  }
  void set_simname(const std::string& s){ untested();
    _simname = s;
  }
private:
  HDF_WRITER _writer;
  std::string _simname;
} p3;
/*--------------------------------------------------------------------------*/
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "hdfprobe", &p3);
/*--------------------------------------------------------------------------*/
CMD_HDF::name_map CMD_HDF::_n;
/*--------------------------------------------------------------------------*/
void CMD_HDF::setup(CS& cmd)
{ untested();
  IO::plotset = false;
  OUTPUT_CMD::setup(cmd);
}
/*--------------------------------------------------------------------------*/
void CMD_HDF::commit(double x, int flg)
{ untested();
  trace3("outdata", x, flg, flags());
  if(flg >= dl_ACCEPTED){ untested();
    do_outdata(x);
  }else{ untested();
  }
}
/*--------------------------------------------------------------------------*/
void CMD_HDF::head(double, double, const std::string& l)
{ untested();
  normal_head(0, 0, l, probelist());
}
/*--------------------------------------------------------------------------*/
void CMD_HDF::do_it(CS& cmd, CARD_LIST* scope)
{untested();
  OUTPUT_CMD::do_it(cmd, scope);
}
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
