PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_APPLIBS_DIR = ../../applibs
ROOT_DIR = ../../..

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

bin_PROGRAMS = generatebits 
bin_SCRIPTS = buildcompressor
generatebits_SOURCES = PIGCmdLineOptions.cc PIGCLITextGenerator.cc \
	GenerateBits.cc

generatebits_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} \
        -I${MACH_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
	-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} \
        -I${TOOLS_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
generatebits_SOURCES += \
	PIGCLITextGenerator.hh PIGCmdLineOptions.hh 
## headers end
