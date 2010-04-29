PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
PROGE_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
IMPL_TESTER_DIR = ${SRC_ROOT_DIR}/applibs/ImplementationTester

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
TPEF_DIR = ../../base/tpef
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
PROGRAM_DIR = ../../base/program
SIM_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
UMACH_DIR = ../../base/umach
INT_APPLIB_DIR = ../../applibs/Interpreter
DISASM_DIR = ../../applibs/Disassembler
TRACEDB_DIR = ../../applibs/TraceDB
ESTIMATE_DIR = ../../applibs/Estimator
LIB_HDB_DIR = ../../applibs/hdb
LIB_PROGE_APPLIBS_DIR = ../../applibs/ProGe
BEM_APPLIBS_DIR = ../../applibs/bem
MACH_APPLIBS_DIR = ../../applibs/mach
IDF_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/idf
LIB_BEM_DIR = ../../base/bem
APPLIBS_FSA_DIR = ../../applibs/FSA

bin_SCRIPTS = buildicdecoderplugin
bin_PROGRAMS = generateprocessor ttaunittester

generateprocessor_SOURCES = GenerateProcessor.cc \
ProGeCmdLineOptions.cc ProGeCmdLineOptions.hh GenerateProcessor.hh

generateprocessor_LDADD = ../../libtce.la

ttaunittester_SOURCES = TTAUnitTester.cc TTAUnitTesterCmdLineOptions.cc \
TTAUnitTesterCmdLineOptions.hh TTAUnitTester.hh

ttaunittester_LDADD = ../../libtce.la

AM_CPPFLAGS = -I${HDB_DIR} -I${TOOLS_DIR} -I${PROGE_APPLIBS_DIR} \
-I${MACH_DIR} -I${BEM_DIR} -I${IMPL_TESTER_DIR} -I${IDF_DIR} \
-I${IDF_APPLIBS_DIR} -I${SIM_APPLIB_DIR} -I${OSAL_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

#-I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
#	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
#	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${IDF_DIR} -I${TPEF_DIR} \
#	-I${UMACH_DIR} -I${TRACEDB_DIR} -I${ESTIMATE_DIR} \
#	-I${HDB_DIR}
AM_LDFLAGS = ${TCE_LDFLAGS}

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


