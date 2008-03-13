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

osed_LDFLAGS = ${GUI_LIBS} ${SQLITE_LIBDIR} ${BOOST_LIBDIR}
osed_LDADD = ${LIB_UM_DIR}/libuniversalmachine.la ${LIB_BASE_DIR}/libttabase.la \
	${LIB_TOOLS_DIR}/libtcetools.la \
	${LIB_WXTOOLS_DIR}/libwxtoolkit.la \
	${LIB_APPLIBS_OSAL_DIR}/libapplibsosal.la \
	${LIB_EPS_GENERATOR_DIR}/libepsgenerator.la \
	${LIB_APPLIBS_MACH_DIR}/libapplibsmach.la \
	${APPLIBS_FSA_DIR}/libfsa.la \
	${BOOST_LDFLAGS} ${TCL_LD_FLAGS} ${EDITLINE_LD_FLAGS}

AM_CXXFLAGS = ${GUI_INCLUDES}

AM_CPPFLAGS = -I${LIB_TOOLS_DIR} -I${LIB_OSAL_DIR} \
	-I${LIB_MEMORY_DIR} -I${LIB_WXTOOLS_DIR} \
	-I${LIB_APPLIBS_OSAL_DIR} -I${LIB_MACH_DIR} \
	-I${LIB_EPS_GENERATOR_DIR} -I${LIB_APPLIBS_MACH_DIR} \
	-I${LIB_GRAPH_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
