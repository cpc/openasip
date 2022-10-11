DIST_OBJECTS = MachineImplementation.o IDFSerializer.o \
UnitImplementationLocation.o

TOOL_OBJECTS = *.o
TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${BOOST_LDFLAGS} ${XERCES_LDFLAGS} \
                     ${DL_FLAGS}

INITIALIZATION = cleanthis

include ${TOP_SRCDIR}/test/Makefile_test.defs

cleanthis:
	@rm -f data/new.idf

