bin_PROGRAMS = tceopgen
tceopgen_SOURCES = tceopgen.cc

LIB_BASE_DIR = ../../../base
LIB_TCETOOLS_DIR = ../../../tools
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach

SRC_ROOT_DIR = $(top_srcdir)/src

OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

tceopgen_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_CPPFLAGS = -I${OSAL_DIR} -I${TOOLS_DIR}
AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	   ${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
