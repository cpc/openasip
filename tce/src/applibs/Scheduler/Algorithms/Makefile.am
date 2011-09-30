SUBDIRS = StackFrameManagement

noinst_LTLIBRARIES = libscheduleralgorithms.la
libscheduleralgorithms_la_SOURCES = BasicBlockScheduler.cc \
AddressSpaceMapper.cc SimpleGuardAllocatorCore.cc RegisterMap.cc \
LinearScanAllocatorCore.cc BasicBlockPass.cc RegisterCopyAdder.cc \
SchedulerPass.cc ControlFlowGraphPass.cc ProgramPass.cc ProcedurePass.cc \
InterPassData.cc SoftwareBypasser.cc CycleLookBackSoftwareBypasser.cc \
DDGPass.cc CopyingDelaySlotFiller.cc BypasserModule.cc DSFillerModule.cc \
RegisterRenamer.cc \
SequentialScheduler.cc PreBypassBasicBlockScheduler.cc \
BBSchedulerController.cc PreOptimizer.cc ResourceConstraintAnalyzer.cc


libscheduleralgorithms_la_LIBADD = \
StackFrameManagement/libstackframemanagement.la

SRC_ROOT_DIR = $(top_srcdir)/src
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
PDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/PDG
CFG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/CFG
DDG_DIR = ${APPLIBS_DIR}/Scheduler/ProgramRepresentations/DDG
MACH_APPLIB_DIR = ${APPLIBS_DIR}/mach
SFM_DIR = $(srcdir)/StackFrameManagement
SELECTOR_DIR = ${APPLIBS_DIR}/Scheduler/Selector
GRAPH_DIR = ${BASE_DIR}/Graph
DISASM_DIR = ${APPLIBS_DIR}/Disassembler
LLVM_BACKEND_DIR = ${APPLIBS_DIR}/LLVMBackend

AM_CPPFLAGS = -I${RESOURCE_MODEL_DIR} -I${TOOLS_DIR} -I${POM_DIR} \
	-I${OSAL_DIR} -I${MACH_DIR} -I${GRAPH_DIR} -I${SCHED_APPLIBS_DIR} \
	-I${RESOURCE_MODEL_DIR} -I${RESOURCE_MANAGER_DIR} -I${CFG_DIR} \
	-I${DDG_DIR} -I${PDG_DIR} -I${DISASM_DIR} -I${SELECTOR_DIR} -I${SFM_DIR} \
	-I${UM_DIR} -I${MACH_APPLIB_DIR} -I${LLVM_BACKEND_DIR} ${LLVM_CPPFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




include_HEADERS = InterPassDatum.hh

## headers start
libscheduleralgorithms_la_SOURCES += \
	ProcedurePass.hh LinearScanAllocatorCore.hh \
	CycleLookBackSoftwareBypasser.hh SequentialScheduler.hh \
	DSFillerModule.hh InterPassData.hh \
	RegisterCopyAdder.hh CopyingDelaySlotFiller.hh \
	SchedulerPass.hh PreOptimizer.hh \
	BasicBlockPass.hh ResourceConstraintAnalyzer.hh \
	BypasserModule.hh DDGPass.hh \
	RegisterMap.hh AddressSpaceMapper.hh \
	PreBypassBasicBlockScheduler.hh SoftwareBypasser.hh \
	ControlFlowGraphPass.hh SimpleGuardAllocatorCore.hh \
	ProgramPass.hh BBSchedulerController.hh \
	BasicBlockScheduler.hh InterPassDatum.hh 
## headers end
