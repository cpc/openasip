DIST_OBJECTS = OperationBehavior.o OperationContext.o OperationState.o \
               Operation.o Operand.o
TOOL_OBJECTS = PluginTools.o FileSystem.o Application.o Exception.o \
               Environment.o SimValue.o Conversion.o ObjectState.o \
	       StringTools.o
MEMORY_OBJECTS = Memory.o TargetMemory.o IdealSRAM.o
TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

#-rdynamic adds all linked symbols to the main application so
#          we don't need to link all the stuff to every shared object
EXTRA_LINKER_FLAGS = ${DL_FLAGS} ${DYNAMIC_FLAG} ${BOOST_LDFLAGS}
INITIALIZATION = dynamic_modules

include ${TOP_SRCDIR}/test/Makefile_test.defs

OSAL_DIR = ${TOP_SRCDIR}/src/base/osal
SIMLIB_DIR = ${TOP_SRCDIR}/src/applibs/Simulator
TOOLS_DIR = ${TOP_SRCDIR}/src/tools
MEM_DIR = ${TOP_SRCDIR}/src/base/memory

COMPILE_FLAGS = -I${TOP_SRCDIR} -I${TOOLS_DIR} -I${OSAL_DIR} \
                -I${TOP_SRCDIR}/src/base/memory -I${TOOLS_DIR} ${CPPFLAGS}

# 'dynamic_modules' rule compiles the dynamic modules representing OSAL
# behavior definitions.
#
# Note that we must link the symbols that the dynamic modules need
# into the dynamic modules otherwise dlopen() fails thanks to the RTLD_NOW
# flag. Symbols needed by tests are in object files listed in ${MISC_OBJ}.
dynamic_modules:
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/clockedOperation.cc \
	${SHARED_CXX_FLAGS} -o data/clockedOperation.so
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/simple.cc \
	${SHARED_CXX_FLAGS} -o data/simple.so
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/operationsWithState.cc \
	${SHARED_CXX_FLAGS} -o data/operationsWithState.so
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/bwidth.cc \
	${SHARED_CXX_FLAGS} -o data/bwidth.so
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/control.cc \
	${SHARED_CXX_FLAGS} -o data/control.so
	${CXX} ${COMPILE_FLAGS} ${MISC_OBJ} data/extend.cc \
	${SHARED_CXX_FLAGS} -o data/extend.so
