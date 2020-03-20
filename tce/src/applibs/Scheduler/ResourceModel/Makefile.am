noinst_LTLIBRARIES = libresourcemodel.la
libresourcemodel_la_SOURCES = BusResource.cc ExecutionPipelineResource.cc \
	FUResource.cc InputFUResource.cc InputPSocketResource.cc \
	ITemplateResource.cc IUResource.cc OutputFUResource.cc \
	OutputPSocketResource.cc PSocketResource.cc SchedulingResource.cc \
	ShortImmPSocketResource.cc \
	ExecutionPipelineResourceTable.cc

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
APPLIBS_MACH_DIR = ${SRC_ROOT_DIR}/applibs/mach
POM_DIR = ${BASE_DIR}/program
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UMACH_DIR = ${BASE_DIR}/umach
GRAPH_DIR = ${BASE_DIR}/Graph
DISASM_DIR = ${APPLIBS_DIR}/Disassembler
PDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/PDG
DDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/DDG
RM_DIR = ${APPLIBS_DIR}/Scheduler/ResourceManager
AM_CPPFLAGS = -I${TOOLS_DIR} -I${PDG_DIR} -I${POM_DIR} -I${OSAL_DIR} \
-I${MACH_DIR} -I${UMACH_DIR} -I${GRAPH_DIR} -I${DISASM_DIR} -I${DDG_DIR} -I${RM_DIR} \
-I${APPLIBS_MACH_DIR} ${LLVM_CPPFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libresourcemodel_la_SOURCES += \
	OutputPSocketResource.hh SchedulingResource.hh \
	OutputFUResource.hh ExecutionPipelineResource.hh \
	IUResource.hh PSocketResource.hh \
	BusResource.hh InputFUResource.hh \
	ITemplateResource.hh ExecutionPipelineResourceTable.hh \
	ShortImmPSocketResource.hh \
	FUResource.hh InputPSocketResource.hh \
	ExecutionPipelineResourceTable.icc SchedulingResource.icc 
## headers end
