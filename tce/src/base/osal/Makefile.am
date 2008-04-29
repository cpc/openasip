noinst_LTLIBRARIES = libosal.la
libosal_la_SOURCES = OperationBehavior.cc OperationContext.cc \
	OperationState.cc OperationSerializer.cc Operation.cc \
	Operand.cc OperationModule.cc OperationPropertyLoader.cc \
	OperationIndex.cc OperationBehaviorLoader.cc \
	OperationBehaviorProxy.cc OperationPool.cc OperationDAG.cc \
	OperationDAGNode.cc OperationNode.cc TerminalNode.cc \
	OperationDAGEdge.cc OperationDAGBehavior.cc OperationDAGConverter.cc \
	OperationDAGBuilder.cc OperationGlobals.cc OperationPoolPimpl.cc \
	OperationContextPimpl.cc OperationPimpl.cc

PROJECT_ROOT = ../../..
DOXYGEN_CONFIG_FILE = ${PROJECT_ROOT}/tools/Doxygen/doxygen.config
AM_CPPFLAGS = -I${PROJECT_ROOT}/src/tools -I../memory \
              -I../../applibs/Simulator -I../../base/Graph
AM_CXXFLAGS = -UNDEBUG

include_HEADERS = OSAL.hh OperationContext.hh OperationBehavior.hh \
	Operation.hh OperationState.hh Operand.hh OperationState.icc \
	OperationBehavior.icc Operand.icc Operation.icc OperationGlobals.hh

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

.PHONY: clean doc

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da

