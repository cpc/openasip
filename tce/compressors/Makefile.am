pkglibdir = ${prefix}/share/tce/codecompressors/base
pkglib_LTLIBRARIES = SimpleDictionary.la

SimpleDictionary_la_SOURCES = SimpleDictionary.cc
SimpleDictionary_la_LDFLAGS = -module

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TPEF_DIR = ${SRC_ROOT_DIR}/base/tpef
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
UMACH_DIR = ${SRC_ROOT_DIR}/base/umach
PROG_DIR = ${SRC_ROOT_DIR}/base/program
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
BEM_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/bem
PIG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/PIG
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

AM_CPPFLAGS = \
        -I${MACH_DIR} -I${UMACH_DIR} -I${OSAL_DIR} -I${TPEF_DIR} -I${BEM_DIR} \
		-I${BEM_APPLIBS_DIR} -I${PIG_APPLIBS_DIR} \
        -I${TOOLS_DIR} -I${PROG_DIR} -I${MACH_APPLIBS_DIR} 
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS}


all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running ProGe from the source tree to find them.
	@@(for soname in $(shell ls -1 ${srcdir}/.libs/*.so); do \
		ln -sf $$soname ${srcdir}; \
	done)

clean-local:
	rm -f ${srcdir}/*.so
