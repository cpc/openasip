bin_PROGRAMS = tceplugingen

BASE_DIR = ../../../base
TOOLS_DIR = ../../../tools
MACH_DIR = ../../../base/mach
MACH_APPLIBS_DIR = ../../../applibs/mach
OSAL_DIR = ../../../base/osal
OSAL_APPLIBS_DIR = ../../../applibs/osal
GRAPH_DIR = ../../../base/Graph


tceplugingen_SOURCES = tceplugingen.cc
tceplugingen_LDADD = ${TOOLS_DIR}/libtcetools.la ${BASE_DIR}/libttabase.la \
	   ${MACH_APPLIBS_DIR}/libapplibsmach.la \
	   ${OSAL_APPLIBS_DIR}/libapplibsosal.la


AM_CPPFLAGS = ${LLVM_CPPFLAGS} -I${TOOLS_DIR} -I${MACH_DIR} \
-I${OSAL_DIR} -I${MACH_APPLIBS_DIR} -I${OSAL_APPLIBS_DIR} \
-I${GRAPH_DIR}


AM_LDFLAGS =  ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	   ${XERCES_LIBS} ${EDITLINE_LD_FLAGS}
	   

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
