bin_PROGRAMS = tceopgen tceoclextgen
tceopgen_SOURCES = tceopgen.cc

tceoclextgen_SOURCES = ExtensionGen.cc
tceoclextgen_LDADD = ../../../libopenasip.la

LIB_BASE_DIR = ../../../base
LIB_TCETOOLS_DIR = ../../../tools
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach

SRC_ROOT_DIR = $(top_srcdir)/src

OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

tceopgen_LDADD = ../../../libopenasip.la

MACH_DIR = ${SRC_ROOT_DIR}/base/mach

AM_CPPFLAGS = -I${OSAL_DIR} -I${TOOLS_DIR} -I${MACH_DIR}
AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
