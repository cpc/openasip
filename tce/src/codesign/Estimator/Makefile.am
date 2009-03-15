PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
BASE_DIR = ${SRC_ROOT_DIR}/base
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
IDF_DIR = ${SRC_ROOT_DIR}/base//idf
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
SIM_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
INT_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
ESTIMATE_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_OSAL_DIR = ../../base/osal
LIB_MACH_DIR = ../../base/mach
LIB_TPEF_DIR = ../../base/tpef
LIB_IDF_DIR = ../../base//idf
LIB_PROGRAM_DIR = ../../base/program
LIB_SIM_APPLIB_DIR = ../../applibs/Simulator
LIB_INT_APPLIB_DIR = ../../applibs/Interpreter
LIB_TRACEDB_DIR = ../../applibs/TraceDB
LIB_ESTIMATE_DIR = ../../applibs/Estimator
LIB_UMACH_DIR = ../../base/umach
LIB_HDB_DIR = ../../applibs/hdb
LIB_PROGE_DIR = ../../applibs/ProGe
DISASM_DIR = ../../applibs/Disassembler
BEM_APPLIB_DIR = ../../applibs/bem
MACH_APPLIB_DIR = ../../applibs/mach
IDF_APPLIB_DIR = ../../applibs/idf
PROG_APPLIB_DIR = ../../applibs/program
LIB_COSTDB_DIR = ../../applibs/costdb
FSA_APPLIB_DIR = ../../applibs/FSA
LIB_APPLIBS_SCHED_DIR = ../../applibs/Scheduler

bin_PROGRAMS = estimate
bin_SCRIPTS = buildestimatorplugin
estimate_SOURCES = EstimatorCmdLineUI.cc EstimatorCmdLineOptions.cc

estimate_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} \
	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${IDF_DIR} -I${TPEF_DIR} \
	-I${UMACH_DIR} -I${TRACEDB_DIR} -I${ESTIMATE_DIR} \
	-I${HDB_DIR} -I${PROGE_DIR} -I${COSTDB_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h
AM_LDFLAGS = ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
estimate_SOURCES += \
	EstimatorCmdLineOptions.hh 
## headers end
