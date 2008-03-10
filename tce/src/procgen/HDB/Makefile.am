LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..
OSAL_DIR = ../../base/osal
MACH_DIR = ../../base/mach
TPEF_DIR = ../../base/tpef
IDF_DIR = ../../base//idf
PROGRAM_DIR = ../../base/program
SIM_APPLIB_DIR = ../../applibs/Simulator
UMACH_DIR = ../../base/umach
INT_APPLIB_DIR = ../../applibs/Interpreter
DISASM_DIR = ../../applibs/Disassembler
TRACEDB_DIR = ../../applibs/TraceDB
ESTIMATE_DIR = ../../applibs/Estimator
HDB_DIR = ../../applibs/hdb
MACH_APPLIB_DIR = ../../applibs/mach
PROG_APPLIB_DIR = ../../applibs/program
FSA_APPLIB_DIR = ../../applibs/FSA
LIB_APPLIBS_SCHED_DIR = ../../applibs/Scheduler

bin_PROGRAMS = createhdb
createhdb_SOURCES = CreateHDB.cc CreateHDBCmdLineOptions.cc

createhdb_LDADD = ${LIB_BASE_DIR}/libttabase.la \
	${LIB_TCETOOLS_DIR}/libtcetools.la \
	${SIM_APPLIB_DIR}/libsimulator.la \
	${INT_APPLIB_DIR}/libinterpreter.la \
	${PROGRAM_DIR}/libprogram.la \
	${DISASM_DIR}/libdisassembler.la \
	${UMACH_DIR}/libuniversalmachine.la \
	${TRACEDB_DIR}/libtracedb.la \
	${HDB_DIR}/libhdb.la \
	${MACH_APPLIB_DIR}/libapplibsmach.la \
	${PROG_APPLIB_DIR}/libapplibsprogram.la \
	${FSA_APPLIB_DIR}/libfsa.la \
	${LIB_APPLIBS_SCHED_DIR}/libscheduler.la


AM_CPPFLAGS = -I${HDB_DIR} -I${LIB_TCETOOLS_DIR}

#-I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
#	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
#	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${IDF_DIR} -I${TPEF_DIR} \
#	-I${UMACH_DIR} -I${TRACEDB_DIR} -I${ESTIMATE_DIR} \
#	-I${HDB_DIR}
AM_LDFLAGS = ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	 ${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $2(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
