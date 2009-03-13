noinst_LTLIBRARIES = libresourcemanager.la
libresourcemanager_la_SOURCES = AssignmentPlan.cc ITemplateBroker.cc \
	ResourceBuildDirector.cc BrokerDirector.cc IUBroker.cc \
	ResourceManager.cc BusBroker.cc OutputFUBroker.cc ResourceMapper.cc \
	ExecutionPipelineBroker.cc OutputPSocketBroker.cc SegmentBroker.cc \
	InputFUBroker.cc PendingAssignment.cc SimpleBrokerDirector.cc \
	InputPSocketBroker.cc ResourceBroker.cc SimpleResourceManager.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UMACH_DIR = ${BASE_DIR}/umach
UM_DIR = ${BASE_DIR}/umach
DISASM_DIR = ${APPLIBS_DIR}/Disassembler
RESOURCE_MODEL_DIR = ${APPLIBS_DIR}/Scheduler/ResourceModel
PDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/PDG
GRAPH_DIR = ${BASE_DIR}/Graph

AM_CPPFLAGS = -I${RESOURCE_MODEL_DIR} -I${PDG_DIR} -I${TOOLS_DIR} \
	-I${POM_DIR} -I${OSAL_DIR} -I${MACH_DIR} -I${UMACH_DIR} -I${GRAPH_DIR} \
	-I${DISASM_DIR} -I${APPLIBS_DIR}/mach

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
