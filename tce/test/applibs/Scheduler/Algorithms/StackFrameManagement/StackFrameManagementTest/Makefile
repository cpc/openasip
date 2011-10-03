DIST_OBJECTS = OldFrontendStackAnalyzer.o StackCodeRemover.o StackFrameCodeCreator.o
TOOL_OBJECTS = *.o
MACH_OBJECTS = *.o
PROG_OBJECTS = *.o
TPEF_OBJECTS = *.o
OSAL_OBJECTS = *.o
SCHED_LIB_OBJECTS = *.o 
UMACH_LIB_OBJS = *.o
#PDG_LIB_OBJECTS = *.o
#GRAPH_LIB_OBJECTS = *.o
#CFG_LIB_OBJECTS = *.o
DISASSEMBLER_LIB_OBJS = *.o


TOP_SRCDIR=../../../../../../


include ${TOP_SRCDIR}/test/Makefile_configure_settings 

EXTRA_LINKER_FLAGS = ${SQLITE_LD_FLAGS} ${XERCES_LDFLAGS}
EXTRA_COMPILER_FLAGS = ${LLVM_CPPFLAGS}
include ${TOP_SRCDIR}/test/Makefile_test.defs
