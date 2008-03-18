BUILD_PASS = ../../../src/bintools/Scheduler/buildschedulerpass

SRC_ROOT_DIR = ../../..
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/src/applibs
SCHED_DIR = ${APPLIBS_DIR}/Scheduler
ALGO_DIR = ${SCHED_DIR}/Algorithms
PDG_DIR = ${SCHED_DIR}/ProgramRepresentations/ProgramDependenceGraph
CFG_DIR = ${SCHED_DIR}/ProgramRepresentations/ControlFlowGraph
DDG_DIR = ${SCHED_DIR}/ProgramRepresentations/DataDependenceGraph

basedir = ${prefix}/share/tce/scheduler/passes/BasicBlockScheduler
nobase_base_DATA = BasicBlockSchedulerPass.so CLBBypasserModule.so CopyingDSFillerModule.so

.PHONY: ${nobase_base_DATA}

all-local: BasicBlockSchedulerPass.so CLBBypasserModule.so \
	CopyingDSFillerModule.so

BasicBlockSchedulerPass.so: BasicBlockSchedulerPass.cc \
	BasicBlockSchedulerPass.hh ${ALGO_DIR}/DSFillerModule.hh \
	${ALGO_DIR}/BypasserModule.hh
	${BUILD_PASS} BasicBlockSchedulerPass.so BasicBlockSchedulerPass.cc

CLBBypasserModule.so:  CLBBypasserModule.cc CLBBypasserModule.hh \
	${ALGO_DIR}/BypasserModule.hh
	${BUILD_PASS} CLBBypasserModule.so CLBBypasserModule.cc

CopyingDSFillerModule.so: CopyingDSFillerModule.cc CopyingDSFillerModule.hh \
	${ALGO_DIR}/DSFillerModule.hh 
	${BUILD_PASS} CopyingDSFillerModule.so CopyingDSFillerModule.cc

clean:
	rm -f *.so *~
