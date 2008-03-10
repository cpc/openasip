TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = LongImmediateUnitState.o LongImmediateRegisterState.o \
	ClockedState.o StateData.o ReadableState.o WritableState.o
TOOL_OBJECTS = SimValue.o ObjectState.o Exception.o Application.o Conversion.o \
	XMLSerializer.o FileSystem.o DOMBuilderErrorHandler.o Environment.o

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
