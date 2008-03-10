LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..
OSAL_DIR = ../../base/osal
MACH_DIR = ../../base/mach
TPEF_DIR = ../../base/tpef
IDF_DIR = ../../base//idf
PROGRAM_DIR = ../../base/program
SIM_APPLIB_DIR = ../../applibs/Simulator
INT_APPLIB_DIR = ../../applibs/Interpreter
DISASM_DIR = ../../applibs/Disassembler
TRACEDB_DIR = ../../applibs/TraceDB
ESTIMATE_DIR = ../../applibs/Estimator
UMACH_DIR = ../../base/umach
HDB_DIR = ../../applibs/hdb
PROGE_DIR = ../../applibs/ProGe
BEM_APPLIB_DIR = ../../applibs/bem
MACH_APPLIB_DIR = ../../applibs/mach
IDF_APPLIB_DIR = ../../applibs/idf
PROG_APPLIB_DIR = ../../applibs/program
COSTDB_DIR = ../../applibs/costdb
FSA_APPLIB_DIR = ../../applibs/FSA
LIB_APPLIBS_SCHED_DIR = ../../applibs/Scheduler

bin_PROGRAMS = estimate
bin_SCRIPTS = buildestimatorplugin
estimate_SOURCES = EstimatorCmdLineUI.cc EstimatorCmdLineOptions.cc

estimate_LDADD = ${LIB_BASE_DIR}/libttabase.la \
	${LIB_TCETOOLS_DIR}/libtcetools.la \
	${SIM_APPLIB_DIR}/libsimulator.la \
	${INT_APPLIB_DIR}/libinterpreter.la \
	${PROGRAM_DIR}/libprogram.la \
	${DISASM_DIR}/libdisassembler.la \
	${UMACH_DIR}/libuniversalmachine.la \
	${TRACEDB_DIR}/libtracedb.la \
	${ESTIMATE_DIR}/libestimator.la \
	${HDB_DIR}/libhdb.la \
	${PROGE_DIR}/libproge.la \
	${BEM_APPLIB_DIR}/libapplibsbem.la \
	${MACH_APPLIB_DIR}/libapplibsmach.la \
	${IDF_APPLIB_DIR}/libapplibsidf.la \
	${COSTDB_DIR}/libcostdb.la \
	${PROG_APPLIB_DIR}/libapplibsprogram.la \
	${FSA_APPLIB_DIR}/libfsa.la \
	${LIB_APPLIBS_SCHED_DIR}/libscheduler.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${IDF_DIR} -I${TPEF_DIR} \
	-I${UMACH_DIR} -I${TRACEDB_DIR} -I${ESTIMATE_DIR} \
	-I${HDB_DIR} -I${PROGE_DIR} -I${COSTDB_DIR}
AM_LDFLAGS = ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
