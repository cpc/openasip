DIST_OBJECTS = Terminal.o TerminalRegister.o MoveGuard.o Move.o \
	NullMoveGuard.o NullTerminal.o NullInstruction.o Instruction.o \
	NullAddress.o NullImmediate.o NullMove.o NullProcedure.o \
	NullProgram.o Immediate.o Procedure.o Program.o \
	NullGlobalScope.o GlobalScope.o Scope.o Label.o DataLabel.o \
	CodeLabel.o InstructionReferenceManager.o InstructionReference.o \
	NullInstructionReferenceManager.o \
	DataMemory.o DataDefinition.o DataAddressDef.o \
	DataInstructionAddressDef.o \
	TerminalImmediate.o TerminalAddress.o \
	AnnotatedInstructionElement.o ProgramAnnotation.o 

MACH_OBJECTS = *.o
TOOL_OBJECTS = *.o
OSAL_OBJECTS = *.o
UMACH_LIB_OBJS = *.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o

TOP_SRCDIR = ../../../..

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} ${BOOST_LDFLAGS} ${SQLITE_LD_FLAGS} \
                     -ldl

include ${TOP_SRCDIR}/test/Makefile_configure_settings

include ${TOP_SRCDIR}/test/Makefile_test.defs
