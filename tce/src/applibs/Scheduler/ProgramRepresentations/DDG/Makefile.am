noinst_LTLIBRARIES = libddg.la
libddg_la_SOURCES = \
	DataDependenceEdge.cc DataDependenceGraph.cc DataDependenceGraphBuilder.cc \
    ConstantAliasAnalyzer.cc FalseAliasAnalyzer.cc \
	LLVMTCEDataDependenceGraphBuilder.cc MemoryAliasAnalyzer.cc \
	LLVMAliasAnalyzer.cc LiveRangeData.cc LiveRange.cc

#MNData.cc
#MemoryAliasAnalyzer.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach
GRAPH_DIR = ${BASE_DIR}/Graph
DISASSEMBLER_DIR = ${APPLIBS_DIR}/Disassembler
SCHED_DIR = ${APPLIBS_DIR}/Scheduler
PDG_DIR = ${SCHED_DIR}/ProgramRepresentations/PDG
CFG_DIR = ${SCHED_DIR}/ProgramRepresentations/CFG
ALGO_DIR = ${SCHED_DIR}/Algorithms
SELECTOR_DIR = ${SCHED_DIR}/Selector
LLVMTCE_DIR = ${APPLIBS_DIR}/LLVMBackend

AM_CPPFLAGS = -I${TOOLS_DIR} -I${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${GRAPH_DIR} -I${PDG_DIR} -I${CFG_DIR} \
	-I${DISASSEMBLER_DIR} -I${ALGO_DIR} -I${SELECTOR_DIR} -I${LLVMTCE_DIR}\
	-I${SCHED_DIR} ${LLVM_CPPFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libddg_la_SOURCES += \
	DataDependenceGraphBuilder.hh ConstantAliasAnalyzer.hh \
	LLVMAliasAnalyzer.hh MemoryAliasAnalyzer.hh \
	DataDependenceEdge.hh DataDependenceGraph.hh \
	LLVMTCEDataDependenceGraphBuilder.hh MoveNodeUse.hh \
	LiveRangeData.hh LiveRange.hh \
	FalseAliasAnalyzer.hh MoveNodeUse.icc 
## headers end
