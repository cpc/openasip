bin_PROGRAMS = schedule
bin_SCRIPTS = buildschedulerpass
schedule_SOURCES = Schedule.cc

SRC_ROOT_DIR = ../..
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
SCHEDULER_DIR = ${APPLIBS_DIR}/Scheduler
POM_DIR = ${BASE_DIR}/program
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach

schedule_LDADD = ${BASE_DIR}/libttabase.la ${APPLIBS_DIR}/libapplibs.la \
	${TOOLS_DIR}/libtcetools.la

AM_CPPFLAGS = -I${SCHEDULER_DIR} -I${TOOLS_DIR} -I${MACH_DIR} -I${UM_DIR} \
	-I${OSAL_DIR} -I${POM_DIR}
AM_LDFLAGS = ${XERCES_LIBS} ${DL_FLAGS} ${TCL_LD_FLAGS} ${EDITLINE_LD_FLAGS} ${PYTHON_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
