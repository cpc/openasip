noinst_LTLIBRARIES = libhwgen.la
libhwgen_la_SOURCES = FUGen.cc RFGen.cc HDLGenerator.cc LHSValue.cc


SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs


IDF_DIR = ${BASE_DIR}/idf
HDB_DIR = ${APPLIBS_DIR}/hdb
ESTIMATOR_DIR = ${APPLIBS_DIR}/Estimator
MACH_DIR = ${BASE_DIR}/mach
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
OSAL_DIR = ${BASE_DIR}/osal
GRAPH_DIR = ${BASE_DIR}/Graph
APPLIBS_PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
PROCGEN_PROGE_DIR = ${SRC_ROOT_DIR}/procgen/ProGe


AM_CPPFLAGS = -I${IDF_DIR} \
	-I${HDB_DIR} \
	-I${ESTIMATOR_DIR} \
	-I${MACH_DIR} \
	-I${TOOLS_DIR} \
	-I${OSAL_DIR} \
	-I${GRAPH_DIR} \
	-I${APPLIBS_PROGE_DIR} \
	-I${PROCGEN_PROGE_DIR} \
	-I$(SRC_ROOT_DIR)/applibs/Explorer \
	-I$(SRC_ROOT_DIR)/applibs/dsdb \
	-I$(SRC_ROOT_DIR)/applibs/Simulator


dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

## headers start
libhwgen_la_SOURCES += \
	FUGen.hh RFGen.hh HDLGenerator.hh HWGenTools.hh HDLRegister.hh \
	HDLPort.hh LHSValue.hh Generatable.hh WidthTransformations.hh \
	BinaryOps.hh
## headers end
