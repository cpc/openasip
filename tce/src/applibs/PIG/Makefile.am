noinst_LTLIBRARIES = libpig.la
libpig_la_SOURCES = AsciiImageWriter.cc \
	BitVector.cc CodeCompressorPlugin.cc \
	InstructionBitVector.cc PIGTextGenerator.cc \
	ProgramImageGenerator.cc RawImageWriter.cc \
	AsciiProgramImageWriter.cc ArrayProgramImageWriter.cc \
	ArrayImageWriter.cc MifImageWriter.cc VhdlProgramImageWriter.cc \
	VhdlImageWriter.cc CoeImageWriter.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src
BASE_DIR = ${SRC_ROOT_DIR}/base

TPEF_DIR = ${BASE_DIR}/tpef
MACH_DIR = ${BASE_DIR}/mach
BEM_DIR = ${BASE_DIR}/bem
PROG_DIR = ${BASE_DIR}/program
UM_DIR = ${SRC_ROOT_DIR}/base/umach
OSAL_DIR = ${BASE_DIR}/osal

TOOLS_DIR = ${SRC_ROOT_DIR}/tools

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${PROG_DIR} -I${TPEF_DIR} \
	-I${BEM_DIR} -I${UM_DIR} -I${OSAL_DIR} ${LLVM_CPPFLAGS}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_PYTHON_LIB}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libpig_la_SOURCES += \
	InstructionBitVector.hh PIGTextGenerator.hh \
	VhdlImageWriter.hh ArrayImageWriter.hh \
	CoeImageWriter.hh RawImageWriter.hh \
	AsciiImageWriter.hh AsciiProgramImageWriter.hh \
	VhdlProgramImageWriter.hh DefaultCompressor.hh \
	BitVector.hh ProgramImageGenerator.hh \
	MifImageWriter.hh BitImageWriter.hh \
	CodeCompressor.hh ArrayProgramImageWriter.hh \
	CodeCompressorPlugin.hh 
## headers end
