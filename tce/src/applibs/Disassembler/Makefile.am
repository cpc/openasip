noinst_LTLIBRARIES = libdisassembler.la
libdisassembler_la_SOURCES = Disassembler.cc DisassemblyElement.cc \
DisassemblyOperand.cc DisassemblyMove.cc DisassemblyInstruction.cc \
DisassemblyRegister.cc DisassemblyFUPort.cc DisassemblyFUOpcodePort.cc \
DisassemblyIntRegister.cc DisassemblyFPRegister.cc DisassemblyImmediate.cc \
DisassemblyGuard.cc DisassemblyBoolRegister.cc POMDisassembler.cc \
DisassemblyImmediateRegister.cc TPEFDisassembler.cc \
DisassemblyReturnAddressRegister.cc DisassemblyControlUnitPort.cc \
DisassemblyInstructionSlot.cc DisassemblyImmediateAssignment.cc \
DisassemblyLabel.cc DisassemblyNOP.cc DisassemblyFUOperand.cc \
DisassemblySequentialGuard.cc DisassemblyAnnotation.cc

SRC_ROOT = $(top_srcdir)/src

TOOLS_DIR = ${SRC_ROOT}/tools
MACH_DIR = ${SRC_ROOT}/base/mach
TPEF_DIR  = ${SRC_ROOT}/base/tpef
OSAL_DIR = ${SRC_ROOT}/base/osal
PROG_DIR  = ${SRC_ROOT}/base/program
UMACH_DIR = ${SRC_ROOT}/base/umach
AM_CPPFLAGS = -I${TOOLS_DIR} -I${TPEF_DIR} -I${PROG_DIR} -I${MACH_DIR} \
-I${UMACH_DIR} -I${OSAL_DIR}

libdisassembler_la_LDFLAGS = 

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libdisassembler_la_SOURCES += \
	DisassemblySequentialGuard.hh DisassemblyGuard.hh \
	DisassemblyFUOperand.hh Disassembler.hh \
	DisassemblyImmediateRegister.hh DisassemblyBoolRegister.hh \
	DisassemblyControlUnitPort.hh DisassemblyInstruction.hh \
	DisassemblyMove.hh DisassemblyNOP.hh \
	DisassemblyFPRegister.hh DisassemblyRegister.hh \
	DisassemblyImmediate.hh DisassemblyIntRegister.hh \
	DisassemblyElement.hh DisassemblyFUPort.hh \
	DisassemblyInstructionSlot.hh DisassemblyAnnotation.hh \
	TPEFDisassembler.hh DisassemblyFUOpcodePort.hh \
	POMDisassembler.hh DisassemblyOperand.hh \
	DisassemblyReturnAddressRegister.hh DisassemblyLabel.hh \
	DisassemblyImmediateAssignment.hh 
## headers end
