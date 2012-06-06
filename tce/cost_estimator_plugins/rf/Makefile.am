pkglibdir = ${prefix}/share/tce/cost_estimator_plugins/rf
pkglib_LTLIBRARIES = StrictMatchRFEstimator.la InterpolatingRFEstimator.la

StrictMatchRFEstimator_la_SOURCES = StrictMatchRFEstimator.cc
StrictMatchRFEstimator_la_LDFLAGS = -module -version-info ${LIB_VERSION}

InterpolatingRFEstimator_la_SOURCES = InterpolatingRFEstimator.cc
InterpolatingRFEstimator_la_LDFLAGS = -module -version-info ${LIB_VERSION}

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
PROG_DIR = ${SRC_ROOT_DIR}/base/program
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
ESTIMATOR_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
SIM_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
		-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} -I${ESTIMATOR_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} -I${IDF_DIR} \
		-I${HDB_DIR} -I${TRACEDB_DIR} -I${SIM_DIR} -I${COSTDB_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS} -module -version-info ${LIB_VERSION}

all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running Estimator from the source tree to find them.
	@@(for soname in `ls -1 ./.libs/*.so`; do \
		ln -sf $$soname .; \
	done)

clean-local:
	rm -f ./*.so
