pkglibdir = ${prefix}/share/tce/scheduler/passes/PreOptimizer
pkglib_LTLIBRARIES = \
	PreOptimizerModule.la

PreOptimizerModule_la_SOURCES = PreOptimizerModule.cc PreOptimizerModule.hh

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TPEF_DIR = ${BASE_DIR}/tpef
MACH_DIR = ${BASE_DIR}/mach
OSAL_DIR = ${BASE_DIR}/osal
UMACH_DIR = ${BASE_DIR}/umach
PROG_DIR = ${BASE_DIR}/program
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
TRACEDB_DIR = ${APPLIBS_DIR}/TraceDB
SIM_DIR = ${APPLIBS_DIR}/Simulator
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
INTERP_DIR = ${APPLIBS_DIR}/Interpreter
LLVMBACKEND_DIR = ${APPLIBS_DIR}/LLVMBackend
SCHEDULER_DIR = ${APPLIBS_DIR}/Scheduler
ALGO_DIR = ${SCHEDULER_DIR}/Algorithms
APPLIBS_PROG = ${APPLIBS_DIR}/program
GRAPH_DIR = ${BASE_DIR}/Graph
CFG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/CFG
DDG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/DDG
PDG_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/PDG

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} -I${TRACEDB_DIR} \
        -I${LLVMBACKEND_DIR} -I${SCHEDULER_DIR} -I${APPLIBS_PROG} \
        -I${ALGO_DIR} -I${GRAPH_DIR} -I${CFG_DIR} -I${DDG_DIR} -I${PDG_DIR} \
        ${LLVM_CPPFLAGS}

#-I${DDG_DIR} -I${RM_DIR} -I${PROGE_DIR} -I${SIM_DIR} -I${HDB_DIR} \
#-I${SELECTOR_DIR} -I${PDG_DIR} 

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
        ${EDITLINE_LD_FLAGS} -module


all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running Scheduler from the source tree to find them.
	@@(for soname in $(shell ls -1 ${srcdir}/.libs/*.so); do \
		ln -sf $$soname ${srcdir}; \
	done)

clean-local:
	rm -f ${srcdir}/*.so
