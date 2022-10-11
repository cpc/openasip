DIST_OBJECTS = Application.o Environment.o Exception.o StringTools.o \
	ConfigurationFile.o ProcessorConfigurationFile.o FileSystem.o \
	Conversion.o
TOP_SRCDIR = ../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${BOOST_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
