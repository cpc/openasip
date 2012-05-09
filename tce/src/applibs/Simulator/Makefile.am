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
	KillCommand.cc MemDumpCommand.cc BusTracker.cc \
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
	CompiledSimCompiler.cc TTASimulationController.cc \
    CompiledSimulation.cc AssignmentQueue.cc \
	CompiledSimSymbolGenerator.cc ConflictDetectionCodeGenerator.cc \
	CompiledSimMove.cc CompiledSimInterpreter.cc CompiledSimSettingCommand.cc \
	CompiledSimulationPimpl.cc FSAFUResourceConflictDetectorPimpl.cc \
	OperationIDIndex.cc CompiledSimUtilizationStats.cc \
	SimpleSimulatorFrontend.cc ExecutingOperation.cc SimulatorCLI.cc \
	SimulatorCmdLineOptions.cc

# Required by compiled simulator to compile simulation engines.
include_HEADERS = \
	CompiledSimulation.hh CompiledSimulation.icc \
	SimulatorConstants.hh tce_systemc.hh MemorySystem.hh \
	MemorySystem.icc SimulatorCLI.hh

# Headers required by the SystemC wrapper.
include_HEADERS += SimpleSimulatorFrontend.hh DetailedOperationSimulator.hh \
	ExecutingOperation.hh

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
        -D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS	

AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da




## headers start
libsimulator_la_SOURCES += \
	LongImmediateRegisterState.hh SettingCommand.hh \
	CompiledSimCompiler.hh DetailedOperationSimulator.hh \
	CompiledSimSettingCommand.hh SimulatorFrontend.hh \
	CompiledSimSymbolGenerator.hh RegisterFileState.hh \
	UntilCommand.hh CompiledSimMove.hh \
	LongImmediateUnitState.hh InputPortState.hh \
	GCUState.hh MachCommand.hh \
	MemoryAccessingFUState.hh CommandsCommand.hh \
	FUConflictDetectorIndex.hh SimulatorToolbox.hh \
	RunCommand.hh ExecutingOperation.hh \
	OperationExecutor.hh SimpleSimulatorFrontend.hh \
	ExecutableInstruction.hh OutputPortState.hh \
	StepiCommand.hh MachineStateBuilder.hh \
	CompiledSimulation.hh FUState.hh \
	MemDumpCommand.hh FSAFUResourceConflictDetectorPimpl.hh \
	ReadableState.hh OpcodeSettingVirtualInputPortState.hh \
	DeleteBPCommand.hh BuslessExecutableMove.hh \
	CompiledSimulationPimpl.hh QuitCommand.hh \
	DisableBPCommand.hh StateData.hh \
	EnableBPCommand.hh Watch.hh \
	SimulationEventHandler.hh ResourceVectorFUResourceConflictDetector.hh \
	NextiCommand.hh ProcedureTransferTracker.hh \
	SimulationController.hh MemoryProxy.hh \
	StateLocator.hh DisassembleCommand.hh \
	PortState.hh UtilizationStats.hh \
	StopPointManager.hh FixedRegisters.hh \
	InlineImmediateValue.hh OperationIDIndex.hh \
	KillCommand.hh SimulatorConstants.hh \
	BusTracker.hh ExecutionTracker.hh \
	LongImmUpdateAction.hh CompiledSimInterpreter.hh \
	GuardState.hh RegisterState.hh \
	SimControlLanguageCommand.hh ClockedState.hh \
	MemorySystem.hh CompiledSimController.hh \
	CompiledSimCodeGenerator.hh TTASimulationController.hh \
	SymbolAddressCommand.hh RFAccessTracker.hh \
	ConfCommand.hh ReservationTableFUResourceConflictDetector.hh \
	FUResourceConflictDetector.hh ConflictDetectionCodeGenerator.hh \
	ProgCommand.hh SimpleOperationExecutor.hh \
	BusState.hh FSAFUResourceConflictDetector.hh \
	CompiledSimUtilizationStats.hh WatchCommand.hh \
	SimProgramBuilder.hh ResumeCommand.hh \
	HelpCommand.hh ExecutableMove.hh \
	TBPCommand.hh OneCycleOperationExecutor.hh \
	SimulationStatistics.hh MachineState.hh \
	ConditionCommand.hh tce_systemc.hh \
	SimulatorInterpreterContext.hh TriggeringInputPortState.hh \
	BPCommand.hh DCMFUResourceConflictDetector.hh \
	SimulatorInterpreter.hh StopPoint.hh \
	IgnoreCommand.hh SimulatorTextGenerator.hh \
	InstructionMemory.hh SimulationStatisticsCalculator.hh \
	AssignmentQueue.hh ConflictDetectingOperationExecutor.hh \
	InfoCommand.hh WritableState.hh \
	MultiLatencyOperationExecutor.hh POMGenMacros.hh \
	TransportPipeline.hh Breakpoint.hh \
	MemorySystem.icc BusState.icc \
	MachineState.icc FUState.icc \
	CompiledSimulation.icc AssignmentQueue.icc \
	OperationExecutor.icc ExecutableInstruction.icc \
	InstructionMemory.icc ReservationTableFUResourceConflictDetector.icc \
	GCUState.icc SettingCommand.icc \
	DCMFUResourceConflictDetector.icc 
## headers end
