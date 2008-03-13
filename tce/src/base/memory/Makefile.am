
noinst_LTLIBRARIES = libmemory.la
libmemory_la_SOURCES = Memory.cc IdealSRAM.cc DirectAccessMemory.cc

PROJECT_ROOT = ../../..
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools \
	          -I${PROJECT_ROOT}/src/applibs/Simulator
AM_CXXFLAGS = -UNDEBUG

include_HEADERS = Memory.hh Memory.icc

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
