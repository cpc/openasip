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

bin_PROGRAMS = hdbeditor

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
		  HDBEditorAboutDialog.cc

hdbeditor_LDFLAGS = $(GUI_LIBS) $(LIB_INTERPRETER) $(TCL_LD_FLAGS) \
	${EDITLINE_LD_FLAGS} ${BOOST_LDFLAGS}

hdbeditor_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${TOOLS_DIR} \
	      -I${HDB_DIR} \
	      -I${MACH_DIR} \
	      -I${WXTOOLKIT_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_CXXFLAGS = -UNDEBUG ${AM_WXCXXFLAGS} 

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
