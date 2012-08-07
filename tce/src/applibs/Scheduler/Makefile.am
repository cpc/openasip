SUBDIRS = ProgramRepresentations ResourceModel ResourceManager Selector \
	Algorithms

noinst_LTLIBRARIES = libscheduler.la

libscheduler_la_SOURCES = SchedulerCmdLineOptions.cc
libscheduler_la_LIBADD = ProgramRepresentations/libprogramrepresentations.la \
	ResourceModel/libresourcemodel.la \
	ResourceManager/libresourcemanager.la \
	Algorithms/libscheduleralgorithms.la \
	Selector/libselector.la

include_HEADERS = SchedulerCmdLineOptions.hh

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach

AM_CPPFLAGS = -I${TOOLS_DIR} -I ${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${APPLIBS_DIR}/mach 
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libscheduler_la_SOURCES += \
	SchedulerCmdLineOptions.hh 
## headers end
