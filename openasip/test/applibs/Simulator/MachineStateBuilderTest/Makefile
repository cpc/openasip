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
	GuardState.o ConflictDetectingOperationExecutor.o
TOOL_OBJECTS = XMLSerializer.o ObjectState.o Exception.o Application.o \
	Conversion.o DOMBuilderErrorHandler.o Environment.o StringTools.o \
	PluginTools.o FileSystem.o TextGenerator.o SimValue.o Informer.o \
	Listener.o
MACH_OBJECTS = ADFSerializer.o Machine.o FunctionUnit.o Bus.o \
	RegisterFile.o Port.o HWOperation.o MachinePart.o \
	MOMTextGenerator.o MachineTester.o ExecutionPipeline.o Socket.o Unit.o \
	BaseRegisterFile.o Segment.o Connection.o MachineTestReporter.o \
	FUPort.o ControlUnit.o MachineTestReportTextGenerator.o \
	Guard.o PipelineElement.o Bridge.o ImmediateUnit.o \
	InstructionTemplate.o TemplateSlot.o ADFSerializerTextGenerator.o \
	AddressSpace.o BaseFUPort.o RFPort.o SpecialRegisterPort.o \
	ImmediateSlot.o DummyMachineTester.o
OSAL_OBJECTS = OperationPool.o OperationBehavior.o OperationIndex.o \
	OperationBehaviorLoader.o OperationSerializer.o Operand.o Operation.o \
	OperationModule.o OperationBehaviorProxy.o OperationContext.o \
	OperationState.o
MEMORY_OBJECTS = Memory.o TargetMemory.o IdealSRAM.o 
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} ${BOOST_LDFLAGS} ${DL_FLAGS} \
	${DYNAMIC_FLAG}

EXTRA_COMPILER_FLAGS = -I${TOP_SRCDIR}/src/base/umach

INITIALIZATION = build_base

include ${TOP_SRCDIR}/test/Makefile_test.defs

build_base:
	cd ${TOP_SRCDIR}/opset/base; make
