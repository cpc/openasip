noinst_LTLIBRARIES = libprogramdependencegraph.la
libprogramdependencegraph_la_SOURCES = ProgramGraph.cc ProgramDependenceEdge.cc ProgramDependenceNode.cc ProgramDependenceGraph.cc

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
CDG_DIR=${APPLIBS_DIR}/Scheduler/ProgramRepresentations/CDG
DDG_DIR=${APPLIBS_DIR}/Scheduler/ProgramRepresentations/DDG
CFG_DIR=${APPLIBS_DIR}/Scheduler/ProgramRepresentations/CFG
DISASSEMBLER_DIR = ${APPLIBS_DIR}/Disassembler

AM_CPPFLAGS = -I${TOOLS_DIR} -I${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${GRAPH_DIR} -I${DISASSEMBLER_DIR} \
	-I${CDG_DIR} -I${DDG_DIR} -I${CFG_DIR} ${LLVM_CPPFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libprogramdependencegraph_la_SOURCES += \
	ProgramGraph.hh ProgramDependenceGraph.hh \
	ProgramDependenceEdge.hh ProgramDependenceNode.hh 
## headers end
