PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
BASE_DIR = ${SRC_ROOT_DIR}/base
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
SIM_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
UM_DIR = ${SRC_ROOT_DIR}/base/umach
INT_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
APPLIBS_HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
APPLIBS_MACH_DIR = ${SRC_ROOT_DIR}/applibs/mach
EXPLORER_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Explorer
SCHEDULER_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
ESTIMATOR_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
DSDB_DIR = ${SRC_ROOT_DIR}/applibs/dsdb
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph
PROGE_APPLIBS = ${SRC_ROOT_DIR}/applibs/ProGe
BEM_APPLIBS = ${SRC_ROOT_DIR}/applibs/bem
IDF_APPLIBS = ${SRC_ROOT_DIR}/applibs/idf
LLVMBACKEND_APPLIBS = ${SRC_ROOT_DIR}/applibs/LLVMBackend
OSAL_APPLIBS = ${SRC_ROOT_DIR}/applibs/osal

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_PROGRAM_DIR = ../../base/program
LIB_SIM_APPLIB_DIR = ../../applibs/Simulator
LIB_UM_DIR = ../../base/umach
LIB_INT_APPLIB_DIR = ../../applibs/Interpreter
DISASM_DIR = ../../applibs/Disassembler
TRACEDB_DIR = ../../applibs/TraceDB
PROG_APPLIB_DIR = ../../applibs/program
LIB_APPLIBS_HDB_DIR = ../../applibs/hdb
LIB_APPLIBS_MACH_DIR = ../../applibs/mach
FSA_APPLIB_DIR = ../../applibs/FSA
LIB_EXPLORER_APPLIBS_DIR = ../../applibs/Explorer
LIB_SCHEDULER_APPLIBS_DIR = ../../applibs/Scheduler
LIB_ESTIMATOR_APPLIBS_DIR = ../../applibs/Estimator
LIB_COSTDB_DIR = ../../applibs/costdb
LIB_DSDB_DIR = ../../applibs/dsdb
LIB_GRAPH_DIR = ../../base/Graph
LIB_PROGE_APPLIBS = ../../applibs/ProGe
LIB_BEM_APPLIBS = ../../applibs/bem
LIB_IDF_APPLIBS = ../../applibs/idf
LIB_LLVMBACKEND_APPLIBS = ../../applibs/LLVMBackend
LIB_OSAL_APPLIBS = ../../applibs/osal

bin_PROGRAMS = explore
bin_SCRIPTS = buildexplorerplugin pareto_vis

EXTRA_DIST = ${bin_SCRIPTS}

explore_SOURCES = Explorer.cc 

explore_LDADD = ../../libtce.la

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} \
	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${BASE_DIR} \
	-I${MACH_DIR} -I${IDF_DIR} -I$(PROGRAM_DIR) -I${APPLIBS_HDB_DIR} \
	-I${APPLIBS_MACH_DIR} -I${EXPLORER_APPLIBS_DIR} \
	-I${SCHEDULER_APPLIBS_DIR} -I${UM_DIR} \
	-I${ESTIMATOR_APPLIBS_DIR} -I${COSTDB_DIR} -I${DSDB_DIR} \
	-I${GRAPH_DIR} -I${PROGE_APPLIBS} -I${IDF_APPLIBS} -I${BEM_APPLIBS} \
	-I${LLVMBACKEND_APPLIBS} -I${OSAL_APPLIBS}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
explore_SOURCES += 
## headers end
