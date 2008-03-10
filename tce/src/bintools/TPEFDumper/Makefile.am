LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..

OSAL_DIR = ../../base/osal
TPEF_DIR = ../../base/tpef
MACH_DIR = ../../base/mach

PROGRAM_DIR = ../../base/program
DISASM_DIR = ../../applibs/Disassembler
UM_DIR = ../../base/umach
APPLIBS_MACH_DIR = ../../applibs/mach
APPLIBS_FSA_DIR = ../../applibs/FSA

bin_PROGRAMS = dumptpef
dumptpef_SOURCES = TPEFDumper.cc TPEFDumper.hh DumpTPEF.cc

dumptpef_LDADD = ${LIB_BASE_DIR}/libttabase.la \
        ${LIB_TCETOOLS_DIR}/libtcetools.la \
        ${DISASM_DIR}/libdisassembler.la \
        ${UM_DIR}/libuniversalmachine.la \
        ${APPLIBS_MACH_DIR}/libapplibsmach.la \
        ${APPLIBS_FSA_DIR}/libfsa.la


AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
        -I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
        -I${MACH_DIR} -I$(PROGRAM_DIR) -I$(TPEF_DIR) \
	-I$(DISASM_DIR) -I${APPLIBS_MACH_DIR}

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} 

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
