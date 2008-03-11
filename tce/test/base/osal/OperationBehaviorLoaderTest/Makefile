DIST_OBJECTS = OperationSerializer.o Operation.o OperationBehavior.o \
	Operand.o OperationModule.o OperationBehaviorLoader.o \
	OperationIndex.o OperationState.o OperationContext.o
TOOL_OBJECTS = Conversion.o ObjectState.o XMLSerializer.o \
	Exception.o DOMBuilderErrorHandler.o FileSystem.o Environment.o \
	PluginTools.o SimValue.o Application.o StringTools.o
MEMORY_OBJECTS = Memory.o 
TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

#-rdynamic adds all linked symbols to the main application so
#          we don't need to link all the stuff to every shared object
EXTRA_LINKER_FLAGS = -lxerces-c -lpthread ${DL_FLAGS} ${DYNAMIC_FLAG} \
	${BOOST_LDFLAGS}

INITIALIZATION = dynamic_modules

include ${TOP_SRCDIR}/test/Makefile_test.defs

OSAL_DIR = ${TOP_SRCDIR}/src/base/osal
SIMLIB_DIR = ${TOP_SRCDIR}/src/applibs/Simulator
TOOLS_DIR = ${TOP_SRCDIR}/src/tools
MEM_DIR = ${TOP_SRCDIR}/src/base/memory

COMPILE_FLAGS = ${CPPFLAGS} -I${TOP_SRCDIR} -I${TOOLS_DIR} -I${OSAL_DIR} \
                -I${MEM_DIR}

dynamic_modules:
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/correct.cc \
	${SHARED_CXX_FLAGS} -o data/correct.opb
