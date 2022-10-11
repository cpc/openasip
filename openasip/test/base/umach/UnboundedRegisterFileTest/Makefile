TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings 

DIST_OBJECTS = UniversalMachine.o UniversalFunctionUnit.o \
               UnboundedRegisterFile.o SmartHWOperation.o UniversalFUPort.o
OSAL_OBJECTS = OperationPool.o Operation.o Operand.o OperationModule.o \
               OperationBehavior.o OperationSerializer.o \
               OperationBehaviorProxy.o OperationBehaviorLoader.o \
               OperationIndex.o
MACH_OBJECTS = Machine.o Bus.o Segment.o Guard.o Socket.o FunctionUnit.o \
               MachinePart.o MOMTextGenerator.o MachineTester.o Unit.o \
               Port.o FUPort.o ControlUnit.o RegisterFile.o \
               BaseRegisterFile.o MachineTestReporter.o HWOperation.o \
               ExecutionPipeline.o MachineTestReportTextGenerator.o \
               ImmediateUnit.o PipelineElement.o Connection.o \
               InstructionTemplate.o TemplateSlot.o Bridge.o AddressSpace.o \
               BaseFUPort.o RFPort.o SpecialRegisterPort.o ImmediateSlot.o \
	       DummyMachineTester.o
TOOL_OBJECTS = Exception.o Application.o TextGenerator.o ObjectState.o \
               Conversion.o StringTools.o FileSystem.o XMLSerializer.o \
               Environment.o PluginTools.o DOMBuilderErrorHandler.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o
EXTRA_COMPILER_LFAGS = ${EDITLINE_INCLUDES}
EXTRA_LINKER_FLAGS = ${TCL_LD_FLAGS} ${BOOST_LDFLAGS} \
                     ${XERCES_LDFLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
