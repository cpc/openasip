/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file SimulationController.cc
 *
 * Definition of SimulationController class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005-2009 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <climits>

#include "SimulationController.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "StateLocator.hh"
#include "MachineStateBuilder.hh"
#include "SimProgramBuilder.hh"
#include "Program.hh"
#include "InstructionMemory.hh"
#include "GCUState.hh"
#include "ExecutableInstruction.hh"
#include "Exception.hh"
#include "UniversalMachine.hh"
#include "UniversalFunctionUnit.hh"
#include "IdealSRAM.hh"
#include "DirectAccessMemory.hh"
#include "Memory.hh"
#include "MemorySystem.hh"
#include "FunctionUnit.hh"
#include "Section.hh"
#include "DataSection.hh"
#include "ASpaceElement.hh"
#include "Instruction.hh"
#include "Procedure.hh"
#include "SimulationEventHandler.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "ControlUnit.hh"
#include "StringTools.hh"
#include "SpecialRegisterPort.hh"
#include "Operation.hh"
#include "FUConflictDetectorIndex.hh"
#include "FSAFUResourceConflictDetector.hh"
#include "SimulatorFrontend.hh"
#include "MemoryProxy.hh"
#include "UnboundedRegisterFile.hh"
#include "RegisterFileState.hh"
#include "MathTools.hh"

using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param machine Machine to be simulated.
 * @param memSys Memory system.
 * @param fuResourceConflictDetection Should the model detect FU resource
 * conflicts.
 * @exception Exception Exceptions while building the simulation models
 * are thrown forward.
 */
SimulationController::SimulationController(
    SimulatorFrontend& frontend,
    const Machine& machine, 
    const Program& program,
    bool fuResourceConflictDetection,
    bool detailedSimulation) :
    TTASimulationController(frontend, machine, program),
    machineState_(NULL), gcu_(NULL) {

    MachineStateBuilder builder(detailedSimulation);

    if (fuResourceConflictDetection)
        buildFUResourceConflictDetectors(machine);

    machineState_ = builder.build(
        machine, frontend.memorySystem(), fuConflictDetectors_);
    // set the real time clock source to be the simulation
    // cycle counter
    for (int i = 0; i < machineState_->FUStateCount(); ++i) {
        FUState& fuState = machineState_->fuState(i);
        fuState.context().setCycleCountVariable(clockCount_);
    }        
    assert(machineState_ != NULL);
    
    gcu_ = &machineState_->gcuState();

    SimProgramBuilder programBuilder;
    instructionMemory_ = programBuilder.build(program, *machineState_);

    findExitPoints(program, machine);
    reset();
}

/**
 * Destructor.
 */
SimulationController::~SimulationController() {

    delete machineState_;
    machineState_ = NULL;
    delete instructionMemory_;
    instructionMemory_ = NULL;


    AssocTools::deleteAllValues(fuConflictDetectors_);
    conflictDetectorVector_.clear();
}

/**
 * Returns a reference to the machine state model.
 *
 * @return A reference to the machine state model.
 */
MachineState&
SimulationController::machineState() {
    assert(machineState_ != NULL);
    return *machineState_;
}

/**
 * Simulates a cycle.
 *
 * @return false in case there are no more instructions to execute,
 * that is, the simulation ended sucessfully, true in case there are
 * more instructions to execute.
 */
inline bool
SimulationController::simulateCycle() {

    const InstructionAddress& pc = gcu_->programCounter();

    try {
        machineState_->clearBuses();

        ExecutableInstruction& instruction = 
            instructionMemory_->instructionAt(pc);
        instruction.execute();
        
        lastExecutedInstruction_ = pc;
        machineState_->endClockOfAllFUStates();

        if (!gcu_->isIdle())
            gcu_->endClock();

        memorySystem().advanceClockOfLocalMemories();
        memorySystem().advanceClockOfSharedMemories();
        machineState_->advanceClockOfAllFUStates();

        for (std::size_t i = 0; i < conflictDetectorVector_.size(); ++i) {
            FUResourceConflictDetector& detector = *conflictDetectorVector_[i];
            if (!detector.isIdle())
                detector.advanceClock();
        }

        ++gcu_->programCounter();
        if (!gcu_->isIdle())
            gcu_->advanceClock();

        machineState_->advanceClockOfAllGuardStates();
        machineState_->advanceClockOfAllLongImmediateUnitStates();

        frontend_.eventHandler().handleEvent(
            SimulationEventHandler::SE_CYCLE_END);

        ++clockCount_;

        // check if the instruction was a return point from the program or
        // the next executed instruction would be sequentially over the
        // instruction space (PC+1 would overflow out of the program)
        if (instruction.isExitPoint() || 
            gcu_->programCounter() == firstIllegalInstructionIndex_) {
            state_ = STA_FINISHED;
            stopRequested_ = true;
            return false;
        }

    } catch (const Exception& e) {
        frontend_.reportSimulatedProgramError(
            SimulatorFrontend::RES_FATAL,
            e.errorMessage());
        prepareToStop(SRE_RUNTIME_ERROR);
        return false;
    } 

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_NEW_INSTRUCTION);
    return true;
}

/**
 * Advance simulation by a given amout of cycles.
 *
 * @param count The number of cycles the simulation is advanced.
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 */
void
SimulationController::step(double count) {
    assert(state_ == STA_STOPPED || state_ == STA_INITIALIZED);
    stopReasons_.clear();
    state_ = STA_RUNNING;
    stopRequested_ = false;

    double counter = 0;
    while (counter < count && !stopRequested_) {
        simulateCycle();
        ++counter;
    }

    if (counter == count) {
        prepareToStop(SRE_AFTER_STEPPING);
    }

    if (state_ != STA_FINISHED)
        state_ = STA_STOPPED;

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Advance simulation by a given amout of steps and skip procedure
 * calls.
 *
 * @param count Number of steps to simulate.
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 */
void
SimulationController::next(int count) {
    stopRequested_ = false;
    stopReasons_.clear();
    state_ = STA_RUNNING;

    bool inCalledProcedure = false;
    const Procedure& procedureWhereStartedStepping = 
        dynamic_cast<const Procedure&>(
            program_.instructionAt(programCounter()).parent());

    int counter = 0;
    while (!stopRequested_ && counter < count) {

        // simulate cycles until we come back to the procedure we started
        // simulating from or until the program ends
        do {
            const bool programEnded = !simulateCycle();

            if (programEnded) {
                prepareToStop(SRE_AFTER_UNTIL);
            } else {
                const Procedure& currentProcedure =
                    dynamic_cast<const Procedure&>(
                        program_.instructionAt(programCounter()).parent());
                inCalledProcedure = 
                    (&procedureWhereStartedStepping != &currentProcedure);
            }
        } while (inCalledProcedure && !stopRequested_);
        ++counter;
    }

    if (counter == count && !inCalledProcedure) {
        prepareToStop(SRE_AFTER_STEPPING);
    }

    if (state_ != STA_FINISHED)
        state_ = STA_STOPPED;

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Advance simulation until a condition for stopping is enabled.
 *
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 */
void
SimulationController::run() {
    stopRequested_ = false;
    stopReasons_.clear();
    state_ = STA_RUNNING;

    while (!stopRequested_) {
        simulateCycle();
    }
    if (state_ != STA_FINISHED)
        state_ = STA_STOPPED;

    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Advance simulation until given address is reached.
 *
 * @param address The instruction address to reach.
 * @exception SimulationExecutionError If a runtime error occurs in 
 *                                     the simulated program.
 */
void
SimulationController::runUntil(UIntWord address) {
    stopRequested_ = false;
    stopReasons_.clear();
    state_ = STA_RUNNING;

    while (!stopRequested_) {
        simulateCycle();

        if (state_ == STA_FINISHED)
            return;

        if (gcu_->programCounter() == address) {
            prepareToStop(SRE_AFTER_UNTIL);
            state_ = STA_STOPPED;
            frontend_.eventHandler().handleEvent(
                SimulationEventHandler::SE_SIMULATION_STOPPED);
            return;
        }
    }
    
    if (state_ != STA_FINISHED) {
        state_ = STA_STOPPED;
    }
    
    frontend_.eventHandler().handleEvent(
        SimulationEventHandler::SE_SIMULATION_STOPPED);
}

/**
 * Builds the FU resource conflict detectors for each FU in the given machine.
 *
 * Uses the "lazy FSA" detection model.
 *
 * @param machine The machine to build FU conflict detectors for.
 */
void
SimulationController::buildFUResourceConflictDetectors(
    const TTAMachine::Machine& machine) {

    const TTAMachine::Machine::FunctionUnitNavigator nav = 
        machine.functionUnitNavigator();

    for (int i = 0; i < nav.count(); ++i) {
        const TTAMachine::FunctionUnit& fu = *nav.item(i);
        FUResourceConflictDetector* detector = 
            new FSAFUResourceConflictDetector(fu);
        fuConflictDetectors_[fu.name()] = detector;
        conflictDetectorVector_.push_back(detector);
    }
}

void 
SimulationController::findExitPoints(
    const TTAProgram::Program& program,
    const TTAMachine::Machine& machine) {
    std::set<InstructionAddress> exitPoints_ = findProgramExitPoints(
        program, machine);
    
    for (std::set<InstructionAddress>::iterator it = exitPoints_.begin(); 
         it != exitPoints_.end(); ++it) {
             instructionMemory_->instructionAt(*it).setExitPoint(true);
    }
}

/**
 * Resets the simulation so it can be started from the beginning.
 *
 * Resets the program counter to its initial value, and also clears the
 * instrution execution counts and states of possible FU resource conflict
 * detectors.
 */
void
SimulationController::reset() {

    state_ = STA_INITIALIZING;
    stopRequested_ = false;
    clockCount_ = 0;
    gcu_->programCounter() = initialPC_;
    machineState_->resetAllFUs();
    state_ = STA_INITIALIZED;
    instructionMemory_->resetExecutionCounts();

    for (FUConflictDetectorIndex::iterator d = fuConflictDetectors_.begin();
         d != fuConflictDetectors_.end(); ++d) {
        FUResourceConflictDetector& detector = *(*d).second;
        detector.reset();
    }
}

/**
 * Returns the program counter value.
 *
 * @return Program counter value.
 */
InstructionAddress
SimulationController::programCounter() const {
    return gcu_->programCounter();
}

/**
 * Returns the instruction memory instance.
 *
 * This is mainly used by clients to fetch instruction execution counts
 * to calculate simulation statistics.
 *
 * @return The instruction memory instance of the currently simulated program.
 */
const InstructionMemory& 
SimulationController::instructionMemory() const {
    return *instructionMemory_;
}

/**
 * Returns a string containing the value(s) of the register file
 * 
 * @param rfName name of the register file to search for
 * @param registerIndex index of the register. if -1, all registers are listed
 * @return A string containing the value(s) of the register file
 * @exception InstanceNotFound If the register cannot be found.
 */
std::string 
SimulationController::registerFileValue(
    const std::string& rfName, int registerIndex) {
    
    std::string stringValue("");

    if (registerIndex >= 0) {
        stringValue += frontend_.findRegister(rfName, registerIndex).value().
            hexValue();
    } else {
        Machine::RegisterFileNavigator navigator = 
            sourceMachine_.registerFileNavigator();
        RegisterFile& rf = *navigator.item(rfName);
        
        bool firstReg = true;
        for (int i = 0; i < rf.numberOfRegisters(); ++i) {
            if (!firstReg) 
                stringValue += "\n";
            const std::string registerName = 
                rfName + "." + Conversion::toString(i);

            SimValue value = frontend_.findRegister(rfName, i).value();
            stringValue += registerName + " " + value.hexValue();

            firstReg = false;
        }
    }
    
    return stringValue;
}

/**
 * Returns the current value of a IU register
 * 
 * @param iuName name of the immediate unit
 * @param index index of the register
 * @return Current value of a IU register
 */
SimValue 
SimulationController::immediateUnitRegisterValue(
    const std::string& iuName, int index) {

    assert(machineState_ != NULL);
    return (machineState_->longImmediateUnitState(iuName)).registerValue(index);
}

/**
 * Returns the current value of a FU port
 * 
 * @param fuName name of the function unit
 * @param portName name of the FU port
 * @return Current value of a FU port
 */
SimValue
SimulationController::FUPortValue(
    const std::string& fuName, const std::string& portName) {

    assert(machineState_ != NULL);
    return (machineState_->portState(portName, fuName)).value();
}

