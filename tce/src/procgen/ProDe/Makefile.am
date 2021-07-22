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
		  ProDeOptions.cc MainFrame.cc\
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
		  EditParameterDialog.cc AutoSelectImplementationsDialog.cc \
		  MachineDialog.cc EditMachineCmd.cc VLIWConnectICCmd.cc \
		  BlocksConnectICCmd.cc

prode_LDFLAGS = $(TCE_LDFLAGS)

prode_LDADD = ../../libtce.la

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
	ProDeOptions.hh FitWidthCmd.hh \
	ProDeSocketEditPolicy.hh SaveOptionsCmd.hh \
	SocketPortConnCmd.hh DeleteSegmentCmd.hh \
	IUDialog.hh SetStatusTextCmd.hh \
	AddressSpaceDialog.hh ModifyIUCmd.hh \
	EditImmediateSlotsCmd.hh FUDialog.hh \
	ProDeOptionsDialog.hh SelectTool.hh \
	ToggleUnitDetailsCmd.hh MDFView.hh \
	ProDeIUPortEditPolicy.hh DeleteComponentCmd.hh \
	FUPortDialog.hh CloseDocumentCmd.hh \
	AddBridgeCmd.hh AddRFFromHDBDialog.hh \
	AddRFFromHDBCmd.hh ModifyFUCmd.hh \
	IUPortDialog.hh ProDe.hh \
	DeleteBusCmd.hh MDFDocument.hh \
	FullyConnectBussesCmd.hh SocketPortConnToolFigure.hh \
	SaveDocumentCmd.hh ProDeIUEditPolicy.hh \
	ConnectRequest.hh AboutDialog.hh \
	ModifySocketCmd.hh UndoCmd.hh \
	AddFUFromHDBDialog.hh EditTemplatesCmd.hh \
	GenerateProcessorDialog.hh DeleteFUCmd.hh \
	GCUDialog.hh \
	VerifyMachineCmd.hh ProDePortEditPolicy.hh \
	DeleteIUCmd.hh EditAddressSpacesCmd.hh \
	ModifyRFPortCmd.hh Model.hh \
	ProDeFUEditPolicy.hh PrintPreviewCmd.hh \
	NewDocumentCmd.hh SocketBusConnCmd.hh \
	AddFUFromHDBCmd.hh MainFrame.hh \
	ProDeTextGenerator.hh AboutCmd.hh \
	OpsetDialog.hh ModifyComponentCmd.hh \
	PrintCmd.hh ProDeGCUEditPolicy.hh \
	EditParameterDialog.hh \
	DeleteBridgeCmd.hh FitHeightCmd.hh \
	ZoomOutCmd.hh ModifySRPortCmd.hh \
	AddIUCmd.hh BusDialog.hh \
	ADFPrintout.hh ProDeRFPortEditPolicy.hh \
	EditBusOrderCmd.hh TemplateListDialog.hh \
	SocketBusConnToolFigure.hh OpenDocumentCmd.hh \
	ProDeBridgeEditPolicy.hh RFGuardDialog.hh \
	QuitCmd.hh AddBusCmd.hh \
	EditOptionsCmd.hh ImplementMachineCmd.hh \
	ModifyFUPortCmd.hh RFDialog.hh \
	PasteComponentCmd.hh AddIUFromHDBDialog.hh \
	ModifyIUPortCmd.hh ModifyBusCmd.hh \
	FUGuardDialog.hh CutComponentCmd.hh \
	CopyComponent.hh DeleteSocketCmd.hh \
	SRPortDialog.hh BridgeDialog.hh \
	DeleteGCUCmd.hh SelectCmd.hh \
	ModifyGCUCmd.hh SaveDocumentAsCmd.hh \
	ProDeSegmentEditPolicy.hh ProcessorImplementationWindow.hh \
	ProDeBusOrderDialog.hh DeletePortCmd.hh \
	SocketDialog.hh ProDeClipboard.hh \
	ProDeSRPortEditPolicy.hh AddFUCmd.hh \
	ProDeConstants.hh FitWindowCmd.hh \
	AddRFCmd.hh ConnectTool.hh \
	EditConnectionsCmd.hh ValidateMachineDialog.hh \
	ModelObserver.hh ModifyBridgeCmd.hh \
	AddGCUCmd.hh EditorCommand.hh \
	BlockImplementationDialog.hh ProDeExportCmd.hh \
	CallExplorerPlugin.hh TemplateSlotDialog.hh \
	RFPortDialog.hh AddSocketCmd.hh \
	AddressSpacesDialog.hh CallExplorerPluginCmd.hh \
	ProDeEditPolicyFactory.hh ProDeBusEditPolicy.hh \
	DeleteRFCmd.hh ImmediateSlotDialog.hh \
	ModelConstants.hh CopyComponentCmd.hh \
	RedoCmd.hh OperationDialog.hh \
	ProDeFUPortEditPolicy.hh ZoomInCmd.hh \
	ProDeOptionsSerializer.hh AddIUFromHDBCmd.hh \
	ModifyRFCmd.hh AddASCmd.hh \
	ProDeRFEditPolicy.hh PasteComponentCmd.icc \
	MDFView.icc AutoSelectImplementationsDialog.hh \
	MachineDialog.hh EditMachineCmd.hh VLIWConnectICCmd.hh \
	BlocksConnectICCmd.hh
## headers end
