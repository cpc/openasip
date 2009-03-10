PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_APPLIBS_DIR = ../../applibs
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..

TOOLS_DIR = ${SRC_ROOT_DIR}/tools

MACH_DIR = ${SRC_ROOT_DIR}/base/mach
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
APPLIBS_BEM_DIR = ${SRC_ROOT_DIR}/applibs/bem

bin_PROGRAMS = createbem
createbem_SOURCES = CreateBEM.cc BEMGeneratorCmdLineOptions.cc \
	BEMGeneratorCLITextGenerator.cc

createbem_LDADD = ${SRC_ROOT_DIR}/libtce.la

createbem-static: CreateBEM.o BEMGeneratorCmdLineOptions.o BEMGeneratorCLITextGenerator.o
	g++ -o createbem-static \
	CreateBEM.o \
	BEMGeneratorCmdLineOptions.o \
	BEMGeneratorCLITextGenerator.o \
	../../base/mach/*.o \
	../../base/bem/*.o \
	../../tools/*.o \
	../../applibs/bem/*.o \
	../../applibs/mach/ResourceVector.o \
	../../applibs/mach/ResourceVectorSet.o \
	-lxerces-c -lsqlite3 -ldl ${BOOST_LDFLAGS}

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${LIB_BASE_DIR} \
        -I${MACH_DIR} -I${BEM_DIR} -I${APPLIBS_BEM_DIR} -I${TOOLS_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
CLEANFILES = createbem-static
