DIST_OBJECTS = ScopeSelector.o
TOOL_OBJECTS = *.o
MACH_OBJECTS = *.o
PROG_OBJECTS = *.o
TPEF_OBJECTS = *.o
OSAL_OBJECTS = *.o
SCHED_LIB_OBJECTS = *.o
UMACH_LIB_OBJS = *.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o

TOP_SRCDIR = ../../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

EXTRA_LINKER_FLAGS = ${SQLITE_LD_FLAGS} ${XERCES_LDFLAGS}
EXTRA_COMPILER_FLAGS = ${LLVM_CPPFLAGS}
include ${TOP_SRCDIR}/test/Makefile_test.defs
