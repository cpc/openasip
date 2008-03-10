TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = BEMGenerator.o
BEM_OBJECTS = *.o
MACH_OBJECTS = *.o
TOOL_OBJECTS = *.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o

EXTRA_COMPILER_LFAGS = 
EXTRA_LINKER_FLAGS = ${TCL_LD_FLAGS} ${BOOST_LDFLAGS} ${XERCES_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
