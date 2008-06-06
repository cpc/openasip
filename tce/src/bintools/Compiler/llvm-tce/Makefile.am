bin_PROGRAMS = llvm-tce

llvm_tce_SOURCES = LLVMTCE.cc LLVMTCECmdLineOptions.cc

BASE_DIR = ../../../base
OSAL_DIR = ../../../base/osal
PROG_DIR = ../../../base/program
MACH_DIR = ../../../base/mach
UMACH_DIR = ../../../base/umach
SCHED_APPLIBS_DIR = ../../../applibs/Scheduler
APPLIBS_DIR = ../../../applibs
TOOLS_DIR = ../../../tools
LLVM_BACKEND_DIR = ../../../applibs/LLVMBackend

LIB_TCETOOLS_DIR = ../../../tools
LIB_BASE_DIR = ../../../base
ROOT_DIR = ../../../
PROGRAM_DIR = ../../../base/program
SIM_APPLIB_DIR = ../../../applibs/Simulator
UM_DIR = ../../../base/umach
INT_APPLIB_DIR = ../../../applibs/Interpreter
DISASM_DIR = ../../../applibs/Disassembler
TRACEDB_DIR = ../../../applibs/TraceDB
PROG_APPLIB_DIR = ../../../applibs/program
LIB_APPLIBS_HDB_DIR = ../../../applibs/hdb
LIB_APPLIBS_OSAL_DIR = ../../../applibs/osal
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach
LIB_APPLIBS_SCHED_DIR = ../../../applibs/Scheduler
FSA_APPLIB_DIR = ../../../applibs/FSA
SCHED_ALGO_DIR = ../../../applibs/Scheduler/Algorithms

AM_CPPFLAGS = -I${TOOLS_DIR} -I${LLVM_BACKEND_DIR} -I${PROG_DIR} \
	-I${MACH_DIR} -I${SCHED_APPLIBS_DIR} -I${OSAL_DIR} \
	-I${UMACH_DIR} -I${SCHED_ALGO_DIR}

llvm_tce_LDADD = ${BASE_DIR}/libttabase.la \
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
	${FSA_APPLIB_DIR}/libfsa.la \
	${LLVM_BACKEND_DIR}/libtcellvmbackend.la \
	${LIB_APPLIBS_OSAL_DIR}/libapplibsosal.la

if PYTHON_ENABLED
AM_LDFLAGS =  ${TCL_LD_FLAGS} ${BOOST_PYTHON_LIB}
else
AM_LDFLAGS =  ${TCL_LD_FLAGS}
endif

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
