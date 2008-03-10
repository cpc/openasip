CURDIR = $(shell pwd)

LIB_TTABASE_DIR = ../../base
LIB_TCETOOLS_DIR = ../../tools
LIB_MACH_DIR = ../../base/mach
LIB_WXTOOLKIT_DIR = ../../applibs/wxToolkit
LIB_INTERPRETER_DIR = ../../applibs/Interpreter
LIB_BASE_DIR = ../../base
LIB_MEMORY_DIR = ../../base/memory
LIB_SIMULATOR_DIR = ../../applibs/Simulator
LIB_PROGRAM_DIR = ../../base/program
LIB_DISASM_DIR = ../../applibs/Disassembler
LIB_UM_DIR = ../../base/umach
LIB_MACHINE_WINDOW_DIR = ../../applibs/wxToolkit/MachineWindow
LIB_GUI_OPTIONS_DIR = ../../applibs/wxToolkit/GUIOptions
LIB_EPS_GENERATOR_DIR = ../../applibs/EPSGenerator
LIB_APPLIBS_PROG_DIR = ../../applibs/program
LIB_APPLIBS_SCHED_DIR = ../../applibs/Scheduler
FSA_APPLIB_DIR = ../../applibs/FSA
MACH_APPLIBS_DIR = ../../applibs/mach

TRACE_DB_DIR = ../../applibs/TraceDB
LIB_OSAL_DIR = ../../base/osal

LATEX_USER_MANUAL_DIR = ../../../doc/man/Proxim
HTML_USER_MANUAL_DIR = man

bin_PROGRAMS = proxim

proxim_SOURCES = Proxim.cc ProximMainFrame.cc ConsoleWindow.cc \
	ProximLineReader.cc ProximSimulationThread.cc \
	SimulatorEvent.cc ProximOpenMachineCmd.cc \
	ProximDisassemblyWindow.cc ProximOpenProgramCmd.cc \
	TracedSimulatorFrontend.cc ProximCDCmd.cc \
	ProximMemoryWindow.cc ProximRegisterWindow.cc ProximControlWindow.cc \
	ProximRunCmd.cc ProximStepICmd.cc ProximResumeCmd.cc \
	ProximKillCmd.cc ProximNextICmd.cc \
	ProximQuitCmd.cc ProximClearConsoleCmd.cc ProximPortWindow.cc \
	ProximAboutDialog.cc ProximAboutCmd.cc ProximRuntimeErrorHandler.cc \
	ProximMachineStateWindow.cc ProximMachineCanvasTool.cc \
	ProximBreakpointWindow.cc ProximEditPolicyFactory.cc \
	ProximRFEditPolicy.cc ProximShowRegistersCmd.cc \
	ProximFUEditPolicy.cc ProximShowPortsCmd.cc ProximToolbox.cc \
	ProximSimulatorWindow.cc ProximNewPortWindowCmd.cc \
	ProximNewRegisterWindowCmd.cc BreakpointPropertiesDialog.cc \
	ProximConstants.cc AddBreakpointDialog.cc AddWatchDialog.cc \
	WatchPropertiesDialog.cc ProximComponentStatusCmd.cc \
	ProximFUDetailsCmd.cc ProximSocketEditPolicy.cc \
	ProximPortEditPolicy.cc ProximRFDetailsCmd.cc \
	ProximBusDetailsCmd.cc ProximBusEditPolicy.cc \
	ProximSocketDetailsCmd.cc ProximSimulatorSettingsCmd.cc \
	SimulatorSettingsDialog.cc ProximOptionsCmd.cc \
	ProximCmdHistoryWindow.cc ProximStopDialog.cc \
	ProximExecuteFileCmd.cc ProximIUEditPolicy.cc ProximIUDetailsCmd.cc \
	ProximFUPortDetailsCmd.cc DisassemblyGridTable.cc \
	ProximDisassemblyGridTable.cc DisasmTopCountAttrProvider.cc \
	HighlightTopExecCountsCmd.cc ClearDisassemblyHighlightCmd.cc \
	ProximDisasmAttrProvider.cc DisasmExecPercentageAttrProvider.cc \
	HighlightExecPercentageCmd.cc ProximUnitWindow.cc

proxim_LDFLAGS = $(GUI_LIBS) $(LIB_INTERPRETER) $(TCL_LD_FLAGS) \
	${EDITLINE_LD_FLAGS} ${BOOST_LDFLAGS} ${BOOST_PYTHON_LIB}

proxim_LDADD =	${LIB_TCETOOLS_DIR}/libtcetools.la \
	${LIB_WXTOOLKIT_DIR}/libwxtoolkit.la \
	${LIB_BASE_DIR}/libttabase.la \
	${LIB_INTERPRETER_DIR}/libinterpreter.la \
	${LIB_SIMULATOR_DIR}/libsimulator.la \
	${LIB_DISASM_DIR}/libdisassembler.la \
	${LIB_UM_DIR}/libuniversalmachine.la \
	${TRACE_DB_DIR}/libtracedb.la \
	${LIB_MACHINE_WINDOW_DIR}/libmachinewindow.la \
	${LIB_GUI_OPTIONS_DIR}/libguioptions.la \
	${LIB_EPS_GENERATOR_DIR}/libepsgenerator.la \
	${LIB_APPLIBS_PROG_DIR}/libapplibsprogram.la \
	${FSA_APPLIB_DIR}/libfsa.la \
	${MACH_APPLIBS_DIR}/libapplibsmach.la \
	${LIB_APPLIBS_SCHED_DIR}/libscheduler.la

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${LIB_TCETOOLS_DIR} \
	      -I${LIB_MACH_DIR} \
	      -I${LIB_WXTOOLKIT_DIR} \
	      -I${LIB_INTERPRETER_DIR} \
	      -I${LIB_SIMULATOR_DIR} \
	      -I${LIB_PROGRAM_DIR} \
	      -I${LIB_DISASM_DIR} \
              -I${LIB_MEMORY_DIR} \
	      -I${LIB_UM_DIR} \
	      -I${LIB_MACHINE_WINDOW_DIR} \
	      -I${LIB_GUI_OPTIONS_DIR} \
	      -I${LIB_OSAL_DIR} \
	      -I${LIB_EPS_GENERATOR_DIR}

AM_CXXFLAGS = -UNDEBUG ${AM_WXCXXFLAGS} 

${LIB_TCETOOLS_DIR}/libtcetools.la:
	@(cd ${LIB_TCETOOLS_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_MACH_DIR}/libbase.la:
	@(cd ${LIB_BASE_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_WXTOOLKIT_DIR}/libwxtoolkit.la:
	@(cd ${LIB_WXTOOLKIT_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_INTERPRETER_DIR}/libinterpreter.la:
	@(cd ${LIB_INTERPRETER_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${LIB_EPS_GENERATOR_DIR}/libepsgenerator.la:
	@(cd ${LIB_EPS_GENERATOR_DIR}; ${MAKE} || exit 1; cd ${CURDIR})
${UM_DIR}/libdisassembler.la:
	@(cd ${UM_DIR}; ${MAKE} || exit 1; cd ${CURDIR})

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
