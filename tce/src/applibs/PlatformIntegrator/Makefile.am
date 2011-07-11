noinst_LTLIBRARIES = libplatformintegrator.la
libplatformintegrator_la_SOURCES = \
	Stratix2DSPBoardIntegrator.cc PlatformIntegrator.cc MemoryGenerator.cc  \
	AlteraOnchipRamGenerator.cc AlteraOnchipRomGenerator.cc VhdlRomGenerator.cc \
	ProjectFileGenerator.cc QuartusProjectGenerator.cc Stratix2SramGenerator.cc \
	HDLPort.cc KoskiIntegrator.cc \
	AlteraHibiDpRamGenerator.cc DefaultProjectFileGenerator.cc \
	IPXactFileGenerator.cc IPXactSerializer.cc IPXactModel.cc \
	AlteraIntegrator.cc AvalonIntegrator.cc SOPCBuilderFileGenerator.cc \
	SOPCInterface.cc AvalonMMMasterInterface.cc IPXactInterface.cc \
	IPXactHibiInterface.cc IPXactClkInterface.cc IPXactResetInterface.cc \
	Vlnv.cc AlteraMemGenerator.cc IPXactAddressSpace.cc \
	Stratix3DevKitIntegrator.cc


SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs

# check these later on
MACH_DIR = ${BASE_DIR}/mach
BEM_DIR = ${BASE_DIR}/bem
IDF_DIR = ${BASE_DIR}/idf
HDB_DIR = ${APPLIBS_DIR}/hdb
PROGE_APPLIBS_DIR = ${APPLIBS_DIR}/ProGe
ESTIMATOR_DIR = $(srcdir)/../Estimator
BEM_APPLIBS_DIR = ${APPLIBS_DIR}/bem
MACH_APPLIBS_DIR = ${APPLIBS_DIR}/mach
IDF_APPLIBS_DIR = ${APPLIBS_DIR}/idf
TOOLS_DIR = ${SRC_ROOT_DIR}/tools

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${BEM_DIR} -I${ESTIMATOR_DIR} \
-I${IDF_DIR} -I${HDB_DIR} -I${BEM_APPLIBS_DIR} -I${MACH_APPLIBS_DIR} \
-I${IDF_APPLIBS_DIR} -I${PROGE_APPLIBS_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libplatformintegrator_la_SOURCES += \
	IPXactFileGenerator.hh IPXactInterface.hh \
	SOPCBuilderFileGenerator.hh SOPCInterface.hh \
	PlatformIntegrator.hh AvalonIntegrator.hh \
	HDLPort.hh \
	Vlnv.hh DefaultProjectFileGenerator.hh \
	IPXactClkInterface.hh QuartusProjectGenerator.hh \
	IPXactModel.hh MemoryGenerator.hh \
	AlteraIntegrator.hh IPXactHibiInterface.hh \
	AlteraOnchipRomGenerator.hh AlteraOnchipRamGenerator.hh \
	Stratix2SramGenerator.hh KoskiIntegrator.hh \
	Stratix2DSPBoardIntegrator.hh AvalonMMMasterInterface.hh \
	AlteraHibiDpRamGenerator.hh IPXactSerializer.hh \
	ProjectFileGenerator.hh IPXactResetInterface.hh \
	VhdlRomGenerator.hh AlteraMemGenerator.hh IPXactAddressSpace.hh \
	Stratix3DevKitIntegrator.hh PlatformIntegratorTypes.hh
## headers end
