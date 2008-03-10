/**
 * @file SimulatorFrontend.hh
 *
 * Declaration of SimulatorFrontend class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_FRONTEND
#define TTA_SIMULATOR_FRONTEND

#include <fstream>
#include <string>
#include <iostream>
#include <ctime>

#include <boost/timer.hpp>
#include <set>

#include "Exception.hh"
#include "Machine.hh"
#include "Program.hh"
#include "SimulationController.hh"
#include "SimulatorConstants.hh"
#include "BaseType.hh"

class SimValue;
class StateData;
class MachineState;
class POMDisassembler;
class ExecutionTracker;
class ExecutionTrace;
class StopPointManager;
class MemorySystem;
class UtilizationStats;
class RFAccessTracker;
class BusTracker;
class ExecutableInstruction;
class ProcedureTransferTracker;
class SimulationEventHandler;
namespace TPEF {
    class Binary;
}

/**
 * Frontend to simulator functionality.
 *
 * User interfaces use this class as the access point to simulation
 * functionality. It acts as a "facade" hiding the complexity of simulator
 * engine. Error messages of exceptions thrown by the methods of this
 * class are generated using SimulatorTextGenerator, thus they can be used 
 * in user interfaces directly.
 *
 */
class SimulatorFrontend {
public:
    SimulatorFrontend(bool useCompiledSimulation = false);
    virtual ~SimulatorFrontend();

    virtual void loadProgram(const std::string& fileName)
        throw (Exception);

    virtual void loadMachine(const std::string& fileName)
        throw (FileNotFound, IOException, SimulationStillRunning, 
               IllegalMachine);

    void loadProgram(const TTAProgram::Program& program)
        throw (Exception);

    void loadMachine(const TTAMachine::Machine& machine)
        throw (SimulationStillRunning, IllegalProgram);

    void loadProcessorConfiguration(const std::string& fileName)
        throw (FileNotFound, IOException, SimulationStillRunning, 
               IllegalMachine);

    const TTAMachine::Machine& machine() const;
    const TTAProgram::Program& program() const;

    const SimValue& stateValue(std::string searchString) 
        throw (InstanceNotFound);
    
    StateData& state(std::string searchString) 
        throw (InstanceNotFound);

    MachineState& machineState();

    virtual void next(int count = 1)
        throw (SimulationExecutionError);
    virtual void step(double count = 1)
        throw (SimulationExecutionError);
    virtual void run()
        throw (SimulationExecutionError);

    virtual void runUntil(UIntWord address)
        throw (SimulationExecutionError);        


    void prepareToStop(StopReason reason);
    unsigned int stopReasonCount() const;
    StopReason stopReason(unsigned int index) const
        throw (OutOfRange);
    bool stoppedByUser() const;
    virtual void killSimulation();

    StopPointManager& stopPointManager();
    MemorySystem& memorySystem();

    bool isSimulationInitialized() const;
    bool isSimulationRunning() const;
    bool isSimulationStopped() const;
    bool isProgramLoaded() const;
    bool isMachineLoaded() const;
    bool hasSimulationEnded() const;

    bool isSequentialSimulation() const;
    bool isCompiledSimulation() const;

    bool executionTracing() const;
    bool busTracing() const;
    bool rfAccessTracing() const;
    bool procedureTransferTracing() const;
    bool profileDataSaving() const;
    bool utilizationDataSaving() const;

    const RFAccessTracker& rfAccessTracker() const
        throw (InstanceNotFound);

    void setExecutionTracing(bool value);
    void setBusTracing(bool value);
    void setRFAccessTracing(bool value);
    void setProcedureTransferTracing(bool value);
    void setProfileDataSaving(bool value);
    void setUtilizationDataSaving(bool value);
    void setTraceDBFileName(const std::string& fileName);

    std::string disassembleInstruction(UIntWord instructionAddress) const;

    std::string programLocationDescription() const;
    InstructionAddress programCounter() const;
    virtual InstructionAddress lastExecutedInstruction() const;
    ClockCycleCount cycleCount() const;
    const TTAProgram::Procedure& currentProcedure() const;
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);

    StateData& findRegister(
        const std::string& rfName, 
        int registerIndex, 
        bool sequential) 
        throw (InstanceNotFound);

    StateData& findBooleanRegister() 
        throw (InstanceNotFound);

    StateData& findPort(
        const std::string& fuName, 
        const std::string& portName) 
        throw (InstanceNotFound);

    const UtilizationStats& utilizationStatistics();
    const ExecutableInstruction& lastExecInstruction() const;
    const ExecutableInstruction& executableInstructionAt(
        InstructionAddress address) const;

    bool automaticFinishImpossible() const;

    void setFUResourceConflictDetection(bool value);
    bool fuResourceConflictDetection() const;

    void setNextInstructionPrinting(bool value);
    bool nextInstructionPrinting() const;
    
    void setSimulationTimeStatistics(bool value);
    bool simulationTimeStatistics() const;

    ExecutionTrace* lastTraceDB();
 
    void setMemoryAccessTracking(bool value);
    bool memoryAccessTracking() const;
    void finishSimulation();

    CycleCount lastRunCycleCount() const;

    SimulationEventHandler& eventHandler();

    double lastRunTime() const;

protected:
    virtual void initializeSimulation();

    void initializeTracing() 
        throw (IOException);
    void initializeDataMemories()
        throw (IllegalProgram);
    void initializeDisassembler() const;
    bool hasStopReason(StopReason reason) const;

    void startTimer();
    void stopTimer();

    /// Machine to run simulation with.
    const TTAMachine::Machine* currentMachine_;
    /// If simulation is initialized, this contains a pointer to the machine
    /// state instance fetched from SimulatorController. Just for a shortcut.
    MachineState* machineState_;
    /// If simulation is initialized, this contains a pointer to the
    /// simulation controller.
    SimulationController* simCon_;
    /// Is the machine owned by SimulatorFrontend or by the client?
    bool machineOwnedByFrontend_;
    /// Program to be simulated.
    const TTAProgram::Program* currentProgram_;
    /// The source file of the program to be simulated. Used to generate
    /// the file name of the trace data base.
    std::string programFileName_;
    /// Is the program owned by SimulatorFrontend or by the client?
    bool programOwnedByFrontend_;   
    /// Is this sequential simulation, that is, is the currentMachine_
    /// an UniversalMachine or not?
    bool sequentialSimulation_;
    /// Is this a compiled simulation or note
    bool compiledSimulation_;
    /// The disassembler used to print out instructions. This is
    /// initialized on demand.
    mutable POMDisassembler* disassembler_;
    /// The file name to store the execution trace data to.
    std::string traceFileName_;
    /// Is execution tracing, i.e., storing the executed instruction
    /// addresses to the trace database, enabled.
    bool executionTracing_;
    /// Is bus tracing, i.e., storing the values of buses in each
    /// clock cycle enabled.
    bool busTracing_;
    /// Is register file (concurrent) access tracking enabled.
    bool rfAccessTracing_;
    /// Is procedure transfer access tracking enabled.
    bool procedureTransferTracing_;
    /// Is saving of profile data to TraceDB enabled.
    bool saveProfileData_;
    /// Is saving of utilization data to TraceDB enabled.
    bool saveUtilizationData_;
    /// The database to use for execution trace data.
    ExecutionTrace* traceDB_;
    /// Last produced execution trace database.
    ExecutionTrace* lastTraceDB_;
    /// The simple execution tracker for storing trace of executed 
    /// instructions.
    ExecutionTracker* executionTracker_;
    /// The tracker for saving bus trace.
    BusTracker* busTracker_;
    /// The register file access tracker.
    RFAccessTracker* rfAccessTracker_;
    /// The procedure transfer tracker.
    ProcedureTransferTracker* procedureTransferTracker_;
    /// The breakpoint manager to be used to bookkeep breakpoints.
    StopPointManager* stopPointManager_;   
    /// Processor utilization statistics.
    UtilizationStats* utilizationStats_;
    /// The source TPEF file.
    TPEF::Binary* tpef_;
    /// Bus trace file stream.
    std::ofstream busTraceStream_;
    /// If this is enabled before initialization, FU resource conflicts are
    /// detected (slows down simulation).
    bool fuResourceConflictDetection_;
    /// If true, the next simulated instructions is printed to stdout when
    /// simulation stops.
    bool printNextInstruction_;
    /// True if the simulation time statistics should be printed out
    bool printSimulationTimeStatistics_;
    /// Flag that indicates is the trace file name set by user.
    bool traceFileNameSetByUser_;
    
    /// If this is enabled before initialization, memory access tracking is
    /// enabled. (slows down simulation)
    bool memoryAccessTracking_;    
    /// Per simulation instance of SimulationEventHandler.
    SimulationEventHandler* eventHandler_;
    /// The cycle count of the last non-interrupted simulation phase.
    ClockCycleCount lastRunCycleCount_;
    /// The wall clock time of the last non-interrupted simulation phase in seconds.
    double lastRunTime_;
    /// The time of the last simulation start. Used to compute simulation speed.
    std::time_t startTime_;
    /// The cycle count when the latest simulation was started. Used to compute simulation speed.
    CycleCount startCycleCount_;
};
#endif
