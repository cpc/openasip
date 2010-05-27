noinst_LTLIBRARIES = libplatformintegrator.la
libplatformintegrator_la_SOURCES = \
	PlatformIntegrator.cc MemoryGenerator.cc Stratix2DSPBoardIntegrator.cc \
	AlteraOnchipRamGenerator.cc AlteraOnchipRomGenerator.cc VhdlRomGenerator.cc \
	ProjectFileGenerator.cc QuartusProjectGenerator.cc Stratix2SramGenerator.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs

# check these later on
MACH_DIR = ${BASE_DIR}/mach
BEM_DIR = ${BASE_DIR}/bem
IDF_DIR = ${BASE_DIR}/idf
HDB_DIR = ${APPLIBS_DIR}/hdb
ESTIMATOR_DIR = $(srcdir)/../Estimator
BEM_APPLIBS_DIR = ${APPLIBS_DIR}/bem
MACH_APPLIBS_DIR = ${APPLIBS_DIR}/mach
IDF_APPLIBS_DIR = ${APPLIBS_DIR}/idf
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${BEM_DIR} -I${ESTIMATOR_DIR} \
-I${IDF_DIR} -I${HDB_DIR} -I${BEM_APPLIBS_DIR} -I${MACH_APPLIBS_DIR} \
-I${IDF_APPLIBS_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
libplatformintegrator_la_SOURCES += \
	PlatformIntegrator.hh MemoryGenerator.hh Stratix2DSPBoardIntegrator.hh \
	AlteraOnchipRamGenerator.hh AlteraOnchipRomGenerator.hh VhdlRomGenerator.hh \
	ProjectFileGenerator.hh QuartusProjectGenerator.hh Stratix2SramGenerator.hh
## headers end
