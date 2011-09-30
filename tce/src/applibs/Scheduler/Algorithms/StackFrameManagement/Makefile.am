noinst_LTLIBRARIES = libstackframemanagement.la
libstackframemanagement_la_SOURCES = OldFrontendStackAnalyzer.cc \
StackFrameCodeCreator.cc StackCodeRemover.cc StackFrameData.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UMACH_DIR = ${BASE_DIR}/umach
UM_DIR = ${BASE_DIR}/umach
SCHED_APPLIBS_DIR = ${APPLIBS_DIR}/Scheduler
PDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/PDG
GRAPH_DIR = ${BASE_DIR}/Graph
DISASM_DIR = ${APPLIBS_DIR}/Disassembler


AM_CPPFLAGS = -I${RESOURCE_MODEL_DIR} -I${TOOLS_DIR} -I${POM_DIR} \
	-I${OSAL_DIR} -I${MACH_DIR} -I${DISASM_DIR} -I${TOOLS_DIR} \
	-I${SCHED_APPLIBS_DIR} -I${UMACH_DIR} -I${PDG_DIR} -I${GRAPH_DIR} \
         ${LLVM_CPPFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libstackframemanagement_la_SOURCES += \
	StackCodeRemover.hh OldFrontendStackAnalyzer.hh \
	StackFrameData.hh StackFrameCodeCreator.hh 
## headers end
