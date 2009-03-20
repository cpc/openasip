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
osed_LDADD = ${SRC_ROOT_DIR}/libtce.la \
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
	AddModuleDialog.hh CommandThread.hh \
	DialogPosition.hh DropDownMenu.hh \
	InputOperandDialog.hh MemoryDialog.hh \
	OSEd.hh OSEdAboutCmd.hh \
	OSEdAboutDialog.hh OSEdAddModuleCmd.hh \
	OSEdAddOperationCmd.hh OSEdBuildAllCmd.hh \
	OSEdBuildCmd.hh OSEdConstants.hh \
	OSEdInfoView.hh OSEdInformer.hh \
	OSEdListener.hh OSEdMainFrame.hh \
	OSEdMemoryCmd.hh OSEdModifyBehaviorCmd.hh \
	OSEdOptions.hh OSEdOptionsCmd.hh \
	OSEdOptionsDialog.hh OSEdOptionsSerializer.hh \
	OSEdPropertiesCmd.hh OSEdQuitCmd.hh \
	OSEdRemoveModuleCmd.hh OSEdRemoveOperationCmd.hh \
	OSEdSimulateCmd.hh OSEdTextGenerator.hh \
	OSEdTreeView.hh OSEdUserManualCmd.hh \
	OperationContainer.hh OperationDAGDialog.hh \
	OperationPropertyDialog.hh OutputOperandDialog.hh \
	ResultDialog.hh SimulateDialog.hh \
	SimulationInfoDialog.hh 
## headers end
