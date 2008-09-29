PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..

OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler

PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program

LIB_DISASM_DIR = ../../applibs/Disassembler
APPLIBS_MACH_DIR = ../../applibs/mach
APPLIBS_FSA_DIR = ../../applibs/FSA

bin_PROGRAMS = tcedisasm
tcedisasm_SOURCES = tcedisasm.cc

tcedisasm_LDADD = ${LIB_BASE_DIR}/libttabase.la \
        ${LIB_TCETOOLS_DIR}/libtcetools.la \
        ${LIB_DISASM_DIR}/libdisassembler.la \
	${APPLIBS_MACH_DIR}/libapplibsmach.la \
	${APPLIBS_FSA_DIR}/libfsa.la


AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
        -I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
        -I${MACH_DIR} -I${UMACH_DIR} -I$(PROGRAM_DIR) -I$(TPEF_DIR) \
	-I$(DISASM_DIR) -I${APPLIBS_MACH_DIR} -I${TOOLS_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} 

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
