DIST_OBJECTS = Terminal.o TerminalRegister.o MoveGuard.o Move.o \
	Instruction.o \
	Immediate.o Procedure.o NullProcedure.o Address.o Program.o \
	NullInstruction.o NullMoveGuard.o NullProgram.o NullTerminal.o \
	NullMove.o NullImmediate.o NullAddress.o Scope.o GlobalScope.o \
	Label.o CodeLabel.o DataLabel.o InstructionReferenceManager.o \
    NullGlobalScope.o NullInstructionReferenceManager.o \
	DataMemory.o DataDefinition.o DataAddressDef.o \
	DataInstructionAddressDef.o \
    InstructionReference.o TerminalImmediate.o TerminalAddress.o \
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
