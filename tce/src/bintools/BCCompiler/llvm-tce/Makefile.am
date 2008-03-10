bin_PROGRAMS = llvmtce

llvmtce_SOURCES = LLVMTCE.cc LLVMTCECmdLineOptions.cc
llvmtce_LDADD =

BASE_DIR = ../../../base
OSAL_DIR = ../../../base/osal
PROG_DIR = ../../../base/program
MACH_DIR = ../../../base/mach
UMACH_DIR = ../../../base/umach
SCHED_APPLIBS_DIR = ../../../applibs/Scheduler
APPLIBS_DIR = ../../../applibs
TOOLS_DIR = ../../../tools
LLVM_BACKEND_DIR = ../../../applibs/LLVMBackend

AM_CPPFLAGS = -I${TOOLS_DIR} -I${LLVM_BACKEND_DIR} -I${PROG_DIR} \
	-I${MACH_DIR} -I${SCHED_APPLIBS_DIR} -I${OSAL_DIR} \
	-I${UMACH_DIR}

AM_LDFLAGS =  ${BASE_DIR}/libttabase.la ${APPLIBS_DIR}/libapplibs.la \
	${TOOLS_DIR}/libtcetools.la ${TCL_LD_FLAGS}


dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
