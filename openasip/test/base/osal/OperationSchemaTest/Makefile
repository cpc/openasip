DIST_OBJECTS = OperationBehavior.o OperationContext.o OperationState.o

TOOL_OBJECTS = DOMBuilderErrorHandler.o Conversion.o Application.o \
               Environment.o Exception.o FileSystem.o SimValue.o \
               ObjectState.o

MEMORY_OBJECTS = Memory.o 

TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = -lxerces-c -lpthread ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
