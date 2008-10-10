lib_LTLIBRARIES = libgraph.la
libgraph_la_SOURCES = Graph.cc BoostGraph.cc GraphUtilities.cc GraphEdge.cc GraphNode.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach

AM_CPPFLAGS = -I${TOOLS_DIR} -I ${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
