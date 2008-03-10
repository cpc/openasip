SUBDIRS = ProgramRepresentations ResourceModel ResourceManager Selector \
	Algorithms

lib_LTLIBRARIES = libscheduler.la

libscheduler_la_SOURCES = SchedulerFrontend.cc SchedulerCmdLineOptions.cc \
	BaseSchedulerModule.cc StartableSchedulerModule.cc \
	HelperSchedulerModule.cc SchedulerConfigurationSerializer.cc \
	SchedulingPlan.cc SchedulerPluginLoader.cc PythonSchedulerModule.cc 

libscheduler_la_LIBADD = ProgramRepresentations/libprogramrepresentations.la \
	ResourceModel/libresourcemodel.la \
	ResourceManager/libresourcemanager.la \
	Algorithms/libscheduleralgorithms.la \
	Selector/libselector.la

SRC_ROOT_DIR = ../../
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach


if PYTHON_ENABLED

AM_CPPFLAGS = -I${TOOLS_DIR} -I ${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${APPLIBS_DIR}/mach -I${MACH_DIR} -I${UM_DIR} ${PYTHON_CPPFLAGS} \
	-DPYTHON_ENABLED
AM_LDFLAGS = ${BOOST_PYTHON_LDFLAGS} ${PYTHON_LDFLAGS} ${PYTHON_EXTRA_LDFLAGS} ${PYTHON_EXTRA_LIBS}

else

AM_CPPFLAGS = -I${TOOLS_DIR} -I ${POM_DIR} -I${TPEF_DIR} -I${OSAL_DIR} \
	-I${MACH_DIR} -I${UM_DIR} -I${APPLIBS_DIR}/mach 

endif


dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
