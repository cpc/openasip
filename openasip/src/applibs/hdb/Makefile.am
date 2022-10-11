noinst_LTLIBRARIES = libhdb.la
libhdb_la_SOURCES = HDBManager.cc FUEntry.cc RFEntry.cc FUArchitecture.cc \
FUExternalPort.cc FUImplementation.cc HWBlockImplementation.cc \
FUPortImplementation.cc RFPortImplementation.cc PortImplementation.cc \
RFImplementation.cc RFArchitecture.cc BlockImplementationFile.cc \
HDBRegistry.cc CostFunctionPlugin.cc HDBEntry.cc HWBlockArchitecture.cc \
CostEstimationData.cc CachedHDBManager.cc HDBTester.cc ExternalPort.cc \
RFExternalPort.cc

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


include_HEADERS = \
	CachedHDBManager.hh \
	FUEntry.hh \
	FUImplementation.hh \
	FUPortImplementation.hh \
	FUArchitecture.hh



## headers start
libhdb_la_SOURCES += \
	HWBlockArchitecture.hh HWBlockImplementation.hh \
	RFPortImplementation.hh FUExternalPort.hh \
	CachedHDBManager.hh FUPortImplementation.hh \
	HDBTypes.hh FUImplementation.hh \
	BlockImplementationFile.hh RFArchitecture.hh \
	HDBManager.hh FUEntry.hh \
	RFEntry.hh CostFunctionPlugin.hh \
	CostEstimationData.hh HDBTester.hh \
	RFImplementation.hh HDBRegistry.hh \
	PortImplementation.hh FUArchitecture.hh \
	HDBEntry.hh ExternalPort.hh RFExternalPort.hh \
	CostEstimationData.icc 
## headers end
