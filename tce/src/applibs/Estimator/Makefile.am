lib_LTLIBRARIES = libestimator.la
libestimator_la_SOURCES = Estimator.cc CostEstimationPlugin.cc \
RFCostEstimationPlugin.cc FUCostEstimationPlugin.cc TransportPath.cc \
ICDecoderEstimatorPlugin.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
IDF_DIR = ${BASE_DIR}/idf
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${BASE_DIR}/mach
PROG_DIR = ${BASE_DIR}/program
MACH_APP_DIR = ${SRC_ROOT_DIR}/applibs/mach
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${MACH_APP_DIR} -I${PROG_DIR} \
	-I${BASE_DIR} -I${TRACEDB_DIR} -I${IDF_DIR} -I${HDB_DIR} \
	-I${PROGE_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
