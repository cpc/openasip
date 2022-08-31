pkglibdir = ${prefix}/share/openasip/icdecoder_plugins/base
pkglib_LTLIBRARIES = DefaultICDecoderPlugin.la

DefaultICDecoderPlugin_la_SOURCES = \
	DefaultICDecoderPlugin.cc CentralizedControlICGenerator.cc \
	DefaultDecoderGenerator.cc DefaultICGenerator.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
PROG_DIR = ${SRC_ROOT_DIR}/base/program
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
ESTIMATOR_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
EXPLORER_DIR = ${SRC_ROOT_DIR}/applibs/Explorer
DSDB_DIR = ${SRC_ROOT_DIR}/applibs/dsdb
SIM_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
INTERP_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
COSTDB_DIR = ${SRC_ROOT_DIR}/applibs/costdb
LLVMBACKEND_DIR = ${SRC_ROOT_DIR}/applibs/LLVMBackend
SCHEDULER_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
APPLIBS_PROG = ${SRC_ROOT_DIR}/applibs/program
PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
PROGE2_DIR = ${SRC_ROOT_DIR}/procgen/ProGe
OSAL_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/osal

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
		-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} -I${ESTIMATOR_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} -I${IDF_DIR} \
		-I${HDB_DIR} -I${TRACEDB_DIR} -I${SIM_DIR} -I${COSTDB_DIR} \
		-I${EXPLORER_DIR} -I${DSDB_DIR} -I${INTERP_DIR} \
		-I${LLVMBACKEND_DIR} -I${SCHEDULER_DIR} -I${APPLIBS_PROG} \
		-I${PROGE_DIR} -I${OSAL_APPLIB_DIR} -I${GRAPH_DIR} -I${PROGE2_DIR}

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h


# TCE 1.1 = 1:0:0
# TCE 1.2 = 2:0:0 
# TCE 1.3 = 3:0:0 
# TCE 1.4 = 4:0:0
# TCE 1.5 = 5:0:0

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBDIR} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS} -module -version-info ${LIB_VERSION}

all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running ProGe from the source tree to find them.
	@@(for soname in `ls -1 ./.libs/*.so`; do \
		ln -sf $$soname .; \
	done)

clean-local:
	rm -f ./*.so

# headers start

DefaultICDecoderPlugin_la_SOURCES += \
	CentralizedControlICGenerator.hh DefaultDecoderGenerator.hh \
	DefaultICGenerator.hh

# headers end
