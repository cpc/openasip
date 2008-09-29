
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
	NullFunctionUnit.cc NullImmediateUnit.cc NullInstructionTemplate.cc  \
        NullPort.cc NullRegisterFile.cc NullMachine.cc NullGuard.cc          \
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
