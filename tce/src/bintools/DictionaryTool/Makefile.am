LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_APPLIBS_DIR = ../../applibs
ROOT_DIR = ../../..

TPEF_DIR = ../../base/tpef
PROGRAM_DIR = ../../base/program
MACH_DIR = ../../base/mach
BEM_DIR = ../../base/bem
BEM_APPLIBS_DIR = ../../applibs/bem
MACH_APPLIBS_DIR = ../../applibs/mach
UMACH_DIR = ../../base/umach
OSAL_DIR = ../../base/osal

bin_PROGRAMS = dictionary_tool
dictionary_tool_SOURCES = DictionaryTool.cc

dictionary_tool_LDADD = ${LIB_BASE_DIR}/libttabase.la \
        ${LIB_TCETOOLS_DIR}/libtcetools.la ${LIB_APPLIBS_DIR}/libapplibs.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} \
        -I${MACH_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
	-I${BEM_APPLIBS_DIR} -I${MACH_APPLIBS_DIR} \
	-I${PROGRAM_DIR} -I${UMACH_DIR} -I${OSAL_DIR}

AM_LDFLAGS = ${BOOST_LIBDIR} ${BOOST_PYTHON_LIB} ${SQLITE_LIBDIR} \
    ${TCL_LD_FLAGS} ${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
