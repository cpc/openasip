pkglibdir = ${prefix}/share/openasip/codecompressors/base
pkglib_LTLIBRARIES = InstructionDictionary.la MoveSlotDictionary.la

InstructionDictionary_la_SOURCES = InstructionDictionary.cc
InstructionDictionary_la_LDFLAGS = -module -version-info ${LIB_VERSION}

MoveSlotDictionary_la_SOURCES = MoveSlotDictionary.cc
MoveSlotDictionary_la_LDFLAGS = -module -version-info ${LIB_VERSION}


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

AM_LDFLAGS = ${BOOST_LIBDIR} ${SQLITE_LIBDIR} ${TCL_LD_FLAGS} ${XERCES_LIBDIR} ${XERCES_LIBS} \
	${EDITLINE_LD_FLAGS} -module -version-info ${LIB_VERSION}


all-local: ${pkglib_LTLIBRARIES}
# Create links to the .so to the current directory just to make 
# running ProGe from the source tree to find them.
	@@(for soname in `ls -1 ./.libs/*.so`; do \
		ln -sf $$soname .; \
	done)

clean-local:
	rm -f ./*.so
