bin_PROGRAMS = tceplugingen

tceplugingen_SOURCES = tceplugingen.cc
tceplugingen_LDADD =

BASE_DIR = ../../../base
TOOLS_DIR = ../../../tools
MACH_DIR = ../../../base/mach
MACH_APPLIBS_DIR = ../../../applibs/mach
OSAL_DIR = ../../../base/osal
OSAL_APPLIBS_DIR = ../../../applibs/osal
GRAPH_DIR = ../../../base/Graph

AM_CPPFLAGS = ${LLVM_CPPFLAGS} -I${TOOLS_DIR} -I${MACH_DIR} \
-I${OSAL_DIR} -I${MACH_APPLIBS_DIR} -I${OSAL_APPLIBS_DIR} \
-I${GRAPH_DIR}

AM_LDFLAGS = ${TOOLS_DIR}/libtcetools.la ${BASE_DIR}/libttabase.la \
	   ${MACH_APPLIBS_DIR}/libapplibsmach.la \
	   ${OSAL_APPLIBS_DIR}/libapplibsosal.la
	   

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
