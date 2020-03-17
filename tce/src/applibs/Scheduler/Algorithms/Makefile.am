SUBDIRS = 

noinst_LTLIBRARIES = libscheduleralgorithms.la
libscheduleralgorithms_la_SOURCES = BasicBlockScheduler.cc \
BasicBlockPass.cc RegisterCopyAdder.cc \
SchedulerPass.cc ControlFlowGraphPass.cc ProgramPass.cc ProcedurePass.cc \
InterPassData.cc SoftwareBypasser.cc CycleLookBackSoftwareBypasser.cc \
DDGPass.cc CopyingDelaySlotFiller.cc \
RegisterRenamer.cc LoopPrologAndEpilogBuilder.cc \
SequentialScheduler.cc PreBypassBasicBlockScheduler.cc \
BBSchedulerController.cc PreOptimizer.cc ResourceConstraintAnalyzer.cc \
BUBasicBlockScheduler.cc \
PostpassOperandSharer.cc \
BF2Scheduler.cc BFOptimization.cc BFScheduleTD.cc BFScheduleBU.cc \
BFScheduleExact.cc BFRegCopyAfter.cc BFRegCopyBefore.cc BFRegCopy.cc \
BFLateBypasses.cc BFLateBypass.cc BFEarlyBypass.cc \
BFDRELate.cc BFDREEarly.cc BF2ScheduleFront.cc \
BFPushAntidepsDown.cc BFPushAntidepDown.cc \
BFShareOperand.cc BFShareOperandsLate.cc BFShareOperandLate.cc \
BFSwapOperands.cc BFShareOperands.cc BFShareOperandWithScheduled.cc \
MoveNodeDuplicator.cc BFKillNode.cc \
BFSchedulePreLoopShared.cc BFDropPreShared.cc \
BFPushDepsUp.cc \
BFPushMoveUp.cc BFRescheduleMove.cc BFUnscheduleMove.cc \
BFScheduleMove.cc BFLoopBypass.cc BFEarlyBypasser.cc  \
BFRemoveGuardFromSucc.cc BFRemoveGuardsFromSuccs.cc \
BFRescheduleResultsClose.cc BFRescheduleResultClose.cc \
BFRescheduleSuccessorsClose.cc BFDRELoop.cc \
BFPushMoveUp2.cc \
BFPostpassDRE.cc \
BFPostpassBypass.cc BFPostpassBypasser.cc  \
BFPostpassLoopDRE.cc BFUnscheduleFromBody.cc BFAssignToBody.cc \
BFRemoveGuard.cc BFTryRemoveGuard.cc \
BFRemoveEdge.cc BFRenameSource.cc BFRenameLiveRange.cc \
BFMergeAndKeepUser.cc BFConnectNodes.cc BFUpdateMoveOnBypass.cc \
BFInsertLiveRangeUse.cc BFClearLiveRangeUse.cc



libscheduleralgorithms_la_LIBADD = 

SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
POM_DIR = ${BASE_DIR}/program
APPLIBS_POM_DIR = ${APPLIBS_DIR}/program
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
SELECTOR_DIR = ${APPLIBS_DIR}/Scheduler/Selector
GRAPH_DIR = ${BASE_DIR}/Graph
DISASM_DIR = ${APPLIBS_DIR}/Disassembler
LLVM_BACKEND_DIR = ${APPLIBS_DIR}/LLVMBackend

AM_CPPFLAGS = -I${RESOURCE_MODEL_DIR} -I${TOOLS_DIR} -I${POM_DIR} \
	-I${OSAL_DIR} -I${MACH_DIR} -I${GRAPH_DIR} -I${SCHED_APPLIBS_DIR} \
	-I${RESOURCE_MODEL_DIR} -I${RESOURCE_MANAGER_DIR} -I${CFG_DIR} \
	-I${DDG_DIR} -I${PDG_DIR} -I${DISASM_DIR} -I${SELECTOR_DIR} \
	-I${UM_DIR} -I${MACH_APPLIB_DIR} -I${LLVM_BACKEND_DIR} \
        -I${APPLIBS_POM_DIR} \
	-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




include_HEADERS = InterPassDatum.hh

## headers start
libscheduleralgorithms_la_SOURCES += \
	RegisterRenamer.hh SchedulerPass.hh \
	CycleLookBackSoftwareBypasser.hh ControlFlowGraphPass.hh \
	ResourceConstraintAnalyzer.hh BUBasicBlockScheduler.hh \
	RegisterCopyAdder.hh BasicBlockPass.hh \
	SoftwareBypasser.hh BasicBlockScheduler.hh \
	PreBypassBasicBlockScheduler.hh \
	ProgramPass.hh CopyingDelaySlotFiller.hh \
	PreOptimizer.hh InterPassData.hh \
	BBSchedulerController.hh ProcedurePass.hh \
	InterPassDatum.hh SequentialScheduler.hh \
	DDGPass.hh PostpassOperandSharer.hh
## headers end
