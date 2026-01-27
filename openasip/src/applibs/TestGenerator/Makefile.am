noinst_LTLIBRARIES = libtege.la
libtege_la_SOURCES = TestGeneratorBase.cc TestCase.cc \
    VerificationDataGenerator.cc ImmediateTestGenerator.cc \
    TestGeneratorCollection.cc TestFileWriter.cc FUOperationTestGenerator.cc \
    ICacheTestGenerator.cc LimmTestSnippetGenerator.cc GuardTestGenerator.cc \
    SnippetGenerator.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs

MACH_DIR = ${BASE_DIR}/mach
UMACH_DIR = ${BASE_DIR}/umach
PROG_DIR = ${BASE_DIR}/program
BEM_DIR = ${BASE_DIR}/bem
IDF_DIR = ${BASE_DIR}/idf
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
HDB_DIR = ${APPLIBS_DIR}/hdb
BEM_APPLIBS_DIR = ${APPLIBS_DIR}/bem
MACH_APPLIBS_DIR = ${APPLIBS_DIR}/mach
IDF_APPLIBS_DIR = ${APPLIBS_DIR}/idf
SIM_APPLIBS_DIR = ${APPLIBS_DIR}/Simulator
PIG_APPLIBS_DIR = ${APPLIBS_DIR}/PIG
LLVM_BACKEND_DIR = ${APPLIBS_DIR}/LLVMBackend
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
PLATFORM_DIR = ${APPLIBS_DIR}/PlatformIntegrator

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${UMACH_DIR} -I${BEM_DIR} \
    -I${IDF_DIR} -I${TPEF_DIR} -I${OSAL_DIR} -I${HDB_DIR} \
    -I${BEM_APPLIBS_DIR} -I${MACH_APPLIBS_DIR} -I${IDF_APPLIBS_DIR} \
    -I${PLATFORM_DIR} -I${PROG_DIR} -I${SIM_APPLIBS_DIR} \
    -I${PIG_APPLIBS_DIR} -I${LLVM_BACKEND_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

## headers start
libtege_la_SOURCES += \
	TestGeneratorBase.hh TestCase.hh VerificationDataGenerator.hh \
	ImmediateTestGenerator.hh TestGeneratorCollection.hh \
	TestFileWriter.hh FUOperationTestGenerator.hh \
	ICacheTestGenerator.hh LimmTestSnippetGenerator.hh \
	GuardTestGenerator.hh SnippetGenerator.hh
## headers end
