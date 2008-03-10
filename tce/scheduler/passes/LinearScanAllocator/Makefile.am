BUILD_PASS = ../../../src/bintools/Scheduler/buildschedulerpass

SRC_ROOT_DIR = ../../../../
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applib
SCHED_DIR = ${APPLIBS_DIR}/Scheduler
PDG_DIR = ${SCHED_DIR}/ProgramRepresentations/ProgramDependenceGraph
CFG_DIR = ${SCHED_DIR}/ProgramRepresentations/ControlFlowGraph
DDG_DIR = ${SCHED_DIR}/ProgramRepresentations/DataDependenceGraph

basedir = ${prefix}/share/tce/scheduler/passes/LinearScanAllocator
nobase_base_DATA = SimpleGuardAllocatorPass.so \
LinearScanAllocatorPass.so AddressSpaceMapperPass.so \
OldFrontendStackAnalyzerPass.so 

.PHONY: ${nobase_base_DATA}

all-local: SimpleGuardAllocatorPass.so AddressSpaceMapperPass.so \
LinearScanAllocatorPass.so OldFrontendStackAnalyzerPass.so 

LinearScanAllocatorPass.so: LinearScanAllocatorPass.cc \
Makefile LinearScanAllocatorPass.hh
	${BUILD_PASS} LinearScanAllocatorPass.so LinearScanAllocatorPass.cc

SimpleGuardAllocatorPass.so: SimpleGuardAllocatorPass.cc \
SimpleGuardAllocatorPass.hh Makefile
	${BUILD_PASS} SimpleGuardAllocatorPass.so SimpleGuardAllocatorPass.cc

OldFrontendStackAnalyzerPass.so: OldFrontendStackAnalyzerPass.cc \
OldFrontendStackAnalyzerPass.hh
	${BUILD_PASS} OldFrontendStackAnalyzerPass.so \
OldFrontendStackAnalyzerPass.cc

AddressSpaceMapperPass.so: AddressSpaceMapperPass.hh AddressSpaceMapperPass.cc 
	${BUILD_PASS} AddressSpaceMapperPass.so AddressSpaceMapperPass.cc

clean:
	rm -f *.so *~
