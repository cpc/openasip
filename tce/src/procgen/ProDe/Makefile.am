CURDIR = $(shell pwd)

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
		  GenerateProcessorDialog.cc OpsetDialog.cc

prode_LDFLAGS = $(GUI_LIBS) ${SQLITE_LIBDIR} ${BOOST_LIBDIR}

prode_LDADD =	${LIB_TCETOOLS_DIR}/libtcetools.la \
	${LIB_WXTOOLKIT_DIR}/libwxtoolkit.la \
	${LIB_BASE_DIR}/libttabase.la \
	${LIB_UM_DIR}/libuniversalmachine.la ${BOOST_LDFLAGS} \
	${LIB_MACHINE_WINDOW_DIR}/libmachinewindow.la \
	${LIB_EPS_GENERATOR_DIR}/libepsgenerator.la \
	${LIB_GUI_OPTIONS_DIR}/libguioptions.la \
	${LIB_MACH_TOOLS_DIR}/libapplibsmach.la \
	${LIB_HDB_DIR}/libhdb.la \
	${LIB_IDF_DIR}/libidf.la \
	${LIB_IDF_TOOLS_DIR}/libapplibsidf.la \
	${LIB_BEM_DIR}/libbem.la \
	${LIB_BEM_TOOLS_DIR}/libapplibsbem.la \
	${LIB_PROGE_DIR}/libproge.la \
	${LIB_TRACE_DB_DIR}/libtracedb.la \
	${LIB_ESTIMATOR_DIR}/libestimator.la \
	${LIB_OSAL_DIR}/libosal.la \
	${APPLIBS_FSA_DIR}/libfsa.la

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} \
	      -I${LIB_MACH_DIR} \
	      -I${LIB_WXTOOLKIT_DIR}  \
	      -I${LIB_MACHINE_WINDOW_DIR} \
	      -I${LIB_EPS_GENERATOR_DIR} \
	      -I${LIB_GUI_OPTIONS_DIR} \
	      -I${LIB_MACH_TOOLS_DIR} \
	      -I${LIB_PROGRAM_DIR} \
	      -I${LIB_HDB_DIR} \
	      -I${LIB_IDF_DIR} \
	      -I${LIB_IDF_TOOLS_DIR} \
	      -I${LIB_BEM_DIR} \
	      -I${LIB_BEM_TOOLS_DIR} \
	      -I${LIB_PROGE_DIR} \
	      -I${LIB_OSAL_DIR}

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
