TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

#-rdynamic adds all linked symbols to the main application so
#          we don't need to link all the stuff to every shared object
EXTRA_LINKER_FLAGS = ${DL_FLAGS} ${DYNAMIC_FLAG} ${BOOST_LDFLAGS}
INITIALIZATION = dynamic_modules

CLEAN_FILES = data/TestExplorerPlugin.so \
	    data/test.dsdb

include ${TOP_SRCDIR}/test/Makefile_test.defs


SRC_BASE=${TOP_SRCDIR}/src
BASE=${SRC_BASE}/base
APPLIBS=${SRC_BASE}/applibs
PROGE=${SRC_BASE}/applibs/ProGe
TRACEDB=${SRC_BASE}/applibs/TraceDB
SIMULATOR=${SRC_BASE}/applibs/Simulator
HDB=${SRC_BASE}/applibs/hdb
COSTDB=${SRC_BASE}/applibs/costdb
EXPLORER=${SRC_BASE}/applibs/Explorer
DSDB=${SRC_BASE}/applibs/dsdb
SCHEDULER_APPLIBS=${SRC_BASE}/applibs/Scheduler
OSAL=${SRC_BASE}/base/osal
UMACH=${SRC_BASE}/base/umach
INTERPRETER=${SRC_BASE}/applibs/Interpreter
TOOLS=${SRC_BASE}/tools
MEMORY=${SRC_BASE}/base/memory
MACH_APPLIBS=${APPLIBS}/mach
PROGRAM_APPLIBS=${APPLIBS}/program

COMPILE_FLAGS = -I${TOP_SRCDIR} -I ${TOOLS} \
-I ${SRC_BASE}/applibs/Estimator -I ${SRC_BASE}/base/idf \
-I ${PROGE} -I ${BASE}/mach -I ${BASE}/program \
-I ${TRACEDB} -I ${SIMULATOR} -I ${HDB} -I ${COSTDB} \
-I ${EXPLORER} -I ${DSDB} -I ${SCHEDULER_APPLIBS} -I ${OSAL} \
-I ${UMACH} -I ${INTERPRETER} -I ${MEMORY} -I ${MACH_APPLIBS} \
-I ${PROGRAM_APPLIBS} ${CPPFLAGS} $(CXXFLAGS)

# 'dynamic_modules' rule compiles the dynamic modules representing Explorer
# plugins
#
# Note that we must link the symbols that the dynamic modules need
# into the dynamic modules otherwise dlopen() fails thanks to the RTLD_NOW
# flag. Symbols needed by tests are in object files listed in ${MISC_OBJ}.
dynamic_modules: data/TestExplorerPlugin.so data/debug.opb 

data/TestExplorerPlugin.so: data/TestExplorerPlugin.cc
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/TestExplorerPlugin.cc \
	${SHARED_CXX_FLAGS} -o data/TestExplorerPlugin.so

data/debug.opb: data/debug.cc
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/debug.cc ${SHARED_CXX_FLAGS} -o \
	data/debug.opb
