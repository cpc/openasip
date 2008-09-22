
noinst_LTLIBRARIES = libidf.la
libidf_la_SOURCES = MachineImplementation.cc UnitImplementationLocation.cc \
IDFSerializer.cc NullUnitImplementationLocation.cc 

libidf_la_LDFLAGS = -lpthread

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: doc

MAINTAINERCLEAN = *~ *.gcov *.bbg *.bb *.da
