bin_PROGRAMS = llvm-tce

llvm_tce_SOURCES = LLVMTCE.cc LLVMTCECmdLineOptions.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

BASE_DIR = ${SRC_ROOT_DIR}/base
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
PROG_DIR = ${SRC_ROOT_DIR}/base/program
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
SCHED_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
LLVM_BACKEND_DIR = ${SRC_ROOT_DIR}/applibs/LLVMBackend
SCHED_ALGO_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/Algorithms

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
LIB_LLVM_BACKEND_DIR =../../../applibs/LLVMBackend

AM_CPPFLAGS = -I${TOOLS_DIR} -I${LLVM_BACKEND_DIR} -I${PROG_DIR} \
	-I${MACH_DIR} -I${SCHED_APPLIBS_DIR} -I${OSAL_DIR} \
	-I${UMACH_DIR} -I${SCHED_ALGO_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

llvm_tce_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
llvm_tce_SOURCES += \
	LLVMTCECmdLineOptions.hh 
## headers end
