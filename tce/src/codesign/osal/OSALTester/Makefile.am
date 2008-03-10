LIB_TCETOOLS_DIR = ../../../tools
LIB_INTERPRETER_DIR = ../../../applibs/Interpreter
LIB_OSAL_DIR = ../../../base/osal
LIB_BASE_DIR = ../../../base
LIB_MEMORY_DIR = ../../../base/memory
LIB_APPLIBS_OSAL_DIR = ../../../applibs/osal
BASE_DIR = ../../../..
LIB_UM_DIR = ../../../base/umach
LIB_APPLIBS_DIR = ../../../applibs
LIB_APPLIBS_HDB_DIR = ../../../applibs/hdb
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach
APPLIBS_FSA_DIR = ../../../applibs/FSA

bin_PROGRAMS = testosal
testosal_SOURCES = TestOsal.cc
testosal_LDADD = ${LIB_INTERPRETER_DIR}/libinterpreter.la \
	${LIB_UM_DIR}/libuniversalmachine.la \
	${LIB_BASE_DIR}/libttabase.la \
	${LIB_TCETOOLS_DIR}/libtcetools.la \
	${LIB_APPLIBS_OSAL_DIR}/libapplibsosal.la \
	${LIB_APPLIBS_HDB_DIR}/libhdb.la \
	${LIB_APPLIBS_MACH_DIR}/libapplibsmach.la \
	${APPLIBS_FSA_DIR}/libfsa.la \
	${DL_FLAGS} -lxerces-c -lpthread \
	${TCL_LD_FLAGS} ${BOOST_LDFLAGS} ${EDITLINE_LD_FLAGS}

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${BASE_DIR} \
	-I${LIB_INTERPRETER_DIR} -I${LIB_OSAL_DIR} \
	-I${LIB_MEMORY_DIR} -I${LIB_APPLIBS_OSAL_DIR} \
	-I${LIB_APPLIBS_HDB_DIR} -I${LIB_APPLIBS_MACH_DIR}

AM_LDFLAGS = ${BOOST_LIBDIR} ${DYNAMIC_FLAG} ${SQLITE_LIBDIR} ${EDITLINE_LIBDIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
