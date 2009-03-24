PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_APPLIBS_DIR = ../../applibs
ROOT_DIR = ../../..

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal

bin_PROGRAMS = dictionary_tool
dictionary_tool_SOURCES = DictionaryTool.cc

dictionary_tool_LDADD = ../../libtce.la

AM_CPPFLAGS = -I${TOOLS_DIR} \
        -I${MACH_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
	-I${BEM_APPLIBS_DIR} -I${MACH_APPLIBS_DIR} \
	-I${PROGRAM_DIR} -I${UMACH_DIR} -I${OSAL_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
