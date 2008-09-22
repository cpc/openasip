lib_LTLIBRARIES = libhdb.la
libhdb_la_SOURCES = HDBManager.cc FUEntry.cc RFEntry.cc FUArchitecture.cc \
FUExternalPort.cc FUImplementation.cc HWBlockImplementation.cc \
FUPortImplementation.cc RFPortImplementation.cc PortImplementation.cc \
RFImplementation.cc RFArchitecture.cc BlockImplementationFile.cc \
HDBRegistry.cc CostFunctionPlugin.cc HDBEntry.cc HWBlockArchitecture.cc \
CostEstimationData.cc CachedHDBManager.cc

PROJECT_ROOT = $(top_srcdir)

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach -I${PROJECT_ROOT}/src/applibs/mach

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile
