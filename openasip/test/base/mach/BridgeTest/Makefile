DIST_OBJECTS = AddressSpace.o BaseRegisterFile.o Bridge.o Bus.o \
	Connection.o ControlUnit.o ExecutionPipeline.o FUPort.o \
	FunctionUnit.o Guard.o HWOperation.o ImmediateUnit.o \
	InstructionTemplate.o ADFSerializer.o Machine.o MachinePart.o \
	PipelineElement.o Port.o RegisterFile.o Segment.o Socket.o \
	TemplateSlot.o Unit.o MachineTester.o MachineTestReporter.o \
	MOMTextGenerator.o MachineTestReportTextGenerator.o \
	ADFSerializerTextGenerator.o SpecialRegisterPort.o BaseFUPort.o \
        RFPort.o ImmediateSlot.o DummyMachineTester.o 

TOOL_OBJECTS = *.o
APPLIBS_MACH_OBJS = ResourceVector.o ResourceVectorSet.o
TOP_SRCDIR = ../../../..

include ${TOP_SRCDIR}/test/Makefile_configure_settings

EXTRA_LINKER_FLAGS = ${BOOST_LDFLAGS} ${XERCES_LDFLAGS} ${SQLITE_LD_FLAGS} \
                     ${DL_FLAGS}

include ${TOP_SRCDIR}/test/Makefile_test.defs
