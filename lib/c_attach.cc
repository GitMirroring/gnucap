/*$Id: c_attach.cc $ -*- C++ -*-
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
 *------------------------------------------------------------------
 */
//testing=script 2023.12.02
#include "e_cardlist.h"
#include "c_comand.h"
#include "globals.h"
#include "io_dir.h"
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
std::map<std::string, void*> attach_list;
const std::string SUFFIX(".so");
/*--------------------------------------------------------------------------*/
std::string plug_path()
{ untested();
  return OS::getenv("GNUCAP_PLUGPATH");
}  
/*--------------------------------------------------------------------------*/
bool ends_with (std::string const& str, std::string const &sfx)
{ untested();
  if (sfx.length() <= str.length()) { untested();
    return !str.compare(str.length() - sfx.length(), sfx.length(), sfx);
  } else { untested();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
void list()
{ untested();
  for (std::map<std::string, void*>::iterator
	 ii = attach_list.begin(); ii != attach_list.end(); ++ii) { untested();
    if (ii->second) { untested();
      IO::mstdout << ii->first << '\n';
    }else{untested();
      error(bTRACE,  ii->first + " (unloaded)\n");
    }
  }
}
/*--------------------------------------------------------------------------*/
void tach_path(CS&, std::string const&, CARD_LIST const*, int, size_t);
void tach_dir(CS&, std::string const&, DIRECTORY const&,
              CARD_LIST const*, int, size_t);
void tach_index(CS&, std::string const&, CARD_LIST const*, int, size_t);
void tach_many(CS&, std::string const&, CARD_LIST const*, int, size_t);
void tach_plugin(CS&, std::string const& file_name, CARD_LIST const*, int, size_t);

void attach_file(CS&, std::string const& file_name, CARD_LIST const*, int, size_t);
void detach_file(CS&, std::string const& file_name, CARD_LIST const*, size_t);
/*--------------------------------------------------------------------------*/
void load_or_unload(CS& cmd, CARD_LIST const* Scope, int flags)
{ untested();
  size_t here = cmd.cursor();
  std::string short_file_name;
  cmd >> short_file_name;
  if (short_file_name == "") { untested();
    // nothing, list what we have
    list();
  }else{ untested();
    std::string full_file_name;
    if (short_file_name[0]=='/' || short_file_name[0]=='.'){itested();
      if (OS::access_ok(short_file_name, R_OK)) {itested();
	// found it, local or root
	full_file_name = short_file_name;
      }else{untested();
	cmd.reset(here);
	throw Exception_CS(std::string("plugin not found in ") + short_file_name[0], cmd);
      }
    }else{ untested();
      std::string path = plug_path();
      full_file_name = findfile(short_file_name, path, R_OK);
      if (full_file_name != "") { untested();
	// found it, with search
      }else{untested();
	cmd.reset(here);
	throw Exception_CS("plugin not found in " + path, cmd);
      }
    }

    tach_path(cmd, full_file_name, Scope, flags, here);
  }
}
/*--------------------------------------------------------------------------*/

class CMD_ATTACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override { untested();
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) { untested();
    }else{untested();
    }
    size_t here = cmd.cursor();
    int dl_scope = RTLD_LOCAL;
    int check = RTLD_NOW;
    // RTLD_NOW means to resolve symbols on loading
    // RTLD_LOCAL means symbols defined in a plugin are local
    do { untested();
      if (cmd.umatch("public ")) {itested();
	dl_scope = RTLD_GLOBAL;
	// RTLD_GLOBAL means symbols defined in a plugin are global
	// Use this when a plugin depends on another.
      }else if (cmd.umatch("lazy ")) {untested();
	check = RTLD_LAZY;
	// RTLD_LAZY means to defer resolving symbols until needed
	// Use when a plugin will not load because of unresolved symbols,
	// but it may work without it.
      }else{ untested();
      }
    } while (cmd.more() && !cmd.stuck(&here));

    load_or_unload(cmd, Scope, check | dl_scope);
  }

  std::string help_text()const override { untested();
    return 
      "load command\n"
      "Loads plugins\n"
      "Syntax: load plugin\n"
      "Plugin search path is: " + plug_path() + " \n"
      "Path is set by GNUCAP_PLUGPATH environment variable\n"
      "With no arg, it lists plugins already loaded\n\n";
  }
} p1;
DISPATCHER<CMD>::INSTALL d1(&command_dispatcher, "attach|load", &p1);
/*--------------------------------------------------------------------------*/
void tach_plugin(CS& cmd, std::string const& fn, CARD_LIST const* Scope,
               int flags, size_t here)
{ untested();
  if(flags){itested();
    attach_file(cmd, fn, Scope, flags, here);
  }else{ itested();
    detach_file(cmd, fn, Scope, here);
  }
}
/*--------------------------------------------------------------------------*/
void tach_path(CS& cmd, std::string const& path, CARD_LIST const* Scope,
               int flags, size_t here)
{ untested();
  if(!ends_with(path, SUFFIX)) { untested();
    tach_many(cmd, path, Scope, flags, here);
  }else{ untested();
    tach_plugin(cmd, path, Scope, flags, here);
  }
}
/*--------------------------------------------------------------------------*/
void tach_many(CS& cmd, std::string const& path, CARD_LIST const* Scope,
               int flags, size_t here)
{ untested();
  DIRECTORY dir(path);

  if(dir.exists()) {itested();
    tach_dir(cmd, path, dir, Scope, flags, here);
  }else{ untested();
    tach_index(cmd, path, Scope, flags, here);
  }
}
/*--------------------------------------------------------------------------*/
void tach_dir(CS& cmd, std::string const& dirname, DIRECTORY const& dir,
              CARD_LIST const* Scope, int flags, size_t here)
{itested();
  DIRECTORY::const_iterator i;
  std::vector<std::string> sos;
  for(i=dir.begin(); i!=dir.end(); ++i) {itested();
    std::string fname = i->d_name;
    size_t s = fname.size();
    size_t slen = SUFFIX.size();
    if(s<slen){itested();
    }else if(fname.substr(s-slen, s) == SUFFIX){itested();
      sos.push_back(fname);
    }else{itested();
    }
  }
  std::sort(sos.begin(), sos.end());
  std::vector<std::string>::const_iterator ni;
  error(bLOG, "Plugins in " + dirname + "\n");
  for(ni=sos.begin(); ni!=sos.end(); ++ni) {itested();
    error(bLOG, " .. " + *ni + "\n");
    // how about recursive load? //
    tach_plugin(cmd, dirname + "/" + *ni, Scope, flags, here);
  }
}
/*--------------------------------------------------------------------------*/
void tach_index(CS& cmd, std::string const& fname, CARD_LIST const* Scope,
                int flags, size_t here)
{ untested();
  CS idx(CS::_INC_FILE, fname);
  std::string::size_type sepplace = fname.find_last_of("/");
  std::string path = fname.substr(0, sepplace+1);

  idx.get_line("");
  if(idx >> "#"){ untested();
    error(bLOG, "Loading " + idx.tail() + "\n");

    try { untested();
      for (;;) { untested();
	idx.get_line("");
	std::string line;
	idx >> line;
	std::string new_path = path + line;
	tach_path(cmd, new_path, Scope, flags, here);
      }
    }catch (Exception_End_Of_Input& e) { untested();
    }

  }else{ untested();
    throw Exception_CS(fname + ": bad index file", cmd);
  }

}
/*--------------------------------------------------------------------------*/
void attach_file(CS& cmd, std::string const& file_name,
                             CARD_LIST const* Scope, int flags,
                             size_t here)
{ untested();
  // a name to look for
  // check if already loaded
  assert(flags);
  if (void* handle = attach_list[file_name]) { untested();
    if (Scope->is_empty()) { untested();
      cmd.warn(bDANGER, here, "\"" + file_name + "\": already loaded, replacing");
      dlclose(handle);
      attach_list[file_name] = nullptr;
    }else{untested();
      cmd.reset(here);
      throw Exception_CS("already loaded, cannot replace when there is a circuit", cmd);
    }
  }else{ untested();
  }

  assert(OS::access_ok(file_name, R_OK));

  if (!flags) { untested();
  }else if (void* handle = dlopen(file_name.c_str(), flags)) { untested();
    attach_list[file_name] = handle;
  }else{untested();
    throw Exception_CS(dlerror(), cmd);
  }
}
/*--------------------------------------------------------------------------*/
void detach_file(CS& cmd, std::string const& file_name,
                 CARD_LIST const* Scope, size_t here)
{itested();
  if (Scope->is_empty()) {itested();
    void* handle = attach_list[file_name];
    if (handle) {itested();
      dlclose(handle);
      attach_list[file_name] = nullptr;
    }else{untested();
      cmd.reset(here);
      throw Exception_CS("plugin not attached", cmd);
    }
  }else{untested();
    cmd.reset(here);
    throw Exception_CS("detach prohibited when there is a circuit", cmd);
  }
}
/*--------------------------------------------------------------------------*/
class CMD_DETACH : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope) override { untested();
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) { untested();
    }else{untested();
    }
    //BUG// due to the way dlopen and dlclose work
    // it doesn't really work.
    // the dispatcher's active instance blocks unload

    load_or_unload(cmd, Scope, 0);
  }

  std::string help_text()const override { untested();
    return 
      "unload command\n"
      "Unloads plugins\n"
      "Syntax: unload plugin\n"
      "The name must match the name you loaded it with.\n"
      "Prohibited when there is a circuit\n"
      "With no arg, it lists plugins already loaded\n\n";
  }
} p2;
DISPATCHER<CMD>::INSTALL d2(&command_dispatcher, "detach|unload", &p2);
/*--------------------------------------------------------------------------*/
class CMD_DETACH_ALL : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST* Scope)override { untested();
    assert(Scope);
    if (Scope == &CARD_LIST::card_list) { untested();
    }else{untested();
    }
    if (Scope->is_empty()) { untested();
      for (std::map<std::string, void*>::iterator
	     ii = attach_list.begin(); ii != attach_list.end(); ++ii) { untested();
	void* handle = ii->second;
	if (handle) { untested();
	  dlclose(handle);
	  ii->second = nullptr;
	}else{itested();
	  // name still in list, but has been detached already
	}
      }
    }else{untested();
      throw Exception_CS("detach prohibited when there is a circuit", cmd);
    }
  }
} p3;
DISPATCHER<CMD>::INSTALL d3(&command_dispatcher, "detach_all", &p3);
/*--------------------------------------------------------------------------*/
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
