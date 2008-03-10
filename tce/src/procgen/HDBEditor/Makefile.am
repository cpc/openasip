CURDIR = $(shell pwd)

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

hdbeditor_LDADD = ${LIB_TCETOOLS_DIR}/libtcetools.la \
	${LIB_WXTOOLKIT_DIR}/libwxtoolkit.la \
	${LIB_BASE_DIR}/libttabase.la \
	${LIB_HDB_DIR}/libhdb.la \
	${LIB_EPSGENERATOR_DIR}/libepsgenerator.la \
	${LIB_APPLIB_MACH_DIR}/libapplibsmach.la \
	${APPLIBS_FSA_DIR}/libfsa.la


AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} \
	      -I${LIB_HDB_DIR} \
	      -I${LIB_MACH_DIR} \
	      -I${LIB_WXTOOLKIT_DIR}

AM_CXXFLAGS = -UNDEBUG ${AM_WXCXXFLAGS} 

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
