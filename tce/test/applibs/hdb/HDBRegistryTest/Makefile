TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = $(shell cd ${OBJDIR}; ls -1 *.o)
MACH_OBJECTS = *.o
PROG_OBJECTS = *.o
TPEF_OBJECTS = *.o
OSAL_OBJECTS = *.o
TOOL_OBJECTS = *.o
UMACH_LIB_OBJS = *.o
APPLIBS_MACH_OBJS = *.o

EXTRA_COMPILER_LFAGS = 
EXTRA_LINKER_FLAGS = ${TCL_LD_FLAGS} ${BOOST_LDFLAGS} ${XERCES_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
