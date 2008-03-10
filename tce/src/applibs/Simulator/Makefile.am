lib_LTLIBRARIES = libsimulator.la 
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
	CompiledSimCompiler.cc \
        CompiledSimulation.cc AssignmentQueue.cc \
	SymbolGenerator.cc ConflictDetectionCodeGenerator.cc \
	CompiledSimMove.cc CompiledSimInterpreter.cc CompiledSimSettingCommand.cc

SRC_ROOT_DIR = ../../
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
PDG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/ProgramDependenceGraph
CFG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/ControlFlowGraph
DDG_DIR = ${SRC_ROOT_DIR}/applibs/Scheduler/ProgramRepresentations/DataDependenceGraph
GRAPH_DIR = ${BASE_DIR}/Graph

TOOLS_DIR = ../../tools
UNIVERSAL_MACH_DIR = ../../base/umach
INTERPRETER_DIR = ../Interpreter

AM_CPPFLAGS = -I${TOOLS_DIR} -I${OSAL_DIR} -I${MEMORY_DIR} -I${MACH_DIR} \
	-I${UNIVERSAL_MACH_DIR} -I${PROG_DIR} -I${INTERPRETER_DIR} -I${BASE_DIR} \
	-I${TPEF_DIR} -I${DISASM_DIR} -I${TRACEDB_DIR} -I${PROG_APPLIBS_DIR} \
	-I${FSA_DIR} -I${MACH_APPLIBS_DIR} -I${OSAL_APPLIBS_DIR} -I${GRAPH_DIR} \
	-I${SCHED_APPLIBS_DIR} -I${CFG_DIR} -I${DDG_DIR} -I${PDG_DIR}

dist-hook:
	rm -rf $(distdir)/CVS $(distdir)/.deps $(distdir)/Makefile

MAINTAINERCLEANFILES = *~ *.gcov *.bbg *.bb *.da
