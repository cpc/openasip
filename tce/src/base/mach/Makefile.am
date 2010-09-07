
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
        SpecialRegisterPort.cc NullAddressSpace.cc NullBus.cc                \
	NullInstructionTemplate.cc  \
        NullRegisterFile.cc NullMachine.cc          \
	DummyMachineTester.cc

libmach_la_LDFLAGS = ${XERCES_LIBDIR} -lxerces-c -lpthread

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config

AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools -I${PROJECT_ROOT}/src/base/osal \
	      -I${PROJECT_ROOT}/src/applibs/mach \
	      -I${PROJECT_ROOT}/src/applibs/FSA
AM_CXXFLAGS = -UNDEBUG

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: doc

MAINTAINERCLEAN = *~ *.gcov *.bbg *.bb *.da




## headers start
libmach_la_SOURCES += \
	HWOperation.hh \
	DummyMachineTester.hh ExecutionPipeline.hh \
	ControlUnit.hh Connection.hh \
	FUPort.hh Socket.hh \
	BaseRegisterFile.hh NullBus.hh \
	TemplateSlot.hh SpecialRegisterPort.hh \
	ImmediateSlot.hh Unit.hh \
	RFPort.hh PipelineElement.hh \
	AddressSpace.hh \
	Port.hh Bus.hh \
	Guard.hh MOMTextGenerator.hh \
	InstructionTemplate.hh ADFSerializer.hh \
	MachineTester.hh NullMachine.hh \
	FunctionUnit.hh MachinePart.hh \
	ADFSerializerTextGenerator.hh Bridge.hh \
	Segment.hh MachineTestReporter.hh \
	BaseFUPort.hh ImmediateUnit.hh \
	MachineTestReportTextGenerator.hh \
	NullInstructionTemplate.hh \
	RegisterFile.hh NullRegisterFile.hh \
	NullAddressSpace.hh Machine.hh \
	Socket.icc TemplateSlot.icc \
	Guard.icc PipelineElement.icc \
	Segment.icc Machine.icc \
	BaseRegisterFile.icc ControlUnit.icc \
	Connection.icc Port.icc \
	Bridge.icc MachinePart.icc 
## headers end
