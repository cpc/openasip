lib_LTLIBRARIES = libddg.la
libddg_la_SOURCES = \
	DataDependenceEdge.cc DataDependenceGraph.cc DataDependenceGraphBuilder.cc ConstantAliasAnalyzer.cc FalseAliasAnalyzer.cc 

#MNData.cc
#MemoryAliasAnalyzer.cc

SRC_ROOT_DIR = ../../../../
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
PDG_DIR = ${SCHED_DIR}/ProgramRepresentations/ProgramDependenceGraph
CFG_DIR = ${SCHED_DIR}/ProgramRepresentations/ControlFlowGraph

AM_CPPFLAGS = -I${TOOLS_DIR} -I${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${GRAPH_DIR} -I${PDG_DIR} -I${CFG_DIR} \
	-I${DISASSEMBLER_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
