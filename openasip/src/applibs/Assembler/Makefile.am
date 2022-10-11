noinst_LTLIBRARIES = libassembler.la
libassembler_la_SOURCES = CodeSectionCreator.cc DataSectionCreator.cc \
LabelManager.cc AssemblerParser.cc MachineResourceManager.cc Assembler.cc \
AssemblyParserDiagnostic.cc

SRC_ROOT = $(top_srcdir)/src

TOOLS_DIR = ${SRC_ROOT}/tools
MACH_DIR = ${SRC_ROOT}/base/mach
TPEF_DIR  = ${SRC_ROOT}/base/tpef
OSAL_DIR = ${SRC_ROOT}/base/osal
AM_CPPFLAGS = -I${TOOLS_DIR} -I${TPEF_DIR} -I${MACH_DIR} -I${OSAL_DIR}

libassembler_la_LDFLAGS = 

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libassembler_la_SOURCES += \
	Assembler.hh DataSectionCreator.hh \
	AssemblerParser.hh MachineResourceManager.hh \
	LabelManager.hh CodeSectionCreator.hh \
	ParserStructs.hh AssemblyParserDiagnostic.hh
## headers end
