DIST_OBJECTS = DisassemblyElement.o DisassemblyFUPort.o \
	DisassemblyMove.o DisassemblyImmediate.o DisassemblyOperand.o \
	DisassemblyFPRegister.o DisassemblyInstruction.o \
	DisassemblyRegister.o DisassemblyFUOpcodePort.o \
	DisassemblyIntRegister.o DisassemblyGuard.o DisassemblyBoolRegister.o \
	DisassemblyImmediateRegister.o DisassemblyControlUnitPort.o \
	DisassemblyInstructionSlot.o DisassemblyImmediateAssignment.o

TOOL_OBJECTS = Conversion.o Exception.o StringTools.o SimValue.o \
	Application.o ObjectState.o

TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${DL_FLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
