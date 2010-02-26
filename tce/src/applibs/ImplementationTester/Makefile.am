noinst_LTLIBRARIES = libimpltester.la
libimpltester_la_SOURCES =  \
	ImplementationTester.cc FUTestbenchGenerator.cc ImplementationSimulator.cc \
	GhdlSimulator.cc ModelsimSimulator.cc RFTestbenchGenerator.cc \
	TestbenchGenerator.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs

MACH_DIR = ${BASE_DIR}/mach
MEM_DIR = ${BASE_DIR}/memory
OSAL_DIR = ${BASE_DIR}/osal
HDB_DIR = ${APPLIBS_DIR}/hdb
MACH_SIM_DIR = ${APPLIBS_DIR}/Simulator
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${OSAL_DIR} -I${HDB_DIR} \
	-I${MACH_SIM_DIR} -I${MEM_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

#AM_LDFLAGS = ${BOOST_PYTHON_LIB}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
libimpltester_la_SOURCES += \
	ImplementationTester.hh FUTestbenchGenerator.hh ImplementationSimulator.hh \
	GhdlSimulator.hh ModelsimSimulator.hh RFTestbenchGenerator.hh \
	TestbenchGenerator.hh
## headers end
