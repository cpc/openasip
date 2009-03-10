PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler

LIB_DISASM_DIR = ../../applibs/Disassembler
UM_DIR = ../../base/umach
APPLIBS_MACH_DIR = ../../applibs/mach
APPLIBS_FSA_DIR = ../../applibs/FSA

bin_PROGRAMS = dumptpef
dumptpef_SOURCES = TPEFDumper.cc TPEFDumper.hh DumpTPEF.cc

dumptpef_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_CPPFLAGS = -I${TOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
        -I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
        -I${MACH_DIR} -I$(PROGRAM_DIR) -I$(TPEF_DIR) \
	-I$(DISASM_DIR) -I${APPLIBS_MACH_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} 

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
