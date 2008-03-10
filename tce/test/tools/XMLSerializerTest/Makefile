DIST_OBJECTS = \
	XMLSerializer.o ObjectState.o Exception.o DOMBuilderErrorHandler.o \
	FileSystem.o Application.o Environment.o
TOOL_OBJECTS = Conversion.o
TOP_SRCDIR = ../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} -lxerces-c -lpthread ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
