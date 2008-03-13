TOP_SRCDIR = ../../../..

disable_watchdog = yes

INITIALIZATION = dynamic_modules

include ${TOP_SRCDIR}/test/Makefile_test.defs

OSAL_DIR = ${TOP_SRCDIR}/src/base/osal
TOOLS_DIR = ${TOP_SRCDIR}/src/tools
MEM_DIR = ${TOP_SRCDIR}/src/base/memory

COMPILE_FLAGS = ${CPPFLAGS} -I${TOP_SRCDIR} -I${TOOLS_DIR} -I${OSAL_DIR} \
	-I${MEM_DIR}

dynamic_modules:
	@${CXX} ${COMPILE_FLAGS} data/example.cc ${SHARED_CXX_FLAGS} -o \
	data/example.opb
	@${CXX} ${COMPILE_FLAGS} data/mips_r4000.cc ${SHARED_CXX_FLAGS} -o \
	data/mips_r4000.opb

