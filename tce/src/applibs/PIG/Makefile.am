noinst_LTLIBRARIES = libpig.la
libpig_la_SOURCES = AsciiImageWriter.cc \
	BitVector.cc CodeCompressorPlugin.cc \
	InstructionBitVector.cc PIGTextGenerator.cc \
	ProgramImageGenerator.cc RawImageWriter.cc \
	AsciiProgramImageWriter.cc ArrayProgramImageWriter.cc \
	ArrayImageWriter.cc MifImageWriter.cc VhdlProgramImageWriter.cc \
	VhdlImageWriter.cc CoeImageWriter.cc IndexBound.cc

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
	-I${BEM_DIR} -I${UM_DIR} -I${OSAL_DIR} \
        -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_PYTHON_LIB}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libpig_la_SOURCES += \
	ArrayProgramImageWriter.hh ProgramImageGenerator.hh \
	AsciiImageWriter.hh DefaultCompressor.hh \
	AsciiProgramImageWriter.hh VhdlProgramImageWriter.hh \
	ArrayImageWriter.hh RawImageWriter.hh \
	BitVector.hh PIGTextGenerator.hh \
	VhdlImageWriter.hh CodeCompressorPlugin.hh \
	BitImageWriter.hh CoeImageWriter.hh \
	CodeCompressor.hh MifImageWriter.hh \
	InstructionBitVector.hh IndexBound.hh
## headers end
