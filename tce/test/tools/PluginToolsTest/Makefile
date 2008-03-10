DIST_OBJECTS = PluginTools.o FileSystem.o Application.o Exception.o \
	Environment.o Conversion.o StringTools.o
TOP_SRCDIR = ../../..
INITIALIZATION = foo

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${BOOST_LDFLAGS} ${DL_FLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs

foo:
	${CXX} data/foo.cc ${SHARED_CXX_FLAGS} data/foo.so
	${CXX} data/foo2.cc ${SHARED_CXX_FLAGS} data/foo2.so	
	${CXX} data/clashing1.cc ${SHARED_CXX_FLAGS} data/clashing1.so
	${CXX} data/clashing2.cc ${SHARED_CXX_FLAGS} data/clashing2.so


