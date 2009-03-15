PROJECT_ROOT = $(top_srcdir)

LIB_MACH_DIR = ${PROJECT_ROOT}/src/base/mach
LIB_OSAL_DIR = ${PROJECT_ROOT}/src/base/osal
LIB_TPEF_DIR = ${PROJECT_ROOT}/src/base/tpef
LIB_MEMORY_DIR = ${PROJECT_ROOT}/src/base/memory
LIB_UNIVERSAL_MACHINE_DIR = ${PROJECT_ROOT}/src/base/umach

LIB_TOOLS_DIR = ${PROJECT_ROOT}/src/tools

noinst_LTLIBRARIES = libprogram.la
libprogram_la_SOURCES = Program.cc Address.cc Instruction.cc Terminal.cc \
	TerminalImmediate.cc TerminalRegister.cc TerminalFUPort.cc \
	Immediate.cc MoveGuard.cc Procedure.cc TPEFProgramFactory.cc Move.cc\
	CodeSnippet.cc NullProcedure.cc NullProgram.cc NullInstruction.cc \
	NullMove.cc NullImmediate.cc NullMoveGuard.cc NullTerminal.cc \
	NullAddress.cc Label.cc CodeLabel.cc DataLabel.cc Scope.cc \
	GlobalScope.cc TerminalAddress.cc TerminalInstructionAddress.cc \
	InstructionReference.cc InstructionReferenceManager.cc \
	NullGlobalScope.cc NullInstructionReferenceManager.cc \
	ProgramWriter.cc DataMemory.cc DataDefinition.cc \
	DataInstructionAddressDef.cc DataAddressDef.cc ProgramAnnotation.cc \
	AnnotatedInstructionElement.cc

DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${LIB_TOOLS_DIR} -I${LIB_MACH_DIR} -I${LIB_OSAL_DIR} \
	-I../ -I${LIB_TPEF_DIR} -I${LIB_UNIVERSAL_MACHINE_DIR} \
	-I${LIB_MEMORY_DIR}
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da *.gcno *.gcda


## headers start
libprogram_la_SOURCES += \
	Address.hh AnnotatedInstructionElement.hh \
	CodeLabel.hh CodeSnippet.hh \
	DataAddressDef.hh DataDefinition.hh \
	DataInstructionAddressDef.hh DataLabel.hh \
	DataMemory.hh GlobalScope.hh \
	Immediate.hh Instruction.hh \
	InstructionReference.hh InstructionReferenceManager.hh \
	Label.hh Move.hh \
	MoveGuard.hh NullAddress.hh \
	NullGlobalScope.hh NullImmediate.hh \
	NullInstruction.hh NullInstructionReferenceManager.hh \
	NullMove.hh NullMoveGuard.hh \
	NullProcedure.hh NullProgram.hh \
	NullTerminal.hh Procedure.hh \
	Program.hh ProgramAnnotation.hh \
	ProgramWriter.hh Scope.hh \
	TPEFProgramFactory.hh Terminal.hh \
	TerminalAddress.hh TerminalFUPort.hh \
	TerminalImmediate.hh TerminalInstructionAddress.hh \
	TerminalRegister.hh Address.icc \
	InstructionReferenceManager.icc Procedure.icc \
	TerminalRegister.icc 
## headers end
