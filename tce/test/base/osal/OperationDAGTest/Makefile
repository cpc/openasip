DIST_OBJECTS = OperationSerializer.o Operation.o OperationBehavior.o \
	Operand.o OperationModule.o OperationBehaviorLoader.o \
	OperationIndex.o OperationState.o OperationContext.o \
	OperationPool.o OperationBehaviorProxy.o OperationDAGBehavior.o \
	OperationDAG.o OperationDAGConverter.o

TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = -lxerces-c -lpthread ${DL_FLAGS} ${DYNAMIC_FLAG} \
	${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs

OSAL_DIR = ${TOP_SRCDIR}/src/base/osal
SIMLIB_DIR = ${TOP_SRCDIR}/src/applibs/Simulator
TOOLS_DIR = ${TOP_SRCDIR}/src/tools
MEM_DIR = ${TOP_SRCDIR}/src/base/memory
COMPILE_FLAGS = ${CPPFLAGS} -I${TOP_SRCDIR} -I${TOOLS_DIR} -I${OSAL_DIR} \
                -I${TOP_SRCDIR}/src/base/memory -I${TOOLS_DIR}
