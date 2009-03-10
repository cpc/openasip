PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
INTERPRETER_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
MEMORY_DIR = ${SRC_ROOT_DIR}/base/memory
APPLIBS_OSAL_DIR = ${SRC_ROOT_DIR}/applibs/osal
APPLIBS_HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
APPLIBS_MACH_DIR = ${SRC_ROOT_DIR}/applibs/mach

LIB_TCETOOLS_DIR = ../../../tools
LIB_INTERPRETER_DIR = ../../../applibs/Interpreter
LIB_OSAL_DIR = ../../../base/osal
LIB_BASE_DIR = ../../../base
LIB_MEMORY_DIR = ../../../base/memory
LIB_APPLIBS_OSAL_DIR = ../../../applibs/osal
LIB_UM_DIR = ../../../base/umach
LIB_APPLIBS_DIR = ../../../applibs
LIB_APPLIBS_HDB_DIR = ../../../applibs/hdb
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach
APPLIBS_FSA_DIR = ../../../applibs/FSA

bin_PROGRAMS = testosal
testosal_SOURCES = TestOsal.cc
testosal_LDADD = ${SRC_ROOT_DIR}/libtce.la \
	${TCL_LD_FLAGS} ${BOOST_LDFLAGS} ${EDITLINE_LD_FLAGS}

AM_CPPFLAGS = -I${TOOLS_DIR} \
	-I${INTERPRETER_DIR} -I${OSAL_DIR} \
	-I${MEMORY_DIR} -I${APPLIBS_OSAL_DIR} \
	-I${APPLIBS_HDB_DIR} -I${APPLIBS_MACH_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${DYNAMIC_FLAG} ${SQLITE_LIBDIR} ${EDITLINE_LIBDIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
