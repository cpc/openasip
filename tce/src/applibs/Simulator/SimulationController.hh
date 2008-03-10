/**
 * @file SimulationController.hh
 *
 * Declaration of SimulationController class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SIMULATION_CONTROLLER_HH
#define TTA_SIMULATION_CONTROLLER_HH

#include <set>
#include <vector>

#include "GlobalLock.hh"
#include "Exception.hh"
#include "BaseType.hh"
#include "SimulatorConstants.hh"
#include "FUConflictDetectorIndex.hh"

class MemorySystem;
class MachineState;
class InstructionMemory;
class GCUState;
class IllegalMachine;
class SimValue;
class InstanceNotFound;
class StateData;
class MemorySystem;
class Memory;
class SimulatorFrontend;

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

/**
 * Controls the simulation running in stand-alone mode.
 *
 * Owns and is the main client of the machine state model.
 */
class SimulationController {
public:

    /// The states of simulation.
    enum SimulationStatus {
        STA_INITIALIZING,///< Simulation is being initialized.
        STA_INITIALIZED, ///< Simulation initialized and ready to run.
        STA_RUNNING,     ///< A run command (run, stepi, until...) given.
        STA_STOPPED,     ///< Simulation stopped for some reason.
        STA_FINISHED     
        ///< Simulation ended after executing the last instruction.
    };

    SimulationController(
        SimulatorFrontend & frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program,
        bool fuResourceConflictDetection = true,
        bool memoryAccessTracking = false,
        bool directAccessMemory = false);

    virtual ~SimulationController();

    virtual void step(double count = 1)
        throw (SimulationExecutionError);

    virtual void next(int count = 1)
        throw (SimulationExecutionError);

    virtual void run()
        throw (SimulationExecutionError);

    virtual void runUntil(UIntWord address)
        throw (SimulationExecutionError);

    void prepareToStop(StopReason reason);
    virtual void reset();
    unsigned int stopReasonCount() const;
    StopReason stopReason(unsigned int index) const
        throw (OutOfRange);

    SimulationStatus state() const;

    virtual InstructionAddress programCounter() const;
    virtual InstructionAddress lastExecutedInstruction() const;
    virtual ClockCycleCount clockCount() const;

    MachineState& machineState();
    MemorySystem& memorySystem();
    const InstructionMemory& instructionMemory() const;
    
    SimulatorFrontend & frontend();

    bool automaticFinishImpossible() const;
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);

protected:
    /// The container type for reasons why simulation stop was requested.
    typedef std::set<StopReason> StopReasonContainer;

    /// Copying not allowed.
    SimulationController(const SimulationController&);
    /// Assignment not allowed.
    SimulationController& operator=(const SimulationController&);
    bool simulateCycle();
    void initializeMemorySystem(const TTAMachine::Machine& machine);
    void findProgramExitPoints(
        const TTAProgram::Program& program,
        const TTAMachine::Machine& machine);

    void buildFUResourceConflictDetectors(const TTAMachine::Machine& machine);

    /// The simulator frontend
    SimulatorFrontend & frontend_;

    /// The source Machine Object Model.
    const TTAMachine::Machine& sourceMachine_;
    /// Program object model of the simulated program.
    const TTAProgram::Program& program_;
    /// Machine state to be simulated.
    MachineState* machineState_;
    /// The memory model storage.
    MemorySystem* memorySystem_;
    /// Represents a lock signal.
    GlobalLock lock_;
    /// Instruction memory.
    InstructionMemory* instructionMemory_;
    /// Global control unit.
    GCUState* gcu_;
    /// Flag indicating that simulation should stop.
    bool stopRequested_;
    /// True if the last executed is a return from a procedure. Used to 
    /// evaluate the finishing condition.
    bool lastInstructionIsAProcedureReturn_;
    /// The current state of the simulation.
    SimulationStatus state_;
    /// The set of reasons the simulation was stopped.
    StopReasonContainer stopReasons_;
    /// How many clock cycles have been simulated.
    ClockCycleCount clockCount_;
    /// The address of the last executed instruction.
    InstructionAddress lastExecutedInstruction_;
    /// The address of the first executed instruction.
    InstructionAddress initialPC_;
    /// If this is true, simulation cannot be finished automatically.
    bool automaticFinishImpossible_;
    /// The FU resource conflict detectors used to detect conflicts during
    /// simulation.
    FUConflictDetectorIndex fuConflictDetectors_;
    /// Resource conflict detectors in a more quickly traversed container.
    std::vector<FUResourceConflictDetector*> conflictDetectorVector_;
    /// True if memory access tracking is enabled.
    bool memoryTracking_;
    /// True if simulation is done with sequential machine.
    bool sequentialSimulation_;
    /// The index of the first illegal instruction in the instruction
    /// sequence.
    InstructionAddress firstIllegalInstructionIndex_;
    /// If direct access memory is used.
    bool directAccessMemory_;
};

#endif
