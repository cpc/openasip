TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = $(shell cd ${OBJDIR}; ls -1 *.o)
MACH_OBJECTS = *.o
TOOL_OBJECTS = *.o
PROG_OBJECTS = *.o
TPEF_OBJECTS = *.o
UMACH_LIB_OBJS = *.o
OSAL_OBJECTS = *.o
APPLIBS_MACH_OBJS = *.o

EXTRA_COMPILER_LFAGS =
EXTRA_LINKER_FLAGS = ${TCL_LD_FLAGS} ${BOOST_LDFLAGS} ${XERCES_LDFLAGS}

INITIALIZATION = init
FINALIZATION = cleanup

.PHONY: init
.PHONY: cleanup

include ${TOP_SRCDIR}/test/Makefile_test.defs

init:
	@cp -f data/oldHDB1.hdb data/tmp_1.hdb
	@cp -f data/oldHDB1.hdb data/tmp_2.hdb

cleanup:
	@rm -f data/newHDB*.hdb data/tmp_*.hdb

