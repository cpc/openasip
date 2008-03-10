TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = GCUState.o FUState.o WritableState.o ReadableState.o \
	ClockedState.o OperationExecutor.o SimulatorToolbox.o PortState.o \
	RegisterState.o StateData.o TransportPipeline.o InputPortState.o \
	TriggeringInputPortState.o OpcodeSettingVirtualInputPortState.o \
	GlobalLock.o SimulatorTextGenerator.o SimulationEventHandler.o
TOOL_OBJECTS = SimValue.o ObjectState.o Exception.o Application.o Conversion.o \
	StringTools.o PluginTools.o XMLSerializer.o FileSystem.o \
	DOMBuilderErrorHandler.o Environment.o TextGenerator.o Informer.o \
	Listener.o
OSAL_OBJECTS = OperationContext.o Operation.o OperationPool.o Operand.o \
	OperationIndex.o OperationBehavior.o OperationSerializer.o \
	OperationBehaviorLoader.o OperationModule.o OperationBehaviorProxy.o \
	OperationState.o
MEMORY_OBJECTS = Memory.o TargetMemory.o 

EXTRA_LINKER_FLAGS = ${XERCES_LDFLAGS} ${BOOST_LDFLAGS} ${DL_FLAGS} \
	${DYNAMIC_FLAG}

include ${TOP_SRCDIR}/test/Makefile_test.defs
