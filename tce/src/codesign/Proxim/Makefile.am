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
proxim_LDADD = ../../libtce.la

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
	ProximStopDialog.hh ProximFUDetailsCmd.hh \
	ProximIUEditPolicy.hh ProximBusEditPolicy.hh \
	ProximRFDetailsCmd.hh AddWatchDialog.hh \
	HighlightExecPercentageCmd.hh ProximNewRegisterWindowCmd.hh \
	ProximOptionsCmd.hh ProximAboutCmd.hh \
	WatchPropertiesDialog.hh DisasmExecPercentageAttrProvider.hh \
	ProximRunCmd.hh ProximPortWindow.hh \
	ProximRuntimeErrorHandler.hh ProximExecuteFileCmd.hh \
	ProximBreakpointWindow.hh ProximEditPolicyFactory.hh \
	BreakpointPropertiesDialog.hh ProximResumeCmd.hh \
	ProximClearConsoleCmd.hh DisasmTopCountAttrProvider.hh \
	Proxim.hh ProximSimulatorSettingsCmd.hh \
	ProximNextICmd.hh HighlightTopExecCountsCmd.hh \
	ProximFUEditPolicy.hh ProximCmdHistoryWindow.hh \
	ProximMachineCanvasTool.hh ProximNewPortWindowCmd.hh \
	ProximMemoryWindow.hh ProximKillCmd.hh \
	ProximToolbox.hh ProximShowPortsCmd.hh \
	ClearDisassemblyHighlightCmd.hh ProximSocketDetailsCmd.hh \
	ProximQuitCmd.hh ProximFUPortDetailsCmd.hh \
	ProximComponentStatusCmd.hh ProximConstants.hh \
	TracedSimulatorFrontend.hh ProximRFEditPolicy.hh \
	ProximBusDetailsCmd.hh ProximSimulatorWindow.hh \
	ProximLineReader.hh ProximIUDetailsCmd.hh \
	ProximPortEditPolicy.hh ProximOpenMachineCmd.hh \
	ProximMainFrame.hh ProximStepICmd.hh \
	ProximCDCmd.hh ProximDisasmAttrProvider.hh \
	ConsoleWindow.hh AddBreakpointDialog.hh \
	ProximUnitWindow.hh ProximSimulationThread.hh \
	ProximShowRegistersCmd.hh ProximControlWindow.hh \
	ProximDisassemblyWindow.hh ProximAboutDialog.hh \
	ProximSocketEditPolicy.hh ProximDisassemblyGridTable.hh \
	SimulatorSettingsDialog.hh SimulatorEvent.hh \
	ProximMachineStateWindow.hh DisassemblyGridTable.hh \
	ProximRegisterWindow.hh ProximSetMachineWinStatusCmd.hh \
	ProximOpenProgramCmd.hh 
## headers end
