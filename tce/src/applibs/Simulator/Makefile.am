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
	UnboundRegisterFileState.cc SimulatorToolbox.cc MemorySystem.cc \
	StateLocator.cc TransportPipeline.cc SimulationController.cc \
	GlobalLock.cc ExecutableMove.cc ExecutableInstruction.cc \
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
	OperationIDIndex.cc CompiledSimUtilizationStats.cc

# Required by compiled simulator to compile simulation engines.
include_HEADERS = \
	CompiledSimulation.hh CompiledSimulation.icc \
	SimulatorConstants.hh 

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
	-I${SCHED_APPLIBS_DIR} -I${CFG_DIR} -I${DDG_DIR} -I${PDG_DIR}
AM_CPPFLAGS += -I${PROJECT_ROOT} # Needed for config.h

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da


## headers start
libsimulator_la_SOURCES += \
	AssignmentQueue.hh BPCommand.hh \
	Breakpoint.hh BusState.hh \
	BusTracker.hh BuslessExecutableMove.hh \
	ClockedState.hh CommandsCommand.hh \
	CompiledSimCodeGenerator.hh CompiledSimCompiler.hh \
	CompiledSimController.hh CompiledSimInterpreter.hh \
	CompiledSimMove.hh CompiledSimSettingCommand.hh \
	CompiledSimSymbolGenerator.hh CompiledSimUtilizationStats.hh \
	CompiledSimulation.hh CompiledSimulationPimpl.hh \
	ConditionCommand.hh ConfCommand.hh \
	ConflictDetectingOperationExecutor.hh ConflictDetectionCodeGenerator.hh \
	DCMFUResourceConflictDetector.hh DeleteBPCommand.hh \
	DisableBPCommand.hh DisassembleCommand.hh \
	EnableBPCommand.hh ExecutableInstruction.hh \
	Watch.hh ExecutableMove.hh \
	ExecutionTracker.hh FSAFUResourceConflictDetector.hh \
	FSAFUResourceConflictDetectorPimpl.hh FUConflictDetectorIndex.hh \
	FUResourceConflictDetector.hh FUState.hh \
	FixedRegisters.hh GCUState.hh \
	GlobalLock.hh GuardState.hh \
	HelpCommand.hh IgnoreCommand.hh \
	InfoCommand.hh InlineImmediateValue.hh \
	InputPortState.hh InstructionMemory.hh \
	KillCommand.hh LongImmUpdateAction.hh \
	LongImmediateRegisterState.hh LongImmediateUnitState.hh \
	MachCommand.hh MachineState.hh \
	MachineStateBuilder.hh MemDumpCommand.hh \
	MemoryAccessingFUState.hh MemoryProxy.hh \
	MemorySystem.hh MultiLatencyOperationExecutor.hh \
	NextiCommand.hh OneCycleOperationExecutor.hh \
	OpcodeSettingVirtualInputPortState.hh OperationExecutor.hh \
	OperationIDIndex.hh OutputPortState.hh \
	POMGenMacros.hh PortState.hh \
	ProcedureTransferTracker.hh ProgCommand.hh \
	QuitCommand.hh RFAccessTracker.hh \
	ReadableState.hh RegisterFileState.hh \
	RegisterState.hh ReservationTableFUResourceConflictDetector.hh \
	ResourceVectorFUResourceConflictDetector.hh ResumeCommand.hh \
	RunCommand.hh SettingCommand.hh \
	SimControlLanguageCommand.hh SimProgramBuilder.hh \
	SimpleOperationExecutor.hh SimulationController.hh \
	SimulationEventHandler.hh SimulationStatistics.hh \
	SimulationStatisticsCalculator.hh SimulatorConstants.hh \
	SimulatorFrontend.hh SimulatorInterpreter.hh \
	SimulatorInterpreterContext.hh SimulatorTextGenerator.hh \
	SimulatorToolbox.hh StateData.hh \
	StateLocator.hh StepiCommand.hh \
	StopPoint.hh StopPointManager.hh \
	SymbolAddressCommand.hh TBPCommand.hh \
	TTASimulationController.hh TransportPipeline.hh \
	TriggeringInputPortState.hh UnboundRegisterFileState.hh \
	UntilCommand.hh UtilizationStats.hh \
	WatchCommand.hh WritableState.hh \
	AssignmentQueue.icc BusState.icc \
	CompiledSimulation.icc DCMFUResourceConflictDetector.icc \
	ExecutableInstruction.icc FUState.icc \
	GCUState.icc InstructionMemory.icc \
	MachineState.icc MemorySystem.icc \
	OperationExecutor.icc ReservationTableFUResourceConflictDetector.icc \
	SettingCommand.icc 
## headers end
