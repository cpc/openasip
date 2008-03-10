TOP_SRCDIR = ../../../..

CLEAN_FILES = ttasim.out data/debug.opb data/test.dsdb

INITIALIZATION = dynamic_modules

include ${TOP_SRCDIR}/test/Makefile_test.defs

OSAL_DIR = ${TOP_SRCDIR}/src/base/osal
SIMLIB_DIR = ${TOP_SRCDIR}/src/applibs/Simulator
TOOLS_DIR = ${TOP_SRCDIR}/src/tools
MEM_DIR = ${TOP_SRCDIR}/src/base/memory

COMPILE_FLAGS = ${CPPFLAGS} -I${TOP_SRCDIR} -I${TOOLS_DIR} -I${OSAL_DIR} -I${MEM_DIR}

dynamic_modules:
		${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/debug.cc ${SHARED_CXX_FLAGS} -o data/debug.opb
