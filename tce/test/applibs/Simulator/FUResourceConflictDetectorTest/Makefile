TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = MachineState.o MachineStateBuilder.o BusState.o \
	FUState.o RegisterFileState.o RegisterState.o UnboundRegisterFileState.o \
	InputPortState.o OutputPortState.o TriggeringInputPortState.o \
	OpcodeSettingVirtualInputPortState.o SimpleOperationExecutor.o \
	OneCycleOperationExecutor.o LongImmediateUnitState.o \
	../../base/umach/UniversalMachine.o \
	../../base/umach/UniversalFunctionUnit.o \
	ClockedState.o StateData.o ReadableState.o WritableState.o \
	OperationExecutor.o LongImmediateRegisterState.o PortState.o \
	GCUState.o MemorySystem.o SimulatorToolbox.o \
	MemoryAccessingFUState.o ../../base/umach/UnboundedRegisterFile.o \
	../../base/umach/SmartHWOperation.o \
	../../base/umach/UniversalFUPort.o StateLocator.o TransportPipeline.o \
	SimulatorTextGenerator.o GlobalLock.o SimulationEventHandler.o \
	GuardState.o \
	FUResourceConflictDetector.o FSAFUResourceConflictDetector.o \
	ResourceVectorFUResourceConflictDetector.o \
	ConflictDetectingOperationExecutor.o
TOOL_OBJECTS = *.o
OSAL_OBJECTS = *.o
FSA_OBJECTS = *.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o
MACH_OBJECTS = *.o
MEMORY_OBJECTS = *.o

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} ${BOOST_LDFLAGS} ${DL_FLAGS} \
	${DYNAMIC_FLAG}

INITIALIZATION = build_base

include ${TOP_SRCDIR}/test/Makefile_test.defs

build_base:
	cd ${TOP_SRCDIR}/opset/base; make
