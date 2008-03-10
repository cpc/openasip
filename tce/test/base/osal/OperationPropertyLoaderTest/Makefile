DIST_OBJECTS = OperationSerializer.o Operation.o OperationBehavior.o \
	Operand.o OperationPropertyLoader.o OperationModule.o \
	OperationContext.o OperationState.o
TOOL_OBJECTS = Conversion.o Application.o ObjectState.o XMLSerializer.o \
	Exception.o DOMBuilderErrorHandler.o FileSystem.o Environment.o \
	StringTools.o SimValue.o
MEMORY_OBJECTS = Memory.o 
TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = -lxerces-c -lpthread ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
