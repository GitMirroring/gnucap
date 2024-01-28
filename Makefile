

gnucap:
	#(cd conf; ${MAKE} -k)
	#(cd include; ${MAKE} -k)
	(cd lib; ${MAKE} -k)
	(cd modelgen; ${MAKE} -k)
	(cd main; ${MAKE} -k)
	(cd apps; ${MAKE} -k)
	(cd mgsrc; ${MAKE} -k)
	(cd mgsim; ${MAKE} -k)
	(cd mgvams; ${MAKE} -k)

debug:
	#(cd conf; ${MAKE} debug)
	#(cd include; ${MAKE} debug)
	(cd lib; ${MAKE} debug)
	(cd modelgen; ${MAKE} debug)
	(cd main; ${MAKE} debug)
	(cd apps; ${MAKE} debug)
	(cd mgsrc; ${MAKE} debug)
	(cd mgsim; ${MAKE} debug)
	(cd mgvams; ${MAKE} debug)

g++:
	#(cd conf; ${MAKE} g++)
	#(cd include; ${MAKE} g++)
	(cd lib; ${MAKE} g++)
	(cd modelgen; ${MAKE} g++)
	(cd main; ${MAKE} g++)
	(cd apps; ${MAKE} g++)
	(cd mgsrc; ${MAKE} g++)
	(cd mgsim; ${MAKE} g++)
	(cd mgvams; ${MAKE} g++)

clean:
	#(cd conf; ${MAKE} clean)
	#(cd include; ${MAKE} clean)
	(cd lib; ${MAKE} clean)
	(cd modelgen; ${MAKE} clean)
	(cd main; ${MAKE} clean)
	(cd apps; ${MAKE} clean)
	(cd mgsrc; ${MAKE} clean)
	(cd mgsim; ${MAKE} clean)
	(cd mgvams; ${MAKE} clean)
	-rm *~ \#*\#

depend:
	#(cd conf; ${MAKE} depend)
	#(cd include; ${MAKE} depend)
	(cd lib; ${MAKE} depend)
	(cd modelgen; ${MAKE} depend)
	(cd main; ${MAKE} depend)
	(cd apps; ${MAKE} depend)
	(cd mgsrc; ${MAKE} depend)
	(cd mgsim; ${MAKE} depend)
	(cd mgvams; ${MAKE} depend)

tags:
	#(cd conf; ${MAKE} tags)
	#(cd include; ${MAKE} tags)
	(cd lib; ${MAKE} tags)
	(cd modelgen; ${MAKE} tags)
	(cd main; ${MAKE} tags)
	(cd apps; ${MAKE} tags)
	(cd mgsrc; ${MAKE} tags)
	(cd mgsim; ${MAKE} tags)
	(cd mgvams; ${MAKE} tags)

unconfig:
	#(cd conf; ${MAKE} unconfig)
	#(cd include; ${MAKE} unconfig)
	(cd lib; ${MAKE} unconfig)
	(cd modelgen; ${MAKE} unconfig)
	(cd main; ${MAKE} unconfig)
	(cd apps; ${MAKE} unconfig)
	(cd mgsrc; ${MAKE} unconfig)
	(cd mgsim; ${MAKE} unconfig)
	(cd mgvams; ${MAKE} unconfig)

install:
	(cd conf; ${MAKE} install)
	(cd include; ${MAKE} install)
	(cd lib; ${MAKE} install)
	(cd modelgen; ${MAKE} install)
	(cd main; ${MAKE} install)
	(cd apps; ${MAKE} install)
	(cd mgsrc; ${MAKE} install)
	(cd mgsim; ${MAKE} install)
	(cd mgvams; ${MAKE} install)

install-debug:
	(cd conf; ${MAKE} install-debug)
	(cd include; ${MAKE} install-debug)
	(cd lib; ${MAKE} install-debug)
	(cd modelgen; ${MAKE} install-debug)
	(cd main; ${MAKE} install-debug)
	(cd apps; ${MAKE} install-debug)
	(cd mgsrc; ${MAKE} install-debug)
	(cd mgsim; ${MAKE} install-debug)
	(cd mgvams; ${MAKE} install-debug)

uninstall:
	(cd conf; ${MAKE} uninstall)
	(cd include; ${MAKE} uninstall)
	(cd lib; ${MAKE} uninstall)
	(cd modelgen; ${MAKE} uninstall)
	(cd main; ${MAKE} uninstall)
	(cd apps; ${MAKE} uninstall)
	(cd mgsrc; ${MAKE} uninstall)
	(cd mgsim; ${MAKE} uninstall)
	(cd mgvams; ${MAKE} uninstall)

manifest:
	(cd conf; ${MAKE} manifest)
	(cd include; ${MAKE} manifest)
	(cd lib; ${MAKE} manifest)
	(cd modelgen; ${MAKE} manifest)
	(cd main; ${MAKE} manifest)
	(cd apps; ${MAKE} manifest)
	(cd mgsrc; ${MAKE} manifest)
	(cd mgsim; ${MAKE} manifest)
	(cd mgvams; ${MAKE} manifest)

header-check:
	(cd conf; ${MAKE} header-check)
	(cd include; ${MAKE} header-check)
	(cd lib; ${MAKE} header-check)
	(cd modelgen; ${MAKE} header-check)
	(cd main; ${MAKE} header-check)
	(cd apps; ${MAKE} header-check)
	(cd mgsrc; ${MAKE} header-check)
	(cd mgsim; ${MAKE} header-check)
	(cd mgvams; ${MAKE} header-check)

date:
	(cd include; ${MAKE} date)

checkin:
	$(MAKE) date
	-git commit -a
