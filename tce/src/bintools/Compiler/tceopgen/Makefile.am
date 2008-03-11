bin_PROGRAMS = tceopgen
tceopgen_SOURCES = tceopgen.cc

OSAL_DIR = ../../../base/osal
LIB_TCETOOLS_DIR = ../../../tools
LIB_BASE_DIR = ../../../base
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach

tceopgen_LDADD = ${LIB_BASE_DIR}/libttabase.la \
${LIB_TCETOOLS_DIR}/libtcetools.la ${LIB_APPLIBS_MACH_DIR}/libapplibsmach.la

AM_CPPFLAGS = -I${OSAL_DIR} -I${LIB_TCETOOLS_DIR}
AM_LDFLAGS =

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
