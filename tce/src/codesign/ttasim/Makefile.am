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
LIB_APPLIBS_SCHED_DIR = ../../applibs/Scheduler
FSA_APPLIB_DIR = ../../applibs/FSA

bin_PROGRAMS = ttasim
ttasim_SOURCES = TTASim.cc SimulatorCmdLineOptions.cc

ttasim_LDADD = ${LIB_BASE_DIR}/libttabase.la \
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
	${LIB_APPLIBS_SCHED_DIR}/libscheduler.la \
	${FSA_APPLIB_DIR}/libfsa.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${OSAL_DIR} \
	-I${SIM_APPLIB_DIR} -I${INT_APPLIB_DIR} -I${LIB_BASE_DIR} \
	-I${MACH_DIR} -I$(PROGRAM_DIR) -I${LIB_APPLIBS_HDB_DIR} \
	-I${LIB_APPLIBS_MACH_DIR} -I${LIB_APPLIBS_SCHED_DIR}
AM_LDFLAGS = ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
