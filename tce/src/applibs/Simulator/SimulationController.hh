/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file SimulationController.hh
 *
 * Declaration of SimulationController class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
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
        bool detailedSimulation = false);

    virtual ~SimulationController();

    virtual void step(double count = 1);

    virtual void next(int count = 1);

    virtual void run();

    virtual void runUntil(UIntWord address);

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
    /// Global control unit.
    GCUState* gcu_; 

    /// The FU resource conflict detectors used to detect conflicts during
    /// simulation.
    FUConflictDetectorIndex fuConflictDetectors_;
    /// Resource conflict detectors in a more quickly traversed container.
    std::vector<FUResourceConflictDetector*> conflictDetectorVector_;
};

#endif
