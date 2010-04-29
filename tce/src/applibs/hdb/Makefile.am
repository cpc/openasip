noinst_LTLIBRARIES = libhdb.la
libhdb_la_SOURCES = HDBManager.cc FUEntry.cc RFEntry.cc FUArchitecture.cc \
FUExternalPort.cc FUImplementation.cc HWBlockImplementation.cc \
FUPortImplementation.cc RFPortImplementation.cc PortImplementation.cc \
RFImplementation.cc RFArchitecture.cc BlockImplementationFile.cc \
HDBRegistry.cc CostFunctionPlugin.cc HDBEntry.cc HWBlockArchitecture.cc \
CostEstimationData.cc CachedHDBManager.cc HDBTester.cc

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

IMPL_TEST_DIR = ${SRC_ROOT_DIR}/applibs/ImplementationTester
MACH_SIM_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal

AM_CPPFLAGS = -I${PROJECT_ROOT} -I${PROJECT_ROOT}/src/tools \
	-I${PROJECT_ROOT}/src/base/mach -I${PROJECT_ROOT}/src/applibs/mach \
	-I${IMPL_TEST_DIR} -I${MACH_SIM_DIR} -I${OSAL_DIR}

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile




## headers start
libhdb_la_SOURCES += \
	FUArchitecture.hh RFPortImplementation.hh \
	FUPortImplementation.hh HWBlockImplementation.hh \
	CachedHDBManager.hh RFEntry.hh \
	RFArchitecture.hh HDBEntry.hh \
	HDBManager.hh PortImplementation.hh \
	CostEstimationData.hh BlockImplementationFile.hh \
	HWBlockArchitecture.hh FUEntry.hh \
	RFImplementation.hh FUImplementation.hh \
	HDBRegistry.hh HDBTester.hh \
	HDBTypes.hh FUExternalPort.hh \
	CostFunctionPlugin.hh CostEstimationData.icc 
## headers end
