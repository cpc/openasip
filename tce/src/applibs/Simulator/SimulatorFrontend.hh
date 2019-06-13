/*
    Copyright (c) 2002-2010 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file SimulatorFrontend.hh
 *
 * Declaration of SimulatorFrontend class
 *
 * @author Pekka J‰‰skel‰inen 2005-2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATOR_FRONTEND
#define TTA_SIMULATOR_FRONTEND

#include <fstream>
#include <string>
#include <iostream>
#include <ostream>
#include <ctime>

#include <boost/timer.hpp>
#include <set>

#include "Exception.hh"
#include "SimulationController.hh"
#include "RemoteController.hh"
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

namespace TTAProgram {
    class Program;
    class Procedure;
}

namespace TTAMachine {
    class AddressSpace;
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

    /// The severities of runtime errors.
    typedef enum {
        RES_MINOR, ///< Minor runtime error, no abort necessary.
        RES_FATAL  ///< Fatal runtime error, there is a serious error in the
                   /// simulated program, thus it makes no sense to go on
                   /// with the simulation.
    } RuntimeErrorSeverity;

    /// Which type of simulation this SimulatorFrontend controls or connects to.
    typedef enum {
        SIM_NORMAL,   ///< Default, interpreted simulation (debugging engine).
        SIM_COMPILED, ///< Compiled, faster simulation.
        SIM_REMOTE,   ///< Remote debugger, not a simulator at all
        SIM_CUSTOM    ///< User-implemented remote HW debugger
    } SimulationType;

    SimulatorFrontend(SimulationType backend = SIM_NORMAL);
    virtual ~SimulatorFrontend();

    virtual void loadProgram(const std::string& fileName);

    virtual void loadMachine(const std::string& fileName);

    void loadProgram(const TTAProgram::Program& program);

    void loadMachine(const TTAMachine::Machine& machine);

    void loadProcessorConfiguration(const std::string& fileName);

    const TTAMachine::Machine& machine() const;
    const TTAProgram::Program& program() const;

    const SimValue& stateValue(std::string searchString);

    StateData& state(std::string searchString);

    MachineState& machineState();

    virtual void next(int count = 1);
    virtual void step(double count = 1);
    virtual void run();

    virtual void runUntil(UIntWord address);

    void prepareToStop(StopReason reason);
    unsigned int stopReasonCount() const;
    StopReason stopReason(unsigned int index) const;
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

    bool isCompiledSimulation() const;
    bool isTCEDebugger() const;
    bool isCustomDebugger() const;
    void setCompiledSimulationLeaveDirty(bool dirty) { 
        leaveCompiledDirty_ = dirty;
    }

    bool executionTracing() const;
    bool busTracing() const;
    bool rfAccessTracing() const;
    bool procedureTransferTracing() const;
    bool profileDataSaving() const;
    bool utilizationDataSaving() const;
    bool staticCompilation() const;

    const RFAccessTracker& rfAccessTracker() const;

    void setCompiledSimulation(bool value);
    void setExecutionTracing(bool value);
    void setBusTracing(bool value);
    void setRFAccessTracing(bool value);
    void setProcedureTransferTracing(bool value);
    void setProfileDataSaving(bool value);
    void setUtilizationDataSaving(bool value);
    void setTraceDBFileName(const std::string& fileName);
    void setTimeout(unsigned int value);
    void setStaticCompilation(bool value);
    
    std::ostream& outputStream();
    void setOutputStream(std::ostream& stream);

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
        const std::string& iuName,
        int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);

    StateData& findRegister(const std::string& rfName, int registerIndex);

    StateData& findBooleanRegister();

    StateData& findPort(const std::string& fuName, const std::string& portName);

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

    void setZeroFillMemoriesOnReset(bool val) 
        { zeroFillMemoriesOnReset_ = val; }

    void setDetailedSimulation(bool val) 
        { detailedSimulation_ = val; }

    CycleCount lastRunCycleCount() const;

    SimulationEventHandler& eventHandler();

    double lastRunTime() const;

    void reportSimulatedProgramError(
        RuntimeErrorSeverity severity, const std::string& description);
    std::string programErrorReport(
        RuntimeErrorSeverity severity, std::size_t index);
    std::size_t programErrorReportCount(
        RuntimeErrorSeverity severity);
    void clearProgramErrorReports();
    
    friend void timeoutThread(unsigned int timeout, SimulatorFrontend* simFE);

    bool compareState(SimulatorFrontend& other, std::ostream* differences=NULL);

    void initializeDataMemories(const TTAMachine::AddressSpace* onlyOne=NULL);

protected:
    virtual void initializeSimulation();

    void initializeTracing();
    void initializeDisassembler() const;
    void initializeMemorySystem();
    void setControllerForMemories(RemoteController* con);
    bool hasStopReason(StopReason reason) const;

    void startTimer();
    void stopTimer();

    /// A type for storing a program error description.
    typedef std::pair<RuntimeErrorSeverity, std::string>
    ProgramErrorDescription;
    /// Container for simulated program error descriptions.
    typedef std::vector<ProgramErrorDescription> ProgramErrorDescriptionList;
    
    /// Machine to run simulation with.
    const TTAMachine::Machine* currentMachine_;
    /// If simulation is initialized, this contains a pointer to the machine
    /// state instance fetched from SimulatorController. Just for a shortcut.
    MachineState* machineState_;
    /// If simulation is initialized, this contains a pointer to the
    /// simulation controller.
    TTASimulationController* simCon_;
    /// Is the machine owned by SimulatorFrontend or by the client?
    bool machineOwnedByFrontend_;
    /// Program to be simulated.
    const TTAProgram::Program* currentProgram_;
    /// The source file of the program to be simulated. Used to generate
    /// the file name of the trace data base.
    std::string programFileName_;
    /// Is the program owned by SimulatorFrontend or by the client?
    bool programOwnedByFrontend_;   
    /// Type of "backend" this Frontend has
    SimulationType currentBackend_;
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
    /// True if the compiled simulation should use static compilation
    bool staticCompilation_;
    /// Flag that indicates is the trace file name set by user.
    bool traceFileNameSetByUser_;
    /// Default output stream
    std::ostream* outputStream_;
    
    /// If this is enabled before initialization, memory access tracking is
    /// enabled. (slows down simulation)
    bool memoryAccessTracking_;    
    /// Per simulation instance of SimulationEventHandler.
    SimulationEventHandler* eventHandler_;
    /// The cycle count of the last non-interrupted simulation phase.
    ClockCycleCount lastRunCycleCount_;
    /// The wall clock time of the last non-interrupted simulation phase in 
    /// seconds.
    double lastRunTime_;
    /// The time of the last simulation start. Used to compute simulation speed.
    std::time_t startTime_;
    /// The cycle count when the latest simulation was started. Used to 
    /// compute simulation speed.
    CycleCount startCycleCount_;
    /// Simulation timeout in seconds
    unsigned int simulationTimeout_;
    /// Runtime error reports.
    ProgramErrorDescriptionList programErrorReports_;
    /// True in case the compilation simulation should not cleanup at
    /// destruction the engine source files.
    bool leaveCompiledDirty_;
    /// The simulation models of the memories in the currently loaded machine.
    MemorySystem* memorySystem_;
    /// Set to true in case the memories should be set to zero at reset.
    bool zeroFillMemoriesOnReset_;
    /// Set to true in case should build a detailed model which simulates
    /// FU stages, possibly with an external system-level model.
    bool detailedSimulation_;
};
#endif
