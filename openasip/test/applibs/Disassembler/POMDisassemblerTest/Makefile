DIST_OBJECTS = DisassemblyElement.o DisassemblyFUPort.o \
	DisassemblyMove.o DisassemblyImmediate.o DisassemblyOperand.o \
	DisassemblyFPRegister.o DisassemblyInstruction.o \
	DisassemblyRegister.o DisassemblyFUOpcodePort.o \
	DisassemblyIntRegister.o DisassemblyGuard.o \
	DisassemblyBoolRegister.o DisassemblyImmediateRegister.o \
	POMDisassembler.o Disassembler.o DisassemblyControlUnitPort.o \
	DisassemblyInstructionSlot.o DisassemblyImmediateAssignment.o \
        DisassemblyLabel.o DisassemblyNOP.o DisassemblySequentialGuard.o

MACH_OBJECTS = *.o
OSAL_OBJECTS = *.o
PROG_OBJECTS = *.o
TOOL_OBJECTS = *.o
UMACH_LIB_OBJS = *.o
TPEF_OBJECTS = *.o
MEMORY_OBJECTS = *.o
SIMULATOR_LIB_OBJS = *.o
INTERPRETER_LIB_OBJS = *.o
TRACEDB_LIB_OBJS = *.o
APPLIBS_PROG_OBJS = *.o
FSA_OBJECTS = *.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o

TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = 
EXTRA_COMPILER_FLAGS = ${LLVM_CPPFLAGS}
include ${TOP_SRCDIR}/test/Makefile_test.defs
