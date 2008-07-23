BUILD_PASS = ../../../src/bintools/Scheduler/buildschedulerpass

SRC_ROOT_DIR = ../../..
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/src/applibs
SCHED_DIR = ${APPLIBS_DIR}/Scheduler
ALGO_DIR = ${SCHED_DIR}/Algorithms
PDG_DIR = ${SCHED_DIR}/ProgramRepresentations/ProgramDependenceGraph
CFG_DIR = ${SCHED_DIR}/ProgramRepresentations/ControlFlowGraph

basedir = ${prefix}/share/tce/scheduler/passes/SequentialScheduler/
nobase_base_DATA = SequentialSchedulerModule.so 

.PHONY: ${nobase_base_DATA}

all-local: SequentialSchedulerModule.so 

SequentialSchedulerModule.so: SequentialSchedulerModule.cc \
	SequentialSchedulerModule.hh	
	${BUILD_PASS} SequentialSchedulerModule.so SequentialSchedulerModule.cc

clean:
	rm -f *.so *~
