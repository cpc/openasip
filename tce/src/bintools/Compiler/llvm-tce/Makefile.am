bin_PROGRAMS = llvm-tce

llvm_tce_SOURCES = LLVMTCE.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

BASE_DIR = ${SRC_ROOT_DIR}/base
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
PROG_DIR = ${SRC_ROOT_DIR}/base/program
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
SCHED_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
OSAL_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/osal
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
LLVM_BACKEND_DIR = ${SRC_ROOT_DIR}/applibs/LLVMBackend
SCHED_ALGO_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/Algorithms

AM_CPPFLAGS = -I${TOOLS_DIR} -I${LLVM_BACKEND_DIR} -I${PROG_DIR} \
	-I${MACH_DIR} -I${SCHED_APPLIBS_DIR} -I${OSAL_DIR} \
	-I${UMACH_DIR} -I${SCHED_ALGO_DIR} -I${GRAPH_DIR} -I${OSAL_APPLIBS_DIR} 
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

llvm_tce_LDADD = ../../../libtce.la

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
## headers end
