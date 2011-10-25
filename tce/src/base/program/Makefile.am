PROJECT_ROOT = $(top_srcdir)

LIB_MACH_DIR = ${PROJECT_ROOT}/src/base/mach
LIB_OSAL_DIR = ${PROJECT_ROOT}/src/base/osal
LIB_TPEF_DIR = ${PROJECT_ROOT}/src/base/tpef
LIB_GRAPH_DIR = ${PROJECT_ROOT}/src/base/Graph
LIB_MEMORY_DIR = ${PROJECT_ROOT}/src/base/memory
LIB_UNIVERSAL_MACHINE_DIR = ${PROJECT_ROOT}/src/base/umach

LIB_TOOLS_DIR = ${PROJECT_ROOT}/src/tools

noinst_LTLIBRARIES = libprogram.la
libprogram_la_SOURCES = Program.cc Address.cc Instruction.cc Terminal.cc \
	TerminalImmediate.cc TerminalRegister.cc TerminalFUPort.cc \
	Immediate.cc MoveGuard.cc Procedure.cc TPEFProgramFactory.cc Move.cc\
	CodeSnippet.cc NullProcedure.cc NullProgram.cc NullInstruction.cc \
	NullAddress.cc Label.cc CodeLabel.cc DataLabel.cc Scope.cc \
	GlobalScope.cc TerminalAddress.cc TerminalInstructionAddress.cc \
	InstructionReference.cc InstructionReferenceManager.cc \
	InstructionReferenceImpl.cc \
	NullGlobalScope.cc NullInstructionReferenceManager.cc \
	ProgramWriter.cc DataMemory.cc DataDefinition.cc \
	DataInstructionAddressDef.cc DataAddressDef.cc ProgramAnnotation.cc \
	AnnotatedInstructionElement.cc Disassembler.cc DisassemblyElement.cc \
	DisassemblyOperand.cc DisassemblyMove.cc DisassemblyInstruction.cc \
	DisassemblyRegister.cc DisassemblyFUPort.cc DisassemblyFUOpcodePort.cc \
	DisassemblyIntRegister.cc DisassemblyFPRegister.cc DisassemblyImmediate.cc \
	DisassemblyGuard.cc DisassemblyBoolRegister.cc POMDisassembler.cc \
	DisassemblyImmediateRegister.cc TPEFDisassembler.cc \
	DisassemblyReturnAddressRegister.cc DisassemblyControlUnitPort.cc \
	DisassemblyInstructionSlot.cc DisassemblyImmediateAssignment.cc \
	DisassemblyLabel.cc DisassemblyNOP.cc DisassemblyFUOperand.cc \
	DisassemblySequentialGuard.cc DisassemblyAnnotation.cc CodeGenerator.cc \
    TerminalBasicBlockReference.cc BasicBlock.cc TerminalSymbolReference.cc \
	MoveNode.cc MoveNodeSet.cc ProgramOperation.cc TerminalProgramOperation.cc \
	TerminalInstructionReference.cc

DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${LIB_TOOLS_DIR} -I${LIB_MACH_DIR} -I${LIB_OSAL_DIR} \
	-I../ -I${LIB_TPEF_DIR} -I${LIB_UNIVERSAL_MACHINE_DIR} \
	-I${LIB_MEMORY_DIR} -I${LIB_GRAPH_DIR} ${LLVM_CPPFLAGS}
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da *.gcno *.gcda

## headers start
libprogram_la_SOURCES += \
	DisassemblySequentialGuard.hh Program.hh \
	CodeGenerator.hh TPEFProgramFactory.hh \
	DisassemblyGuard.hh DisassemblyFUOperand.hh \
	Address.hh Scope.hh \
	DataLabel.hh DataDefinition.hh \
	Disassembler.hh Label.hh \
	DisassemblyImmediateRegister.hh InstructionReferenceImpl.hh \
	DisassemblyBoolRegister.hh DisassemblyControlUnitPort.hh \
	DisassemblyInstruction.hh DisassemblyMove.hh \
	MoveGuard.hh DisassemblyNOP.hh \
	DisassemblyFPRegister.hh Move.hh \
	MoveNodeSet.hh InstructionReferenceManager.hh \
	AnnotatedInstructionElement.hh DisassemblyRegister.hh \
	Procedure.hh NullProcedure.hh \
	DisassemblyImmediate.hh MoveNode.hh \
	NullGlobalScope.hh BasicBlock.hh \
	DisassemblyIntRegister.hh DisassemblyElement.hh \
	DisassemblyFUPort.hh DisassemblyInstructionSlot.hh \
	ProgramWriter.hh TerminalAddress.hh \
	DisassemblyAnnotation.hh ProgramOperation.hh \
	CodeLabel.hh TPEFDisassembler.hh \
	Terminal.hh TerminalRegister.hh \
	DisassemblyFUOpcodePort.hh DataAddressDef.hh \
	POMDisassembler.hh Immediate.hh \
	NullProgram.hh DisassemblyOperand.hh \
	TerminalImmediate.hh DataInstructionAddressDef.hh \
	TerminalInstructionAddress.hh NullAddress.hh \
	TerminalProgramOperation.hh CodeSnippet.hh \
	DataMemory.hh NullInstructionReferenceManager.hh \
	TerminalSymbolReference.hh ProgramAnnotation.hh \
	InstructionReference.hh Instruction.hh \
	TerminalBasicBlockReference.hh NullInstruction.hh \
	DisassemblyReturnAddressRegister.hh DisassemblyLabel.hh \
	DisassemblyImmediateAssignment.hh TerminalInstructionReference.hh \
	TerminalFUPort.hh GlobalScope.hh \
	MoveNode.icc InstructionReferenceManager.icc \
	TerminalRegister.icc Address.icc \
	Procedure.icc InstructionReferenceImpl.icc 
## headers end
