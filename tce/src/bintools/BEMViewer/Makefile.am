PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

LIB_TCETOOLS_DIR = ../../tools
LIB_BEM_DIR = ../../base/bem
ROOT_DIR = ../../..

BASE_DIR = ${SRC_ROOT_DIR}/base
BEM_DIR = ${BASE_DIR}/bem
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

bin_PROGRAMS = viewbem
viewbem_SOURCES = ViewBEM.cc BEMViewerCmdLineOptions.cc

viewbem_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_CPPFLAGS = -I${TOOLS_DIR} -I${ROOT_DIR} -I${BASE_DIR} \
        -I${BEM_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

