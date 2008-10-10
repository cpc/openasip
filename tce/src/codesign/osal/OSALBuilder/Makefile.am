PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
OSAL_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/osal
MACH_APPLIB_DIR = ${SRC_ROOT_DIR}/applibs/mach
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb

LIB_TCETOOLS_DIR = ../../../tools
LIB_BASE_DIR = ../../../base
LIB_OSAL_APPLIB_DIR = ../../../applibs/osal
UM_DIR = ../../../base/umach
LIB_MACH_APPLIB_DIR = ../../../applibs/mach
LIB_HDB_DIR = ../../../applibs/hdb
APPLIBS_FSA_DIR = ../../../applibs/FSA

bin_PROGRAMS = buildopset
buildopset_SOURCES = BuildOpset.cc

# -l switches must be after libraries, which needs them 
# (for some stupido linkers)
buildopset_LDADD = ${LIB_TCETOOLS_DIR}/libtcetools.la \
	${UM_DIR}/libuniversalmachine.la \
	${LIB_BASE_DIR}/libttabase.la \
	${LIB_OSAL_APPLIB_DIR}/libapplibsosal.la \
	${LIB_MACH_APPLIB_DIR}/libapplibsmach.la \
	${LIB_HDB_DIR}/libhdb.la \
	${APPLIBS_FSA_DIR}/libfsa.la \
	-lxerces-c ${DL_FLAGS} -lpthread ${BOOST_LDFLAGS}

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} \
	      -I${OSAL_APPLIB_DIR} -I${MACH_APPLIB_DIR} -I${HDB_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
