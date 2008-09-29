bin_PROGRAMS = schedule
bin_SCRIPTS = buildschedulerpass
schedule_SOURCES = Schedule.cc

LIB_BASE_DIR = ../../base
LIB_APPLIBS_DIR = ../../applibs
LIB_TOOLS_DIR = ../../tools

SRC_ROOT_DIR = $(top_srcdir)/src
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
SCHEDULER_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
POM_DIR = ${SRC_ROOT_DIR}/base/program
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
UM_DIR = ${SRC_ROOT_DIR}/base/umach

schedule_LDADD = ${LIB_BASE_DIR}/libttabase.la \
	${LIB_APPLIBS_DIR}/libapplibs.la \
	${LIB_TOOLS_DIR}/libtcetools.la

AM_CPPFLAGS = -I${SCHEDULER_DIR} -I${TOOLS_DIR} -I${MACH_DIR} -I${UM_DIR} \
	-I${OSAL_DIR} -I${POM_DIR}
AM_LDFLAGS = ${XERCES_LIBS} ${DL_FLAGS} ${TCL_LD_FLAGS} ${EDITLINE_LD_FLAGS} ${PYTHON_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
