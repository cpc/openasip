/**
 * @file SimulationController.hh
 *
 * Declaration of SimulationController class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef SIMULATION_CONTROLLER_HH
#define SIMULATION_CONTROLLER_HH

#include "TTASimulationController.hh"

/**
 * Controls the simulation running in stand-alone mode.
 *
 * Owns and is the main client of the machine state model.
 */
class SimulationController : public TTASimulationController {
public:

    SimulationController(
        SimulatorFrontend& frontend,
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

    virtual void reset();

    virtual InstructionAddress programCounter() const;

    virtual MachineState& machineState();
    virtual const InstructionMemory& instructionMemory() const;

    virtual std::string registerFileValue(
        const std::string& rfName, 
        int registerIndex = -1);
    
    virtual SimValue immediateUnitRegisterValue(
    const std::string& iuName, int index = -1);
    
    virtual SimValue FUPortValue(
        const std::string& fuName, 
        const std::string& portName);

private:
    /// Copying not allowed.
    SimulationController(const SimulationController&);
    /// Assignment not allowed.
    SimulationController& operator=(const SimulationController&);

    bool simulateCycle();
    void buildFUResourceConflictDetectors(const TTAMachine::Machine& machine);
    void findExitPoints(
        const TTAProgram::Program& program,
        const TTAMachine::Machine& machine);

    /// Instruction memory.
    InstructionMemory* instructionMemory_;
    
    /// Machine state to be simulated.
    MachineState* machineState_;
    /// Represents a lock signal.
    GlobalLock lock_;
    /// Global control unit.
    GCUState* gcu_; 

    /// The FU resource conflict detectors used to detect conflicts during
    /// simulation.
    FUConflictDetectorIndex fuConflictDetectors_;
    /// Resource conflict detectors in a more quickly traversed container.
    std::vector<FUResourceConflictDetector*> conflictDetectorVector_;
};

#endif
