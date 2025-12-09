bin_PROGRAMS = tceopgen tceriscvopgen tceoclextgen
tceopgen_SOURCES = tceopgen.cc

tceriscvopgen_SOURCES = tceriscvopgen.cc
tceriscvopgen_LDADD = ../../../libopenasip.la


tceoclextgen_SOURCES = ExtensionGen.cc
tceoclextgen_LDADD = ../../../libopenasip.la

LIB_BASE_DIR = ../../../base
LIB_TCETOOLS_DIR = ../../../tools
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach

SRC_ROOT_DIR = $(top_srcdir)/src

OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
BEM_DIR = ${SRC_ROOT_DIR}/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
MACH_DIR = ${SRC_ROOT_DIR}/base/mach


tceopgen_LDADD = ../../../libopenasip.la


AM_CPPFLAGS = -I${OSAL_DIR} -I${TOOLS_DIR} -I${MACH_DIR} -I${BEM_DIR} -I${BEM_APPLIBS_DIR}
AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
