LIB_TCETOOLS_DIR = ../../tools
LIB_BASE_DIR = ../../base
ROOT_DIR = ../../..

BEM_DIR = ${LIB_BASE_DIR}/bem

bin_PROGRAMS = viewbem
viewbem_SOURCES = ViewBEM.cc BEMViewerCmdLineOptions.cc

viewbem_LDADD = ${BEM_DIR}/libbem.la \
        ${LIB_TCETOOLS_DIR}/libtcetools.la

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} -I${ROOT_DIR} -I${LIB_BASE_DIR} \
        -I${BEM_DIR}

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} \
	${XERCES_LIBS} ${EDITLINE_LD_FLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

