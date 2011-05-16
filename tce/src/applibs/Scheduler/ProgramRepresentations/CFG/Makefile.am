noinst_LTLIBRARIES = libcfg.la
libcfg_la_SOURCES = BasicBlockNode.cc ControlFlowEdge.cc ControlFlowGraph.cc \
CFGStatistics.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
SCHED_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
SCHED_ALGO_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/Algorithms
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach
GRAPH_DIR = ${BASE_DIR}/Graph
DISASSEMBLER_DIR = ${APPLIBS_DIR}/Disassembler
SCHEDULER_DIR = ${APPLIBS_DIR}/Scheduler
PR_DIR = ${SCHEDULER_DIR}/ProgramRepresentations/
DDG_DIR = ${PR_DIR}/DDG/
PDG_DIR = ${PR_DIR}/PDG/

AM_CPPFLAGS = -I${TOOLS_DIR} -I${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${GRAPH_DIR} -I${DISASSEMBLER_DIR} \
-I${DDG_DIR} -I${PDG_DIR} -I${SCHED_ALGO_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libcfg_la_SOURCES += \
	CFGStatistics.hh ControlFlowGraph.hh \
	ControlFlowEdge.hh BasicBlockNode.hh 
## headers end
