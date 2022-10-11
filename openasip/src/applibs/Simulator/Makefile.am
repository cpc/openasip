noinst_LTLIBRARIES = libsimulator.la 
libsimulator_la_SOURCES = WritableState.cc ReadableState.cc StateData.cc \
	ClockedState.cc BusState.cc FUState.cc FixedRegisters.cc \
	GCUState.cc InputPortState.cc LongImmediateRegisterState.cc \
	LongImmediateUnitState.cc MachineState.cc MachineStateBuilder.cc \
	MemoryAccessingFUState.cc OneCycleOperationExecutor.cc \
	OpcodeSettingVirtualInputPortState.cc OperationExecutor.cc \
	OutputPortState.cc PortState.cc RegisterFileState.cc \
	RegisterState.cc SimpleOperationExecutor.cc \
	TriggeringInputPortState.cc \
	SimulatorToolbox.cc MemorySystem.cc \
	StateLocator.cc TransportPipeline.cc SimulationController.cc \
	ExecutableMove.cc ExecutableInstruction.cc \
	InstructionMemory.cc LongImmUpdateAction.cc SimProgramBuilder.cc \
	SimulatorInterpreterContext.cc SimulatorFrontend.cc \
	SimulatorInterpreter.cc ProgCommand.cc SimulatorTextGenerator.cc \
	MachCommand.cc ConfCommand.cc QuitCommand.cc HelpCommand.cc \
	Breakpoint.cc RunCommand.cc StepiCommand.cc \
	DisassembleCommand.cc UntilCommand.cc SimControlLanguageCommand.cc \
	SimulationEventHandler.cc ExecutionTracker.cc SettingCommand.cc \
	ResumeCommand.cc InfoCommand.cc BPCommand.cc TBPCommand.cc \
	ConditionCommand.cc IgnoreCommand.cc DeleteBPCommand.cc \
	EnableBPCommand.cc DisableBPCommand.cc NextiCommand.cc \
	KillCommand.cc MemDumpCommand.cc MemWriteCommand.cc BusTracker.cc \
	BuslessExecutableMove.cc \
	SimulationStatisticsCalculator.cc SimulationStatistics.cc \
	UtilizationStats.cc StopPoint.cc StopPointManager.cc Watch.cc \
	WatchCommand.cc RFAccessTracker.cc CommandsCommand.cc \
	ProcedureTransferTracker.cc GuardState.cc FUResourceConflictDetector.cc \
	FSAFUResourceConflictDetector.cc \
	ResourceVectorFUResourceConflictDetector.cc \
	DCMFUResourceConflictDetector.cc \
	ReservationTableFUResourceConflictDetector.cc \
	ConflictDetectingOperationExecutor.cc MemoryProxy.cc \
	MultiLatencyOperationExecutor.cc SymbolAddressCommand.cc \
	CompiledSimCodeGenerator.cc CompiledSimController.cc \
	CompiledSimCompiler.cc TTASimulationController.cc OTASimulationController.cc \
    CompiledSimulation.cc AssignmentQueue.cc \
	CompiledSimSymbolGenerator.cc ConflictDetectionCodeGenerator.cc \
	CompiledSimMove.cc CompiledSimInterpreter.cc CompiledSimSettingCommand.cc \
	CompiledSimulationPimpl.cc FSAFUResourceConflictDetectorPimpl.cc \
	OperationIDIndex.cc CompiledSimUtilizationStats.cc \
	SimpleSimulatorFrontend.cc ExecutingOperation.cc \
	CallPathTracker.cc BackTraceCommand.cc SimulatorCLI.cc \
	SimulatorCmdLineOptions.cc \
	RemoteController.cc CustomDBGController.cc TCEDBGController.cc \
	TTASimulatorCLI.cc

# Required by compiled simulator to compile simulation engines.
include_HEADERS = \
	CompiledSimulation.hh CompiledSimulation.icc \
	SimulatorConstants.hh tce_systemc.hh MemorySystem.hh \
	MemorySystem.icc SimulatorCLI.hh

# Headers required by the SystemC wrapper.
include_HEADERS += SimpleSimulatorFrontend.hh DetailedOperationSimulator.hh \
	ExecutingOperation.hh SimulationEventHandler.hh

PROJECT_ROOT = $(top_srcdir)
SRC_ROOT_DIR = ${PROJECT_ROOT}/src
BASE_DIR = ${SRC_ROOT_DIR}/base

TPEF_DIR = ${BASE_DIR}/tpef
OSAL_DIR = ${BASE_DIR}/osal
MEMORY_DIR = ${BASE_DIR}/memory
MACH_DIR = ${BASE_DIR}/mach
PROG_DIR = ${BASE_DIR}/program
DISASM_DIR = ${SRC_ROOT_DIR}/applibs/Disassembler
TRACEDB_DIR = ${SRC_ROOT_DIR}/applibs/TraceDB
PROG_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/program
FSA_DIR = ${SRC_ROOT_DIR}/applibs/FSA
MACH_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/mach
OSAL_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/osal
PIG_DIR = ${SRC_ROOT_DIR}/applibs/PIG
BEMG_DIR = ${SRC_ROOT_DIR}/applibs/bem
BEM_DIR = ${SRC_ROOT_DIR}/base/bem

SCHED_APPLIBS_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler
RESOURCE_MODEL_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ResourceModel
PDG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/PDG
CFG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/CFG
DDG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/DDG
GRAPH_DIR = ${BASE_DIR}/Graph

TOOLS_DIR = ${SRC_ROOT_DIR}/tools
UNIVERSAL_MACH_DIR = ${BASE_DIR}/umach
INTERPRETER_DIR = ${SRC_ROOT_DIR}/applibs/Interpreter

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} -I${MEMORY_DIR} -I${MACH_DIR} \
	-I${UNIVERSAL_MACH_DIR} -I${PROG_DIR} -I${INTERPRETER_DIR} -I${BASE_DIR} \
	-I${TPEF_DIR} -I${DISASM_DIR} -I${TRACEDB_DIR} -I${PROG_APPLIBS_DIR} \
	-I${FSA_DIR} -I${MACH_APPLIBS_DIR} -I${OSAL_APPLIBS_DIR} -I${GRAPH_DIR} \
	-I${SCHED_APPLIBS_DIR} -I${CFG_DIR} -I${DDG_DIR} -I${PDG_DIR} \
	-I${PIG_DIR}  -I${BEMG_DIR} -I${BEM_DIR} \
        -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS	

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libsimulator_la_SOURCES += \
	Breakpoint.hh InlineImmediateValue.hh \
	ResumeCommand.hh SimulationStatistics.hh \
	BusTracker.hh SimulatorInterpreterContext.hh \
	SimControlLanguageCommand.hh GuardState.hh \
	UtilizationStats.hh IgnoreCommand.hh \
	ConditionCommand.hh OpcodeSettingVirtualInputPortState.hh \
	HelpCommand.hh CompiledSimUtilizationStats.hh \
	QuitCommand.hh SettingCommand.hh \
	CompiledSimCodeGenerator.hh CompiledSimInterpreter.hh \
	CompiledSimCompiler.hh ConflictDetectionCodeGenerator.hh \
	TTASimulationController.hh CompiledSimSymbolGenerator.hh \
	InputPortState.hh ExecutableInstruction.hh \
	SimProgramBuilder.hh SimulatorConstants.hh \
	WatchCommand.hh WritableState.hh \
	DeleteBPCommand.hh SymbolAddressCommand.hh \
	SimulatorFrontend.hh ClockedState.hh \
	MultiLatencyOperationExecutor.hh SimulatorToolbox.hh \
	KillCommand.hh RegisterState.hh \
	Watch.hh MachCommand.hh \
	ProgCommand.hh SimulatorCmdLineOptions.hh \
	FixedRegisters.hh MachineState.hh \
	BusState.hh POMGenMacros.hh \
	ReservationTableFUResourceConflictDetector.hh InfoCommand.hh \
	ExecutingOperation.hh FSAFUResourceConflictDetector.hh \
	OperationIDIndex.hh ExecutionTracker.hh \
	NextiCommand.hh FUResourceConflictDetector.hh \
	OneCycleOperationExecutor.hh SimulatorCLI.hh \
	LongImmediateRegisterState.hh AssignmentQueue.hh \
	StepiCommand.hh RegisterFileState.hh \
	MemorySystem.hh FSAFUResourceConflictDetectorPimpl.hh \
	MemDumpCommand.hh MemWriteCommand.hh CompiledSimulationPimpl.hh \
	GCUState.hh EnableBPCommand.hh \
	DisassembleCommand.hh SimulatorTextGenerator.hh \
	SimulatorInterpreter.hh StopPointManager.hh \
	FUState.hh CompiledSimulation.hh \
	CompiledSimSettingCommand.hh ExecutableMove.hh \
	LongImmUpdateAction.hh BuslessExecutableMove.hh \
	StateLocator.hh CompiledSimController.hh \
	CommandsCommand.hh ProcedureTransferTracker.hh \
	SimulationController.hh ReadableState.hh \
	MemoryProxy.hh DisableBPCommand.hh \
	SimpleSimulatorFrontend.hh ConfCommand.hh \
	TriggeringInputPortState.hh ConflictDetectingOperationExecutor.hh \
	LongImmediateUnitState.hh RunCommand.hh \
	ResourceVectorFUResourceConflictDetector.hh PortState.hh \
	MachineStateBuilder.hh OperationExecutor.hh \
	RFAccessTracker.hh TransportPipeline.hh \
	OutputPortState.hh BPCommand.hh \
	FUConflictDetectorIndex.hh SimpleOperationExecutor.hh \
	InstructionMemory.hh DCMFUResourceConflictDetector.hh \
	TBPCommand.hh MemoryAccessingFUState.hh \
	StopPoint.hh UntilCommand.hh \
	StateData.hh CompiledSimMove.hh \
	SimulationEventHandler.hh tce_systemc.hh \
	DetailedOperationSimulator.hh SimulationStatisticsCalculator.hh \
	ReservationTableFUResourceConflictDetector.icc FUState.icc \
	MemorySystem.icc BusState.icc \
	OperationExecutor.icc InstructionMemory.icc \
	SettingCommand.icc CompiledSimulation.icc \
	GCUState.icc DCMFUResourceConflictDetector.icc \
	ExecutableInstruction.icc MachineState.icc \
	AssignmentQueue.icc TTASimulatorCLI.hh
## headers end
