LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..
OSAL_DIR = ../../base/osal
MACH_DIR = ../../base/mach
PROGRAM_DIR = ../../base/program
SIM_APPLIB_DIR = ../../applibs/Simulator
UM_DIR = ../../base/umach
INT_APPLIB_DIR = ../../applibs/Interpreter
DISASM_DIR = ../../applibs/Disassembler
TRACEDB_DIR = ../../applibs/TraceDB
PROG_APPLIB_DIR = ../../applibs/program
LIB_APPLIBS_HDB_DIR = ../../applibs/hdb
LIB_APPLIBS_MACH_DIR = ../../applibs/mach
FSA_APPLIB_DIR = ../../applibs/FSA
EXPLORER_APPLIBS_DIR = ../../applibs/Explorer
SCHEDULER_APPLIBS_DIR = ../../applibs/Scheduler
ESTIMATOR_APPLIBS_DIR = ../../applibs/Estimator
COSTDB_DIR = ../../applibs/costdb
IDF_DIR = ../../base/idf
DSDB_DIR = ../../applibs/dsdb
GRAPH_DIR = ../../base/Graph
PROGE_APPLIBS = ../../applibs/ProGe
BEM_APPLIBS = ../../applibs/bem
IDF_APPLIBS = ../../applibs/idf

bin_PROGRAMS = explore
bin_SCRIPTS = buildexplorerplugin
explore_SOURCES = Explorer.cc ExplorerCmdLineOptions.cc

explore_LDADD = ${LIB_BASE_DIR}/libttabase.la \
	${LIB_TCETOOLS_DIR}/libtcetools.la \
	${SIM_APPLIB_DIR}/libsimulator.la \
	${INT_APPLIB_DIR}/libinterpreter.la \
	${PROGRAM_DIR}/libprogram.la \
	${DISASM_DIR}/libdisassembler.la \
	${UM_DIR}/libuniversalmachine.la \
	${TRACEDB_DIR}/libtracedb.la \
	${PROG_APPLIB_DIR}/libapplibsprogram.la \
	${LIB_APPLIBS_HDB_DIR}/libhdb.la \
	${LIB_APPLIBS_MACH_DIR}/libapplibsmach.la \
	${FSA_APPLIB_DIR}/libfsa.la \
	${EXPLORER_APPLIBS_DIR}/libexplorer.la \
	${SCHEDULER_APPLIBS_DIR}/libscheduler.la \
	${ESTIMATOR_APPLIBS_DIR}/libestimator.la \
	${COSTDB_DIR}/libcostdb.la \
	${DSDB_DIR}/libdsdb.la \
	${PROGE_APPLIBS}/libproge.la \
	${BEM_APPLIBS}/libapplibsbem.la \
	${IDF_APPLIBS}/libapplibsidf.la
	${GRAPH_DIR}/libgraph.la


AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
	-I${MACH_DIR} -I${IDF_DIR} -I$(PROGRAM_DIR) -I${LIB_APPLIBS_HDB_DIR} \
	-I${LIB_APPLIBS_MACH_DIR} -I${EXPLORER_APPLIBS_DIR} \
	-I${SCHEDULER_APPLIBS_DIR} -I${UM_DIR} \
	-I${ESTIMATOR_APPLIBS_DIR} -I${COSTDB_DIR} -I${DSDB_DIR} \
	-I${GRAPH_DIR} -I${PROGE_APPLIBS} -I${IDF_APPLIBS} -I${BEM_APPLIBS}
AM_LDFLAGS = ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} \
    ${TCL_LD_FLAGS} ${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
