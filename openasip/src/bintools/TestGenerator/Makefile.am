PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_APPLIBS_DIR = ../../applibs
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
PROG_DIR = ${SRC_ROOT_DIR}/base/program
TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
TEGE_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/TestGenerator
SIM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG

bin_PROGRAMS = generatetests
generatetests_SOURCES= GenerateTests.cc TestGenCmdLineOptions.cc

generatetests_LDADD = ../../libopenasip.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${LIB_BASE_DIR} \
              -I${MACH_DIR} -I${IDF_DIR} -I${TOOLS_DIR} -I${PROG_DIR} \
              -I${TEGE_APPLIBS_DIR} -I${SIM_APPLIBS_DIR} \
              -I${TPEF_DIR} -I${PIG_APPLIBS_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT}
AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

## headers start
generatetests_SOURCES += TestGenCmdLineOptions.hh
## headers end
