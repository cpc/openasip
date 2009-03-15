CURDIR = $(shell pwd)

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
WXTOOLKIT_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
HDB_DIR = ${SRC_ROOT_DIR}/applibs/hdb
MACHINE_WINDOW_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit/MachineWindow
GUI_OPTIONS_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit/GUIOptions
EPS_GENERATOR_DIR = ${SRC_ROOT_DIR}/applibs/EPSGenerator
MACH_TOOLS_DIR = ${SRC_ROOT_DIR}/applibs/mach
IDF_DIR = ${SRC_ROOT_DIR}/base/idf
IDF_TOOLS_DIR = ${SRC_ROOT_DIR}/applibs/idf
BEM_DIR = ${SRC_ROOT_DIR}/base/bem
PROGE_DIR = ${SRC_ROOT_DIR}/applibs/ProGe
BEM_TOOLS_DIR = ${SRC_ROOT_DIR}/applibs/bem
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
EXPLORER_DIR = ${SRC_ROOT_DIR}/applibs/Explorer
SIMULATOR_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
ESTIMATOR_DIR = ${SRC_ROOT_DIR}/applibs/Estimator
DSDB_DIR = ${SRC_ROOT_DIR}/applibs/dsdb
INTERPRETER_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter

LIB_TTABASE_DIR = ../../base
LIB_TCETOOLS_DIR = ../../tools
LIB_MACH_DIR = ../../base/mach
LIB_WXTOOLKIT_DIR = ../../applibs/wxToolkit
LIB_BASE_DIR = ../../base
LIB_UM_DIR = ../../base/umach
LIB_PROGRAM_DIR = ../../base/program
LIB_HDB_DIR = ../../applibs/hdb
LIB_MACHINE_WINDOW_DIR = ../../applibs/wxToolkit/MachineWindow
LIB_GUI_OPTIONS_DIR = ../../applibs/wxToolkit/GUIOptions
LIB_EPS_GENERATOR_DIR = ../../applibs/EPSGenerator
LIB_MACH_TOOLS_DIR = ../../applibs/mach
LIB_IDF_DIR = ../../base/idf
LIB_IDF_TOOLS_DIR = ../../applibs/idf
LIB_BEM_DIR = ../../base/bem
LIB_PROGE_DIR = ../../applibs/ProGe
LIB_BEM_TOOLS_DIR = ../../applibs/bem
LIB_TRACE_DB_DIR = ../../applibs/TraceDB
LIB_ESTIMATOR_DIR = ../../applibs/Estimator
LIB_OSAL_DIR = ../../base/osal
LIB_APPLIBS_DIR = ../../applibs/
APPLIBS_FSA_DIR = ../../applibs/FSA

bin_PROGRAMS = prode

prode_SOURCES = Model.cc\
		  ProDe.cc ProDeOptionsSerializer.cc\
		  ProDeOptions.cc ChildFrame.cc MainFrame.cc\
		  MDFDocument.cc MDFView.cc\
		  FUDialog.cc AddFUCmd.cc \
		  AddRFCmd.cc AddBusCmd.cc AddSocketCmd.cc\
		  AddBridgeCmd.cc AddIUCmd.cc AddGCUCmd.cc\
		  AddASCmd.cc DeleteComponentCmd.cc\
		  ModifyComponentCmd.cc FUPortDialog.cc \
		  ModelConstants.cc\
		  SelectTool.cc \
		  ProDePortEditPolicy.cc ProDeBusEditPolicy.cc\
	          ProDeSegmentEditPolicy.cc\
		  ProDeSocketEditPolicy.cc ProDeRFEditPolicy.cc\
		  ProDeIUEditPolicy.cc\
		  ProDeGCUEditPolicy.cc ProDeBridgeEditPolicy.cc\
		  ModifyFUCmd.cc ModifyRFCmd.cc ModifyIUCmd.cc\
		  ModifyGCUCmd.cc ModifyBusCmd.cc ModifySocketCmd.cc\
		  ModifyBridgeCmd.cc\
		  DeleteFUCmd.cc DeleteRFCmd.cc DeleteIUCmd.cc\
		  DeleteGCUCmd.cc DeleteBusCmd.cc DeleteSocketCmd.cc\
		  DeleteBridgeCmd.cc\
		  ProDeOptionsDialog.cc EditOptionsCmd.cc\
		  EditorCommand.cc ProDeConstants.cc\
		  SaveOptionsCmd.cc\
		  EditConnectionsCmd.cc EditAddressSpacesCmd.cc\
		  VerifyMachineCmd.cc AboutCmd.cc\
		  OpenDocumentCmd.cc NewDocumentCmd.cc SaveDocumentCmd.cc\
		  RFDialog.cc RFPortDialog.cc BusDialog.cc BridgeDialog.cc\
		  IUDialog.cc IUPortDialog.cc SocketDialog.cc\
		  AboutDialog.cc ZoomInCmd.cc\
		  ZoomOutCmd.cc FitWindowCmd.cc\
		  FitHeightCmd.cc FitWidthCmd.cc SaveDocumentAsCmd.cc\
		  CloseDocumentCmd.cc QuitCmd.cc SelectCmd.cc GCUDialog.cc\
		  AddressSpacesDialog.cc AddressSpaceDialog.cc \
		  PrintCmd.cc PrintPreviewCmd.cc \
		  ProDeTextGenerator.cc UndoCmd.cc RedoCmd.cc \
		  ProDeClipboard.cc CopyComponentCmd.cc \
		  PasteComponentCmd.cc\
		  CopyComponent.cc CutComponentCmd.cc RFGuardDialog.cc \
		  FUGuardDialog.cc TemplateListDialog.cc \
		  TemplateSlotDialog.cc EditTemplatesCmd.cc \
		  OperationDialog.cc SetStatusTextCmd.cc\
		  ProDeFUPortEditPolicy.cc ProDeFUEditPolicy.cc\
		  ConnectTool.cc ConnectRequest.cc \
		  SocketPortConnToolFigure.cc SocketBusConnToolFigure.cc \
		  DeletePortCmd.cc SocketPortConnCmd.cc SocketBusConnCmd.cc \
		  ModifyFUPortCmd.cc ModifyIUPortCmd.cc ModifyRFPortCmd.cc \
		  ProDeRFPortEditPolicy.cc ProDeIUPortEditPolicy.cc\
		  DeleteSegmentCmd.cc SRPortDialog.cc \
		  ProDeEditPolicyFactory.cc ImmediateSlotDialog.cc \
		  EditImmediateSlotsCmd.cc ADFPrintout.cc ProDeExportCmd.cc \
		  ProDeBusOrderDialog.cc EditBusOrderCmd.cc \
		  FullyConnectBussesCmd.cc ProDeSRPortEditPolicy.cc \
		  ModifySRPortCmd.cc AddRFFromHDBCmd.cc AddRFFromHDBDialog.cc \
		  AddIUFromHDBCmd.cc AddIUFromHDBDialog.cc AddFUFromHDBCmd.cc \
		  AddFUFromHDBDialog.cc ValidateMachineDialog.cc \
		  ToggleUnitDetailsCmd.cc ProcessorImplementationWindow.cc \
		  BlockImplementationDialog.cc ImplementMachineCmd.cc \
		  GenerateProcessorDialog.cc OpsetDialog.cc \
		  CallExplorerPlugin.cc CallExplorerPluginCmd.cc \
		  EditParameterDialog.cc

prode_LDFLAGS = $(GUI_LIBS) ${SQLITE_LIBDIR} ${BOOST_LIBDIR}

prode_LDADD = ${SRC_ROOT_DIR}/libtce.la

AM_WXCXXFLAGS = $(GUI_INCLUDES)


AM_CPPFLAGS = -I${TOOLS_DIR} \
	      -I${MACH_DIR} \
	      -I${WXTOOLKIT_DIR}  \
	      -I${MACHINE_WINDOW_DIR} \
	      -I${EPS_GENERATOR_DIR} \
	      -I${GUI_OPTIONS_DIR} \
	      -I${MACH_TOOLS_DIR} \
	      -I${PROGRAM_DIR} \
	      -I${HDB_DIR} \
	      -I${IDF_DIR} \
	      -I${IDF_TOOLS_DIR} \
	      -I${BEM_DIR} \
	      -I${BEM_TOOLS_DIR} \
	      -I${PROGE_DIR} \
	      -I${OSAL_DIR} \
	      -I${EXPLORER_DIR} \
	      -I${SIMULATOR_DIR} \
	      -I${ESTIMATOR_DIR} \
	      -I${DSDB_DIR} \
	      -I${INTERPRETER_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

AM_CXXFLAGS = -UNDEBUG ${AM_WXCXXFLAGS} 

${LIB_TCETOOLS_DIR}/libtcetools.la:
	@(cd ${LIB_TCETOOLS_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_MACH_DIR}/libmach.la:
	@(cd ${LIB_MACH_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_WXTOOLKIT_DIR}/libwxtoolkit.la:
	@(cd ${LIB_WXTOOLKIT_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_MACHINE_WINDOW_DIR}/libmachinewindow.la:
	@(cd ${LIB_MACHINE_WINDOW_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_GUI_OPTIONS_DIR}/libguioptions.la:
	@(cd ${LIB_GUI_OPTIONS_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_EPS_GENERATOR_DIR}/libepsgenerator.la:
	@(cd ${LIB_EPS_GENERATOR_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_HDB_DIR}/libhdb.la:
	@(cd ${LIB_HDB_DIR}; ${MAKE} || exit 1; cd ${CURDIR})

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
prode_SOURCES += \
	ADFPrintout.hh AboutCmd.hh \
	AboutDialog.hh AddASCmd.hh \
	AddBridgeCmd.hh AddBusCmd.hh \
	AddFUCmd.hh AddFUFromHDBCmd.hh \
	AddFUFromHDBDialog.hh AddGCUCmd.hh \
	AddIUCmd.hh AddIUFromHDBCmd.hh \
	AddIUFromHDBDialog.hh AddRFCmd.hh \
	AddRFFromHDBCmd.hh AddRFFromHDBDialog.hh \
	AddSocketCmd.hh AddressSpaceDialog.hh \
	AddressSpacesDialog.hh BlockImplementationDialog.hh \
	BridgeDialog.hh BusDialog.hh \
	ChildFrame.hh CloseDocumentCmd.hh \
	ConnectRequest.hh ConnectTool.hh \
	CopyComponent.hh CopyComponentCmd.hh \
	CutComponentCmd.hh DeleteBridgeCmd.hh \
	DeleteBusCmd.hh DeleteComponentCmd.hh \
	DeleteFUCmd.hh DeleteGCUCmd.hh \
	DeleteIUCmd.hh DeletePortCmd.hh \
	DeleteRFCmd.hh DeleteSegmentCmd.hh \
	DeleteSocketCmd.hh EditAddressSpacesCmd.hh \
	EditBusOrderCmd.hh EditConnectionsCmd.hh \
	EditImmediateSlotsCmd.hh EditOptionsCmd.hh \
	EditTemplatesCmd.hh EditorCommand.hh \
	FUDialog.hh FUGuardDialog.hh \
	FUPortDialog.hh FitHeightCmd.hh \
	FitWidthCmd.hh FitWindowCmd.hh \
	FullyConnectBussesCmd.hh GCUDialog.hh \
	GenerateProcessorDialog.hh IUDialog.hh \
	IUPortDialog.hh ImmediateSlotDialog.hh \
	ImplementMachineCmd.hh MDFDocument.hh \
	MDFLayoutConstraints.hh MDFView.hh \
	MainFrame.hh Model.hh \
	ModelConstants.hh ModelObserver.hh \
	ModifyBridgeCmd.hh ModifyBusCmd.hh \
	ModifyComponentCmd.hh ModifyFUCmd.hh \
	ModifyFUPortCmd.hh ModifyGCUCmd.hh \
	ModifyIUCmd.hh ModifyIUPortCmd.hh \
	ModifyRFCmd.hh ModifyRFPortCmd.hh \
	ModifySRPortCmd.hh ModifySocketCmd.hh \
	NewDocumentCmd.hh OpenDocumentCmd.hh \
	OperationDialog.hh OpsetDialog.hh \
	PasteComponentCmd.hh PrintCmd.hh \
	PrintPreviewCmd.hh ProDe.hh \
	ProDeBridgeEditPolicy.hh ProDeBusEditPolicy.hh \
	ProDeBusOrderDialog.hh ProDeClipboard.hh \
	ProDeEditPolicyFactory.hh ProDeExportCmd.hh \
	ProDeFUEditPolicy.hh ProDeFUPortEditPolicy.hh \
	ProDeGCUEditPolicy.hh ProDeIUEditPolicy.hh \
	ProDeIUPortEditPolicy.hh ProDeOptions.hh \
	ProDeOptionsDialog.hh ProDeOptionsSerializer.hh \
	ProDePortEditPolicy.hh ProDeRFEditPolicy.hh \
	ProDeRFPortEditPolicy.hh ProDeSRPortEditPolicy.hh \
	ProDeSegmentEditPolicy.hh ProDeSocketEditPolicy.hh \
	CallExplorerPlugin.hh ProcessorImplementationWindow.hh \
	QuitCmd.hh RFDialog.hh \
	RFGuardDialog.hh RFPortDialog.hh \
	RedoCmd.hh SRPortDialog.hh \
	SaveDocumentAsCmd.hh SaveDocumentCmd.hh \
	SaveOptionsCmd.hh SelectCmd.hh \
	SelectTool.hh SetStatusTextCmd.hh \
	SocketBusConnCmd.hh SocketBusConnToolFigure.hh \
	SocketDialog.hh SocketPortConnCmd.hh \
	SocketPortConnToolFigure.hh TemplateListDialog.hh \
	TemplateSlotDialog.hh ToggleUnitDetailsCmd.hh \
	UndoCmd.hh ValidateMachineDialog.hh \
	VerifyMachineCmd.hh ZoomInCmd.hh \
	ZoomOutCmd.hh CallExplorerPluginCmd.hh \
	EditParameterDialog.hh ProDeConstants.hh \
	ProDeTextGenerator.hh MDFView.icc \
	PasteComponentCmd.icc 
## headers end
