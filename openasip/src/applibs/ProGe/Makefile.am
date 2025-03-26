noinst_LTLIBRARIES = libproge.la
libproge_la_SOURCES = Netlist.cc NetlistBlock.cc NetlistPort.cc \
NetlistGenerator.cc PortConnectionProperty.cc ICDecoderGeneratorPlugin.cc \
NetlistWriter.cc VerilogNetlistWriter.cc VHDLNetlistWriter.cc ProGeUI.cc \
ProcessorGenerator.cc BlockSourceCopier.cc ProGeScriptGenerator.cc \
ProGeTestBenchGenerator.cc VirtualNetlistBlock.cc HDLTemplateInstantiator.cc \
CUOpcodeGenerator.cc \
BaseNetlistBlock.cc Parameter.cc NetlistVisualization.cc \
NetlistTools.cc InverterBlock.cc Signal.cc \
SignalGroup.cc NetlistFactories.cc \
NetlistPortGroup.cc SignalGroupDefinitions.cc \
ProGeContext.cc GlobalPackage.cc \
TestBenchBlock.cc SinglePortSSRAMBlock.cc \
ProcessorWrapperBlock.cc LoopBufferBlock.cc MemoryBusInterface.cc \
ProGeTools.cc RV32MicroCodeGenerator.cc \
SinglePortByteMaskSSRAMBlock.cc


SRC_ROOT_DIR = $(top_srcdir)/src
BASE_DIR = ${SRC_ROOT_DIR}/base
APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs

MACH_DIR = ${BASE_DIR}/mach
BEM_DIR = ${BASE_DIR}/bem
IDF_DIR = ${BASE_DIR}/idf
OSAL_DIR = ${BASE_DIR}/osal
HDB_DIR = ${APPLIBS_DIR}/hdb
ESTIMATOR_DIR = $(srcdir)/../Estimator
BEM_APPLIBS_DIR = ${APPLIBS_DIR}/bem
MACH_APPLIBS_DIR = ${APPLIBS_DIR}/mach
IDF_APPLIBS_DIR = ${APPLIBS_DIR}/idf
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
PROGE_DIR = ${SRC_ROOT_DIR}/procgen/ProGe
PLATFORM_DIR = ${APPLIBS_DIR}/PlatformIntegrator
PROGRAM_DIR = ${BASE_DIR}/program
PIG_DIR = ${APPLIBS_DIR}/PIG

AM_CPPFLAGS = -I${TOOLS_DIR} -I${MACH_DIR} -I${BEM_DIR} -I${ESTIMATOR_DIR} \
-I${IDF_DIR} -I${HDB_DIR} -I${BEM_APPLIBS_DIR} -I${MACH_APPLIBS_DIR} \
-I${IDF_APPLIBS_DIR} -I${PLATFORM_DIR} -I${OSAL_DIR} -I${PROGE_DIR} \
-I$(BASE_DIR)/Graph -I$(SRC_ROOT_DIR)/procgen/ProGe \
-I$(SRC_ROOT_DIR)/applibs/Explorer \
-I$(SRC_ROOT_DIR)/applibs/dsdb \
-I$(SRC_ROOT_DIR)/applibs/Simulator \
-I$(APPLIBS_DIR)/HWGen -I${PROGRAM_DIR} -I${PIG_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

## headers start
libproge_la_SOURCES += \
	ProGeUI.hh VerilogNetlistWriter.hh \
	HDLTemplateInstantiator.hh NetlistGenerator.hh \
	BlockSourceCopier.hh NetlistBlock.hh \
	NetlistPort.hh VHDLNetlistWriter.hh \
	ICDecoderGeneratorPlugin.hh ProcessorGenerator.hh \
	PortConnectionProperty.hh Netlist.hh \
	ProGeTestBenchGenerator.hh ProGeTypes.hh \
	VirtualNetlistBlock.hh NetlistWriter.hh \
	ProGeScriptGenerator.hh ProGeScriptGenerator.icc \
	LockUnitGenerator.hh CUOpcodeGenerator.hh \
	DataPathGateBlockGenerator.hh \
	IGenerationPhases.hh BaseNetlistBlock.hh \
	Parameter.hh NetlistVisualization.hh \
	DummyLeafBlock.hh NetlistTools.hh \
	InverterBlock.hh SignalTypes.hh \
	SignalGroupTypes.hh Signal.hh \
	SignalGroup.hh NetlistFactories.hh \
	NetlistPortGroup.hh \
	SignalGroupDefinitions.hh \
	ProGeContext.hh \
	GlobalPackage.hh \
	TestBenchBlock.hh \
	ProcessorWrapperBlock.hh LoopBufferBlock.hh \
	MemoryBusInterface.hh \
	GeneratableFUNetlistBlock.hh \
	GeneratableRFNetlistBlock.hh \
	OperationPool.hh Operation.hh Operand.hh \
	IPXact.hh ProGeCmdLineOptions.hh ProGeTools.hh \
	MicroCodeGenerator.hh RV32MicroCodeGenerator.hh \
	SinglePortByteMaskSSRAMBlock.hh

## headers end
