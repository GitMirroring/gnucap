GNUCAP_CONF = $(shell which gnucap-conf$(SUFFIX))

include Make.override

ifneq ($(GNUCAP_CONF),)
    CXX = $(shell $(GNUCAP_CONF) --cxx)
    GNUCAP_CPPFLAGS = $(shell $(GNUCAP_CONF) --cppflags) -DADD_VERSION -DPIC
    GNUCAP_CXXFLAGS = $(shell $(GNUCAP_CONF) --cxxflags)
	 GNUCAP_LIBDIR   = $(shell $(GNUCAP_CONF) --pkglibdir)
else
    $(info no gnucap-conf, this will not work)
endif

GNUCAP_CXXFLAGS+= -fPIC -shared

INSTALL_FILES = out_hdf.so
CLEANFILES = *.so *.o *~
OBJS = out_hdf.o s__out.o out__cmd.o

all: $(INSTALL_FILES)

LIBS=-lhdf5_serial -lhdf5_cpp

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $(GNUCAP_CXXFLAGS) $(CPPFLAGS) $(GNUCAP_CPPFLAGS) -o $@ $<

out_hdf.so : ${OBJS}
	$(CXX) $(CXXFLAGS) $(GNUCAP_CXXFLAGS) $(CPPFLAGS) $(GNUCAP_CPPFLAGS) -o $@ $+ $(LIBS)

install : $(INSTALL_FILES)
	-install -d $(GNUCAP_LIBDIR)
	install $(INSTALL_FILES) $(GNUCAP_LIBDIR)

uninstall :
	(cd $(GNUCAP_LIBDIR) ; rm $(INSTALL_FILES))

clean:
	rm -f $(CLEANFILES)

distclean: clean
	rm Make.override

Make.override:
	[ -e $@ ] || echo "# here you may override settings" > $@
