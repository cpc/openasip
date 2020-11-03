
noinst_LTLIBRARIES = libmach.la
libmach_la_SOURCES =							     \
	AddressSpace.cc	Guard.cc Socket.cc HWOperation.cc ImmediateUnit.cc   \
	Port.cc Unit.cc Bus.cc RegisterFile.cc Connection.cc                 \
	Machine.cc FunctionUnit.cc Bridge.cc MachinePart.cc FUPort.cc        \
	BaseRegisterFile.cc ExecutionPipeline.cc PipelineElement.cc          \
	InstructionTemplate.cc TemplateSlot.cc Segment.cc ControlUnit.cc     \
	ADFSerializer.cc MachineTester.cc MachineTestReportTextGenerator.cc  \
	MachineTestReporter.cc MOMTextGenerator.cc ImmediateSlot.cc          \
	ADFSerializerTextGenerator.cc BaseFUPort.cc RFPort.cc                \
        SpecialRegisterPort.cc NullAddressSpace.cc \
	NullInstructionTemplate.cc  \
        NullRegisterFile.cc NullMachine.cc          \
	DummyMachineTester.cc ResourceVector.cc ResourceVectorSet.cc

libmach_la_LDFLAGS = ${XERCES_LIBDIR} -lxerces-c ${PTHREAD_LIBS}

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools -I${PROJECT_ROOT}/src/base/osal \
	      -I${PROJECT_ROOT}/src/applibs/mach \
	      -I${PROJECT_ROOT}/src/applibs/FSA -I$(PROJECT_ROOT)/src/tools
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: doc

MAINTAINERCLEAN = *~ *.gcov *.bbg *.bb *.da

## Install all headers of the mach library so it can be used from 
## external projects.
include_HEADERS = \
	HWOperation.hh DummyMachineTester.hh \
	ExecutionPipeline.hh ControlUnit.hh \
	Connection.hh FUPort.hh \
	Socket.hh BaseRegisterFile.hh \
	TemplateSlot.hh \
	SpecialRegisterPort.hh ImmediateSlot.hh \
	Unit.hh RFPort.hh \
	PipelineElement.hh AddressSpace.hh \
	Port.hh Bus.hh \
	Guard.hh MOMTextGenerator.hh \
	InstructionTemplate.hh ADFSerializer.hh \
	MachineTester.hh NullMachine.hh \
	FunctionUnit.hh MachinePart.hh \
	ADFSerializerTextGenerator.hh Bridge.hh \
	Segment.hh MachineTestReporter.hh \
	BaseFUPort.hh ImmediateUnit.hh \
	MachineTestReportTextGenerator.hh NullInstructionTemplate.hh \
	RegisterFile.hh NullRegisterFile.hh \
	NullAddressSpace.hh Machine.hh \
	Socket.icc TemplateSlot.icc \
	Guard.icc PipelineElement.icc \
	Segment.icc Machine.icc \
	BaseRegisterFile.icc ControlUnit.icc \
	Connection.icc Port.icc \
	Bridge.icc MachinePart.icc 

## headers start
libmach_la_SOURCES += \
	AddressSpace.hh Socket.hh \
	DummyMachineTester.hh ImmediateUnit.hh \
	RegisterFile.hh Machine.hh \
	Bus.hh Guard.hh \
	NullMachine.hh BaseRegisterFile.hh \
	ImmediateSlot.hh NullRegisterFile.hh \
	ControlUnit.hh HWOperation.hh \
	Port.hh PipelineElement.hh \
	ADFSerializerTextGenerator.hh InstructionTemplate.hh \
	TemplateSlot.hh MOMTextGenerator.hh \
	NullInstructionTemplate.hh MachineTester.hh \
	BaseFUPort.hh ADFSerializer.hh \
	ExecutionPipeline.hh FunctionUnit.hh \
	Unit.hh NullAddressSpace.hh \
	Connection.hh FUPort.hh \
	RFPort.hh Bridge.hh \
	SpecialRegisterPort.hh MachineTestReportTextGenerator.hh \
	Segment.hh MachinePart.hh \
	MachineTestReporter.hh ControlUnit.icc \
	PipelineElement.icc Machine.icc \
	TemplateSlot.icc Guard.icc \
	Bridge.icc Connection.icc \
	Segment.icc Port.icc \
	Socket.icc MachinePart.icc \
	BaseRegisterFile.icc \
	ResourceVector.hh ResourceVectorSet.hh AutomagicTools.hh
## headers end
