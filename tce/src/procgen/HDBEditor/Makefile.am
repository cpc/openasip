CURDIR = $(shell pwd)

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
WXTOOLKIT_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit

LIB_HDB_DIR = ../../applibs/hdb
LIB_TCETOOLS_DIR = ../../tools
LIB_WXTOOLKIT_DIR = ../../applibs/wxToolkit
LIB_MACH_DIR = ../../base/mach
LIB_BASE_DIR = ../../base
LIB_EPSGENERATOR_DIR = ../../applibs/EPSGenerator
LIB_APPLIB_MACH_DIR = ../../applibs/mach
APPLIBS_FSA_DIR = ../../applibs/FSA
IMPL_TEST_DIR = ${SRC_ROOT_DIR}/applibs/ImplementationTester
MACH_SIM_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal

bin_PROGRAMS = hdbeditor testhdb

hdbeditor_SOURCES = AddFUArchFromADFCmd.cc AddFUImplementationCmd.cc \
		  AddRFArchitectureCmd.cc \
		  AddRFImplementationCmd.cc BlockImplementationFileDialog.cc \
		  CreateHDBCmd.cc FUArchitectureDialog.cc \
		  FUExternalPortDialog.cc FUImplementationDialog.cc \
		  FUImplementationParameterDialog.cc \
		  FUPortImplementationDialog.cc  HDBBrowserInfoPanel.cc \
		  HDBBrowserWindow.cc HDBEditor.cc HDBEditorConstants.cc \
		  HDBEditorDeleteCmd.cc HDBEditorMainFrame.cc \
		  HDBEditorQuitCmd.cc HDBToHtml.cc OpenHDBCmd.cc \
		  RFArchitectureDialog.cc RFImplementationDialog.cc \
		  RFPortImplementationDialog.cc HDBEditorModifyCmd.cc \
		  CostFunctionPluginDialog.cc CostEstimationDataDialog.cc \
		  AddCostFunctionPluginCmd.cc AddBusEntryCmd.cc \
		  AddSocketEntryCmd.cc AddFUEntryCmd.cc AddRFEntryCmd.cc \
		  SetCostFunctionPluginCmd.cc HDBEditorAboutCmd.cc \
		  HDBEditorAboutDialog.cc \
	AddBusEntryCmd.hh \
	AddCostFunctionPluginCmd.hh \
	AddFUArchFromADFCmd.hh \
	AddFUEntryCmd.hh \
	AddFUImplementationCmd.hh \
	AddRFArchitectureCmd.hh \
	AddRFEntryCmd.hh \
	AddRFImplementationCmd.hh \
	AddSocketEntryCmd.hh \
	BlockImplementationFileDialog.hh \
	CostEstimationDataDialog.hh \
	CostFunctionPluginDialog.hh \
	CreateHDBCmd.hh \
	FUArchitectureDialog.hh \
	FUArchitectureParametersDialog.hh \
	FUExternalPortDialog.hh \
	FUImplementationDialog.hh \
	FUImplementationParameterDialog.hh \
	FUPortImplementationDialog.hh \
	HDBBrowserInfoPanel.hh \
	HDBBrowserWindow.hh \
	HDBEditorAboutCmd.hh \
	HDBEditorAboutDialog.hh \
	HDBEditorConstants.hh \
	HDBEditorDeleteCmd.hh \
	HDBEditor.hh \
	HDBEditorMainFrame.hh \
	HDBEditorModifyCmd.hh \
	HDBEditorQuitCmd.hh \
	HDBToHtml.hh \
	OpenHDBCmd.hh \
	RFArchitectureDialog.hh \
	RFImplementationDialog.hh \
	RFPortImplementationDialog.hh \
	SetCostFunctionPluginCmd.hh \
	SetFUArchitectureCmd.hh \
	SetRFArchitectureCmd.hh \
	TestHDBCmdLineOptions.hh


hdbeditor_LDFLAGS = $(TCE_LDFLAGS)
hdbeditor_LDADD = ../../libtce.la

testhdb_SOURCES = TestHDB.cc TestHDBCmdLineOptions.cc
testhdb_LDFLAGS = $(TCE_LDFLAGS)
testhdb_LDADD = ../../libtce.la

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${TOOLS_DIR} \
	      -I${HDB_DIR} \
	      -I${MACH_DIR} \
	      -I${WXTOOLKIT_DIR} \
				-I${IMPL_TEST_DIR} \
				-I${MACH_SIM_DIR} \
				-I${OSAL_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_CXXFLAGS = -UNDEBUG ${AM_WXCXXFLAGS} 

AM_LDFLAGS = ${TCE_LDFLAGS}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


