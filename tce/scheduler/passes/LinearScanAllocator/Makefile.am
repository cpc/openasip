pkglibdir = ${prefix}/share/tce/scheduler/passes/LinearScanAllocator
pkglib_LTLIBRARIES = \
	SimpleGuardAllocatorPass.la AddressSpaceMapperPass.la \
	LinearScanAllocatorPass.la OldFrontendStackAnalyzerPass.la 

SimpleGuardAllocatorPass_la_SOURCES = SimpleGuardAllocatorPass.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
PROG_DIR = ${SRC_ROOT_DIR}/base/program
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
ESTIMATOR_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
EXPLORER_DIR = ${SRC_ROOT_DIR}/applibs/Explorer
DSDB_DIR = ${SRC_ROOT_DIR}/applibs/dsdb
SIM_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
INTERP_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb
LLVMBACKEND_DIR = ${SRC_ROOT_DIR}/applibs/LLVMBackend
SCHEDULER_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
ALGO_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/Algorithms
APPLIBS_PROG = ${SRC_ROOT_DIR}/applibs/program
PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
RM_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ResourceManager
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph
PDG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/ProgramDependenceGraph
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler
CFG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/ControlFlowGraph
DDG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/DataDependenceGraph
SELECTOR_DIR = ${SCHEDULER_DIR}/Selector
STACK_FRAME_DIR = ${SCHEDULER_DIR}/Algorithms/StackFrameManagement

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
		-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} -I${ESTIMATOR_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} -I${IDF_DIR} \
		-I${HDB_DIR} -I${TRACEDB_DIR} -I${SIM_DIR} -I${COSTDB_DIR} \
		-I${EXPLORER_DIR} -I${DSDB_DIR} -I${INTERP_DIR} \
		-I${LLVMBACKEND_DIR} -I${SCHEDULER_DIR} -I${APPLIBS_PROG} \
		-I${PROGE_DIR} -I${ALGO_DIR} -I${RM_DIR} -I${PDG_DIR} \
		-I${GRAPH_DIR} -I${DISASM_DIR} -I${DDG_DIR} -I${CFG_DIR} \
		-I${SELECTOR_DIR} -I${STACK_FRAME_DIR}

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS} -module

all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running Scheduler from the source tree to find them.
	@@(for soname in $(shell ls -1 ${srcdir}/.libs/*.so); do \
		ln -sf $$soname ${srcdir}; \
	done)

clean-local:
	rm -f ${srcdir}/*.so
