LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
LIB_APPLIBS_DIR = ../../applibs
ROOT_DIR = ../../..

TPEF_DIR = ../../base/tpef
MACH_DIR = ../../base/mach
BEM_DIR = ../../base/bem
BEM_APPLIBS_DIR = ../../applibs/bem
PIG_APPLIBS_DIR = ../../applibs/PIG
MACH_APPLIBS_DIR = ../../applibs/mach

bin_PROGRAMS = generatebits 
bin_SCRIPTS = buildcompressor
generatebits_SOURCES = PIGCmdLineOptions.cc PIGCLITextGenerator.cc \
	GenerateBits.cc

generatebits_LDADD = ${LIB_BASE_DIR}/libttabase.la \
        ${LIB_TCETOOLS_DIR}/libtcetools.la ${LIB_APPLIBS_DIR}/libapplibs.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} \
        -I${MACH_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
	-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR}

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
