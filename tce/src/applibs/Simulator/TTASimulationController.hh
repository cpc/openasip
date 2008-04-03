/**
 * @file CompiledSimController.hh
 *
 * Declaration of TTASimulationController class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
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
 * An abstract interface class for controlling the simulation
 *
 * Owns and is the main client of the machine state model.
 */
class TTASimulationController {
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

    TTASimulationController(
        SimulatorFrontend & frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program,
        bool memoryAccessTracking,
        bool directAccessMemory);

    virtual ~TTASimulationController();

    virtual void step(double count = 1)
        throw (SimulationExecutionError) = 0;

    virtual void next(int count = 1)
        throw (SimulationExecutionError) = 0;

    virtual void run()
        throw (SimulationExecutionError) = 0;

    virtual void runUntil(UIntWord address)
        throw (SimulationExecutionError) = 0;

    virtual void reset() = 0;
    
    
    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1) = 0;
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1) = 0;
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName) = 0;
    
    virtual void prepareToStop(StopReason reason);
    virtual unsigned int stopReasonCount() const;
    virtual StopReason stopReason(unsigned int index) const
        throw (OutOfRange);
    virtual SimulationStatus state() const;
    virtual InstructionAddress programCounter() const = 0;
    virtual InstructionAddress lastExecutedInstruction() const;
    virtual ClockCycleCount clockCount() const;
    virtual MemorySystem& memorySystem();
    virtual SimulatorFrontend& frontend();
    virtual bool automaticFinishImpossible() const;
    
    virtual std::set<InstructionAddress> findProgramExitPoints(
    const TTAProgram::Program& program,
    const TTAMachine::Machine& machine) const;

protected:
    /// Copying not allowed.
    TTASimulationController(const TTASimulationController&);
    /// Assignment not allowed.
    TTASimulationController& operator=(const TTASimulationController&);
    
    /// The container type for reasons why simulation stop was requested.
    typedef std::set<StopReason> StopReasonContainer;
    
    virtual void initializeMemorySystem(const TTAMachine::Machine& machine);

    /// Reference to the simulator frontend
    SimulatorFrontend& frontend_;
    /// The source Machine Object Model.
    const TTAMachine::Machine& sourceMachine_;
    /// Program object model of the simulated program.
    const TTAProgram::Program& program_;
   
     /// The memory system
    MemorySystem* memorySystem_;
        
    /// Flag indicating that simulation should stop.
    bool stopRequested_;
    /// The set of reasons the simulation was stopped.
    StopReasonContainer stopReasons_;
 
    /// The current state of the simulation.
    SimulationStatus state_;
    /// How many clock cycles have been simulated.
    ClockCycleCount clockCount_;
    /// The address of the last executed instruction.
    InstructionAddress lastExecutedInstruction_;
    /// The address of the first executed instruction.
    InstructionAddress initialPC_;
    /// If this is true, simulation cannot be finished automatically.
    mutable bool automaticFinishImpossible_;
    /// True if memory access tracking is enabled.
    bool memoryTracking_;
    /// True if simulation is done with sequential machine.
    bool sequentialSimulation_;
    /// The index of the first illegal instruction in the instruction
    /// sequence.
    mutable InstructionAddress firstIllegalInstructionIndex_;
    /// If direct access memory is used.
    bool directAccessMemory_;
};

#endif
