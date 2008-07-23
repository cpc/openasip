SUBDIRS = StackFrameManagement

lib_LTLIBRARIES = libscheduleralgorithms.la
libscheduleralgorithms_la_SOURCES = BasicBlockScheduler.cc \
AddressSpaceMapper.cc SimpleGuardAllocatorCore.cc RegisterMap.cc \
LinearScanAllocatorCore.cc BasicBlockPass.cc RegisterCopyAdder.cc \
SchedulerPass.cc ControlFlowGraphPass.cc ProgramPass.cc ProcedurePass.cc \
InterPassData.cc SoftwareBypasser.cc CycleLookBackSoftwareBypasser.cc \
DDGPass.cc CopyingDelaySlotFiller.cc BypasserModule.cc DSFillerModule.cc \
SequentialScheduler.cc


libscheduleralgorithms_la_LIBADD = \
StackFrameManagement/libstackframemanagement.la

SRC_ROOT_DIR = ../../..
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MACH_DIR = ${BASE_DIR}/mach
UM_DIR = ${BASE_DIR}/umach
SCHED_APPLIBS_DIR = ${APPLIBS_DIR}/Scheduler
RESOURCE_MODEL_DIR = ${APPLIBS_DIR}/Scheduler/ResourceModel
RESOURCE_MANAGER_DIR = ${APPLIBS_DIR}/Scheduler/ResourceManager
PDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/ProgramDependenceGraph
CFG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/ControlFlowGraph
DDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/DataDependenceGraph
MACH_APPLIB_DIR = ${APPLIBS_DIR}/mach
SFM_DIR = StackFrameManagement
SELECTOR_DIR = ${APPLIBS_DIR}/Scheduler/Selector
GRAPH_DIR = ${BASE_DIR}/Graph
DISASM_DIR = ${APPLIBS_DIR}/Disassembler


AM_CPPFLAGS = -I${RESOURCE_MODEL_DIR} -I${TOOLS_DIR} -I${POM_DIR} \
	-I${OSAL_DIR} -I${MACH_DIR} -I${GRAPH_DIR} -I${SCHED_APPLIBS_DIR} \
	-I${RESOURCE_MODEL_DIR} -I${RESOURCE_MANAGER_DIR} -I${CFG_DIR} \
	-I${DDG_DIR} -I${PDG_DIR} -I${DISASM_DIR} -I${SELECTOR_DIR} -I${SFM_DIR} \
	-I${UM_DIR} -I${MACH_APPLIB_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
