PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_APPLIBS_DIR = ../../applibs
ROOT_DIR = ../../..

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
ASM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Assembler
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler
UM_DIR = ${SRC_ROOT_DIR}/applibs/umach

bin_PROGRAMS = tceasm
tceasm_SOURCES = tceasm.cc

tceasm_LDADD = ${LIB_BASE_DIR}/libttabase.la \
        ${LIB_TCETOOLS_DIR}/libtcetools.la \
        ${LIB_APPLIBS_DIR}/libapplibs.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
        -I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
        -I${MACH_DIR} -I$(PROGRAM_DIR) -I$(TPEF_DIR) -I${ASM_APPLIBS_DIR} \
	-I${DISASM_DIR} -I${UM_DIR} -I${TOOLS_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
