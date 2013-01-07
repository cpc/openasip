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

include_HEADERS = Program.hh Address.hh Address.icc GlobalScope.hh Scope.hh \
	DataLabel.hh Label.hh 

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
	DisassemblyNOP.hh DisassemblyLabel.hh \
	TPEFProgramFactory.hh POMDisassembler.hh \
	Address.hh CodeLabel.hh \
	DisassemblyOperand.hh TerminalInstructionReference.hh \
	DisassemblyGuard.hh DataLabel.hh \
	Immediate.hh NullProgram.hh \
	DisassemblyFUOpcodePort.hh TerminalAddress.hh \
	DataAddressDef.hh DataDefinition.hh \
	MoveNode.hh CodeGenerator.hh \
	Instruction.hh Label.hh \
	TPEFDisassembler.hh DisassemblyImmediateRegister.hh \
	Scope.hh TerminalRegister.hh \
	TerminalFUPort.hh DisassemblyInstruction.hh \
	CodeSnippet.hh DisassemblyFUOperand.hh \
	DisassemblyInstructionSlot.hh NullProcedure.hh \
	Procedure.hh DisassemblyFPRegister.hh \
	DisassemblyBoolRegister.hh BasicBlock.hh \
	TerminalSymbolReference.hh TerminalInstructionAddress.hh \
	InstructionReference.hh MoveNodeSet.hh \
	TerminalBasicBlockReference.hh ProgramOperation.hh \
	DisassemblyFUPort.hh DisassemblyElement.hh \
	InstructionReferenceManager.hh Terminal.hh \
	DisassemblyControlUnitPort.hh NullGlobalScope.hh \
	TerminalProgramOperation.hh Program.hh \
	Disassembler.hh DataMemory.hh \
	Move.hh DisassemblyMove.hh \
	DisassemblyImmediate.hh NullInstruction.hh \
	DisassemblyIntRegister.hh MoveGuard.hh \
	AnnotatedInstructionElement.hh DataInstructionAddressDef.hh \
	NullAddress.hh InstructionReferenceImpl.hh \
	DisassemblyAnnotation.hh DisassemblyReturnAddressRegister.hh \
	NullInstructionReferenceManager.hh ProgramAnnotation.hh \
	DisassemblyRegister.hh GlobalScope.hh \
	DisassemblySequentialGuard.hh DisassemblyImmediateAssignment.hh \
	TerminalImmediate.hh ProgramWriter.hh \
	MoveNode.icc TerminalRegister.icc \
	InstructionReferenceManager.icc Address.icc \
	Procedure.icc InstructionReferenceImpl.icc 
## headers end
