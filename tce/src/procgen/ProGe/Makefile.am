PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
PROGE_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
BEM_DIR = ${SRC_ROOT_DIR}/base/bem

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..
OSAL_DIR = ../../base/osal
TPEF_DIR = ../../base/tpef
IDF_DIR = ../../base//idf
PROGRAM_DIR = ../../base/program
SIM_APPLIB_DIR = ../../applibs/Simulator
UMACH_DIR = ../../base/umach
INT_APPLIB_DIR = ../../applibs/Interpreter
DISASM_DIR = ../../applibs/Disassembler
TRACEDB_DIR = ../../applibs/TraceDB
ESTIMATE_DIR = ../../applibs/Estimator
LIB_HDB_DIR = ../../applibs/hdb
LIB_PROGE_APPLIBS_DIR = ../../applibs/ProGe
BEM_APPLIBS_DIR = ../../applibs/bem
MACH_APPLIBS_DIR = ../../applibs/mach
IDF_APPLIBS_DIR = ../../applibs/idf
LIB_BEM_DIR = ../../base/bem
APPLIBS_FSA_DIR = ../../applibs/FSA

bin_SCRIPTS = buildicdecoderplugin
bin_PROGRAMS = generateprocessor

generateprocessor_SOURCES = GenerateProcessor.cc \
ProGeCmdLineOptions.cc

generateprocessor_LDADD = ${LIB_BASE_DIR}/libttabase.la \
${LIB_TCETOOLS_DIR}/libtcetools.la ${LIB_PROGE_APPLIBS_DIR}/libproge.la \
${LIB_HDB_DIR}/libhdb.la ${UMACH_DIR}/libuniversalmachine.la \
${BEM_APPLIBS_DIR}/libapplibsbem.la ${ESTIMATE_DIR}/libestimator.la \
${TRACEDB_DIR}/libtracedb.la ${MACH_APPLIBS_DIR}/libapplibsmach.la \
${IDF_APPLIBS_DIR}/libapplibsidf.la ${LIB_BEM_DIR}/libbem.la \
	${APPLIBS_FSA_DIR}/libfsa.la


AM_CPPFLAGS = -I${HDB_DIR} -I${TOOLS_DIR} -I${PROGE_APPLIBS_DIR} \
-I${MACH_DIR} -I${BEM_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

#-I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
#	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
#	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${IDF_DIR} -I${TPEF_DIR} \
#	-I${UMACH_DIR} -I${TRACEDB_DIR} -I${ESTIMATE_DIR} \
#	-I${HDB_DIR}
AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $2(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
