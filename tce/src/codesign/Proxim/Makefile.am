CURDIR = $(shell pwd)

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
MACH_DIR = ${SRC_ROOT_DIR}/base/mach
WXTOOLKIT_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit
INTERPRETER_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter
SIMULATOR_DIR = ${SRC_ROOT_DIR}/applibs/Simulator
PROGRAM_DIR = ${SRC_ROOT_DIR}/base/program
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler
MEMORY_DIR = ${SRC_ROOT_DIR}/base/memory
UM_DIR = ${SRC_ROOT_DIR}/base/umach
MACHINE_WINDOW_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit/MachineWindow
GUI_OPTIONS_DIR = ${SRC_ROOT_DIR}/applibs/wxToolkit/GUIOptions
OSAL_DIR = ${SRC_ROOT_DIR}/base/osal
EPS_GENERATOR_DIR = ${SRC_ROOT_DIR}/applibs/EPSGenerator

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

proxim_LDFLAGS = $(TCE_LDFLAGS)
proxim_LDADD =	${SRC_ROOT_DIR}/libtce.la

AM_WXCXXFLAGS = $(GUI_INCLUDES)

AM_CPPFLAGS = -I${TOOLS_DIR} \
	      -I${MACH_DIR} \
	      -I${WXTOOLKIT_DIR} \
	      -I${INTERPRETER_DIR} \
	      -I${SIMULATOR_DIR} \
	      -I${PROGRAM_DIR} \
	      -I${DISASM_DIR} \
              -I${MEMORY_DIR} \
	      -I${UM_DIR} \
	      -I${MACHINE_WINDOW_DIR} \
	      -I${GUI_OPTIONS_DIR} \
	      -I${OSAL_DIR} \
	      -I${EPS_GENERATOR_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

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


## headers start
proxim_SOURCES += \
	AddBreakpointDialog.hh AddWatchDialog.hh \
	BreakpointPropertiesDialog.hh ClearDisassemblyHighlightCmd.hh \
	ConsoleWindow.hh DisasmExecPercentageAttrProvider.hh \
	DisasmTopCountAttrProvider.hh DisassemblyGridTable.hh \
	HighlightExecPercentageCmd.hh HighlightTopExecCountsCmd.hh \
	Proxim.hh ProximAboutCmd.hh \
	ProximAboutDialog.hh ProximBreakpointWindow.hh \
	ProximBusDetailsCmd.hh ProximBusEditPolicy.hh \
	ProximCDCmd.hh ProximClearConsoleCmd.hh \
	ProximCmdHistoryWindow.hh ProximComponentStatusCmd.hh \
	ProximConstants.hh ProximControlWindow.hh \
	ProximDisasmAttrProvider.hh ProximDisassemblyGridTable.hh \
	ProximDisassemblyWindow.hh ProximEditPolicyFactory.hh \
	ProximExecuteFileCmd.hh ProximFUDetailsCmd.hh \
	ProximFUEditPolicy.hh ProximFUPortDetailsCmd.hh \
	ProximIUDetailsCmd.hh ProximIUEditPolicy.hh \
	ProximKillCmd.hh ProximLineReader.hh \
	ProximMachineCanvasTool.hh ProximMachineStateWindow.hh \
	ProximMainFrame.hh ProximMemoryWindow.hh \
	ProximNewPortWindowCmd.hh ProximNewRegisterWindowCmd.hh \
	ProximNextICmd.hh ProximOpenMachineCmd.hh \
	ProximOpenProgramCmd.hh ProximOptionsCmd.hh \
	ProximPortEditPolicy.hh ProximPortWindow.hh \
	ProximQuitCmd.hh ProximRFDetailsCmd.hh \
	ProximRFEditPolicy.hh ProximRegisterWindow.hh \
	ProximResumeCmd.hh ProximRunCmd.hh \
	ProximRuntimeErrorHandler.hh ProximSetMachineWinStatusCmd.hh \
	ProximShowPortsCmd.hh ProximShowRegistersCmd.hh \
	ProximSimulationThread.hh ProximSimulatorSettingsCmd.hh \
	ProximSimulatorWindow.hh ProximSocketDetailsCmd.hh \
	ProximSocketEditPolicy.hh ProximStepICmd.hh \
	ProximStopDialog.hh ProximToolbox.hh \
	ProximUnitWindow.hh SimulatorEvent.hh \
	SimulatorSettingsDialog.hh TracedSimulatorFrontend.hh \
	WatchPropertiesDialog.hh 
## headers end
