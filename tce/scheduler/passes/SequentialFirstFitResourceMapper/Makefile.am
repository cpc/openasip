BUILD_PASS = ../../../src/bintools/Scheduler/buildschedulerpass

basedir = ${prefix}/share/tce/scheduler/passes/SequentialFirstFitResourceMapper
nobase_base_DATA = DirectMappingRegisterAllocator.so SequentialFirstFitResourceAllocator.so

.PHONY: DirectMappingRegisterAllocator.so SequentialFirstFitResourceAllocator.so

all-local: DirectMappingRegisterAllocator.so SequentialFirstFitResourceAllocator.so

DirectMappingRegisterAllocator.so: DirectMappingRegisterAllocator.cc
	${BUILD_PASS} DirectMappingRegisterAllocator.so DirectMappingRegisterAllocator.cc

SequentialFirstFitResourceAllocator.so: SequentialFirstFitResourceAllocator.cc
	${BUILD_PASS} SequentialFirstFitResourceAllocator.so SequentialFirstFitResourceAllocator.cc

clean:
	rm -f *.so *~
