PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
MEMORY_DIR = ${SRC_ROOT_DIR}/base/memory
WXTOOLS_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit
APPLIBS_OSAL_DIR = ${SRC_ROOT_DIR}/applibs/osal
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
EPS_GENERATOR_DIR = ${SRC_ROOT_DIR}/applibs/EPSGenerator
APPLIBS_MACH_DIR = ${SRC_ROOT_DIR}/applibs/mach
GRAPH_DIR = ${SRC_ROOT_DIR}/base/Graph

LIB_BASE_DIR = ../../../base
LIB_TOOLS_DIR = ../../../tools
LIB_OSAL_DIR = ../../../base/osal
LIB_GRAPH_DIR = ../../../base/Graph
LIB_MEMORY_DIR = ../../../base/memory
LIB_WXTOOLS_DIR = ../../../applibs/wxToolkit
LIB_APPLIBS_OSAL_DIR = ../../../applibs/osal
LIB_MACH_DIR = ../../../base/mach
LIB_UM_DIR = ../../../base/umach
LIB_EPS_GENERATOR_DIR = ../../../applibs/EPSGenerator
LIB_APPLIBS_MACH_DIR = ../../../applibs/mach
APPLIBS_FSA_DIR = ../../../applibs/FSA

bin_PROGRAMS = osed

osed_SOURCES = OSEd.cc OSEdMainFrame.cc OSEdTreeView.cc OSEdInfoView.cc \
	OperationContainer.cc OSEdConstants.cc \
	OSEdQuitCmd.cc OSEdAboutCmd.cc \
	OSEdPropertiesCmd.cc OperationPropertyDialog.cc \
	InputOperandDialog.cc OutputOperandDialog.cc \
	AddModuleDialog.cc OSEdAddModuleCmd.cc \
	OSEdTextGenerator.cc OSEdAddOperationCmd.cc \
	SimulateDialog.cc OSEdSimulateCmd.cc OSEdOptions.cc \
	OSEdOptionsSerializer.cc OSEdOptionsDialog.cc OSEdOptionsCmd.cc \
	ResultDialog.cc OSEdBuildCmd.cc \
	OSEdBuildAllCmd.cc OSEdUserManualCmd.cc OSEdRemoveModuleCmd.cc \
	OSEdModifyBehaviorCmd.cc DropDownMenu.cc CommandThread.cc \
	DialogPosition.cc OSEdRemoveOperationCmd.cc \
	SimulationInfoDialog.cc MemoryDialog.cc OSEdMemoryCmd.cc \
	OSEdInformer.cc OSEdListener.cc OSEdAboutDialog.cc \
	OperationDAGDialog.cc

osed_LDFLAGS = ${TCE_LDFLAGS}
osed_LDADD = ../../../libtce.la \
	${BOOST_LDFLAGS} ${TCL_LD_FLAGS} ${EDITLINE_LD_FLAGS}

AM_CXXFLAGS = ${GUI_INCLUDES}

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} \
	-I${MEMORY_DIR} -I${WXTOOLS_DIR} \
	-I${APPLIBS_OSAL_DIR} -I${MACH_DIR} \
	-I${EPS_GENERATOR_DIR} -I${APPLIBS_MACH_DIR} \
	-I${GRAPH_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
osed_SOURCES += \
	SimulateDialog.hh OSEdOptionsDialog.hh \
	OSEdMainFrame.hh OperationDAGDialog.hh \
	OutputOperandDialog.hh OSEdRemoveOperationCmd.hh \
	OSEdQuitCmd.hh ResultDialog.hh \
	OSEdListener.hh DialogPosition.hh \
	OSEdAddModuleCmd.hh OSEdOptions.hh \
	OSEdOptionsCmd.hh OSEdBuildCmd.hh \
	OSEdInfoView.hh OSEdAboutDialog.hh \
	OSEdRemoveModuleCmd.hh AddModuleDialog.hh \
	InputOperandDialog.hh OSEdConstants.hh \
	OSEdBuildAllCmd.hh MemoryDialog.hh \
	OSEdPropertiesCmd.hh OSEdInformer.hh \
	OSEd.hh OSEdOptionsSerializer.hh \
	DropDownMenu.hh OSEdAboutCmd.hh \
	OperationPropertyDialog.hh OSEdTextGenerator.hh \
	OperationContainer.hh OSEdUserManualCmd.hh \
	OSEdAddOperationCmd.hh OSEdMemoryCmd.hh \
	OSEdTreeView.hh CommandThread.hh \
	SimulationInfoDialog.hh OSEdModifyBehaviorCmd.hh \
	OSEdSimulateCmd.hh 
## headers end
