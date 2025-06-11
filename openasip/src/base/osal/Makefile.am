noinst_LTLIBRARIES = libosal.la
libosal_la_SOURCES = OperationBehavior.cc OperationContext.cc \
	OperationState.cc OperationSerializer.cc Operation.cc \
	Operand.cc OperationModule.cc OperationPropertyLoader.cc \
	OperationIndex.cc OperationBehaviorLoader.cc \
	OperationBehaviorProxy.cc OperationPool.cc OperationDAG.cc \
	OperationDAGNode.cc OperationNode.cc TerminalNode.cc \
	OperationDAGEdge.cc OperationDAGBehavior.cc OperationDAGConverter.cc \
	OperationDAGBuilder.cc OperationGlobals.cc OperationPoolPimpl.cc \
	OperationContextPimpl.cc OperationPimpl.cc ConstantNode.cc \
    OperationBuilder.cc RISCVInstructionExecutor.cc

PROJECT_ROOT = $(top_srcdir)
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config
AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools \
              -I$(PROJECT_ROOT)/src/base/memory \
              -I../../applibs/Simulator \
              -I$(PROJECT_ROOT)/src/base/Graph ${LLVM_CPPFLAGS} \
			  -I$(PROJECT_ROOT)/src/applibs/bem \
			  -I$(PROJECT_ROOT)/src/base/bem \
			  -I$(PROJECT_ROOT)/src/base/mach 

AM_CXXFLAGS = -UNDEBUG

include_HEADERS = OSAL.hh OperationContext.hh OperationBehavior.hh \
	Operation.hh OperationState.hh Operand.hh OperationState.icc \
	OperationBehavior.icc Operand.icc Operation.icc OperationGlobals.hh \
	OperationPool.hh OperationPool.icc SimulateTriggerWrappers.icc \
    OperationBuilder.hh

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da





## headers start
libosal_la_SOURCES += \
	OperationDAG.hh OSAL.hh \
	OperationPool.hh TerminalNode.hh \
	OperationContext.hh OperationPimpl.hh \
	OperationState.hh OperationDAGLanguageParser.hh \
	Operand.hh OperationContextPimpl.hh \
	OperationPoolPimpl.hh OperationModule.hh \
	OperationDAGConverter.hh OperationBehavior.hh \
	Operation.hh OperationNode.hh \
	OperationGlobals.hh OperationBuilder.hh \
	OperationDAGBuilder.hh OperationPropertyLoader.hh \
	OperationDAGNode.hh OperationSerializer.hh \
	ConstantNode.hh OperationBehaviorProxy.hh \
	OperationIndex.hh OperationDAGBehavior.hh \
	OperationBehaviorLoader.hh OperationDAGEdge.hh \
	OperationState.icc Operand.icc \
	Operation.icc OperationModule.icc \
	OperationBehavior.icc SimulateTriggerWrappers.icc \
	OperationPool.icc OperationIndex.icc \
	RISCVInstructionExecutor.hh
## headers end
