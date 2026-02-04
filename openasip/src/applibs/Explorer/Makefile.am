noinst_LTLIBRARIES = libexplorer.la
libexplorer_la_SOURCES = ComponentImplementationSelector.cc CostEstimates.cc \
			DesignSpaceExplorer.cc DesignSpaceExplorerPlugin.cc \
			FrequencySweep.cc ExplorerPluginParameter.cc \
			ExplorerCmdLineOptions.cc ExplorationTools.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
IDF_DIR = ${BASE_DIR}/idf
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${BASE_DIR}/mach
PROG_DIR = ${BASE_DIR}/program
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb
SIMULATOR_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
DSDB_DIR = ${SRC_ROOT_DIR}/applibs/dsdb
SCHEDULER_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
LLVMBACKEND_DIR = ${SRC_ROOT_DIR}/applibs/LLVMBackend
OSAL_DIR = ${BASE_DIR}/osal
UMACH_DIR = ${BASE_DIR}/umach
ESTIMATOR_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
INTERPRETER_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
APPLIBS_MACH = ${SRC_ROOT_DIR}/applibs/mach
APP_OSAL_DIR = ${SRC_ROOT_DIR}/applibs/osal
BASE_GRAPH_DIR = ${BASE_DIR}/Graph

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${PROG_DIR} -I${BASE_DIR} \
	-I${TRACEDB_DIR} -I${IDF_DIR} -I${HDB_DIR} -I${COSTDB_DIR} \
	-I${SIMULATOR_DIR} -I${DSDB_DIR} -I${SCHEDULER_APPLIBS_DIR} \
	-I${OSAL_DIR} -I${UMACH_DIR} -I${ESTIMATOR_DIR} \
	-I${INTERPRETER_DIR} -I${APPLIBS_MACH} -I${LLVMBACKEND_DIR} -I${APP_OSAL_DIR} \
	-I${BASE_GRAPH_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libexplorer_la_SOURCES += \
	FrequencySweep.hh CostEstimates.hh \
	ExplorerCmdLineOptions.hh ExplorerPluginParameter.hh \
	ComponentImplementationSelector.hh DesignSpaceExplorer.hh \
	DesignSpaceExplorerPlugin.hh DesignSpaceExplorerPlugin.icc \
	ExplorationTools.hh
## headers end
