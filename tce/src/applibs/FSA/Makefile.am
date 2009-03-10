noinst_LTLIBRARIES = libfsa.la
libfsa_la_SOURCES = FiniteStateAutomaton.cc FUFiniteStateAutomaton.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
MACH_DIR = ${BASE_DIR}/mach
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
APPLIBS_MACH_DIR = ${SRC_ROOT_DIR}/applibs/mach

AM_CPPFLAGS = -I${MACH_DIR} -I${BASE_DIR} -I${TOOLS_DIR} -I${APPLIBS_MACH_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
