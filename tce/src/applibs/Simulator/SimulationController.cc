/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @author Jussi Nykänen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005-2015 (pjaaskel-no.spam-cs.tut.fi)
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
    tmpExecutedInstructions_(1) {

    if (fuResourceConflictDetection)
        buildFUResourceConflictDetectors(machine);

    for (int i = 0; i < 1; ++i) {
        frontend_.selectCore(i);
        MachineStateBuilder builder(detailedSimulation);
        MachineState* machineState = NULL;

        if (fuResourceConflictDetection) {
            machineState = builder.build(
                machine, frontend.memorySystem(i), fuConflictDetectors_.at(i));
        } else {
            machineState = builder.build(machine, frontend.memorySystem(i));
        }
        // set the real time clock source to be the simulation
        // cycle counter
        for (int i = 0; i < machineState->FUStateCount(); ++i) {
            FUState& fuState = machineState->fuState(i);
            fuState.context().setCycleCountVariable(clockCount_);
        }   
        machineStates_.push_back(machineState);
    }
    frontend_.selectCore(0);
    
    SimProgramBuilder programBuilder;
    for (int i = 0; i < 1; ++i) {
        InstructionMemory* instructionMemory = 
            programBuilder.build(program, *machineStates_[i]);
        instructionMemories_.push_back(instructionMemory);
    }  

    findExitPoints(program, machine);
    reset();
}

/**
 * Destructor.
 */
SimulationController::~SimulationController() {

    SequenceTools::deleteAllItems(machineStates_);
    SequenceTools::deleteAllItems(instructionMemories_);
    SequenceTools::deleteAllItems(conflictDetectorVector_);
}

/**
 * Returns a reference to the currently selected machine state model.
 */
MachineState&
SimulationController::machineState(int core) {
    if (core == -1) core = frontend_.selectedCore();
    return *machineStates_.at(core);
}

/**
 * Simulates a cycle.
 *
 * @return false in case there are no more instructions to execute,
 * that is, the simulation ended sucessfully, true in case there are
 * more instructions to execute.
 */
bool
SimulationController::simulateCycle() {

    tmpExecutedInstructions_ = lastExecutedInstruction_;

    // The number of cores that have reached the exit function,
    // use this to stop automatically after all of them have
    // called it.
    unsigned finishedCoreCount = 0;
    bool finished = false;
    for (int core = 0; core < 1; ++core) {
        MachineState* machineState = machineStates_[core];

        if (machineState->isFinished()) {
            ++finishedCoreCount;
            continue;
        }
        
        GCUState& gcu = machineState->gcuState();
        const InstructionAddress& pc = gcu.programCounter();

        MemorySystem* memorySystem = &frontend_.memorySystem(core);
        try {
            machineState->clearBuses();

            ExecutableInstruction* instruction = 
                &(instructionMemories_[core]->instructionAt(pc));

            instruction->execute();

            tmpExecutedInstructions_[core] = pc;
        
            machineState->endClockOfAllFUStates();

            if (!gcu.isIdle()) {
                gcu.endClock();
            }
            
            memorySystem->advanceClockOfLocalMemories();
            machineState->advanceClockOfAllFUStates();

            ++gcu.programCounter();
            if (!gcu.isIdle())
                gcu.advanceClock();

            machineState->advanceClockOfAllGuardStates();
            machineState->advanceClockOfAllLongImmediateUnitStates();

            // check if the instruction was a return point from the program or
            // the next executed instruction would be sequentially over the
            // instruction space (PC+1 would overflow out of the program)
            if (instruction->isExitPoint() || 
                gcu.programCounter() == firstIllegalInstructionIndex_) {
                machineState->setFinished();
                ++finishedCoreCount;
            } 
        } catch (const Exception& e) {
            frontend_.selectCore(core);
            frontend_.reportSimulatedProgramError(
                SimulatorFrontend::RES_FATAL,
                e.errorMessage());
            prepareToStop(SRE_RUNTIME_ERROR);
            return false;
        } 
    }
    
    if (finishedCoreCount == 1)
        finished = true;

    // assume all cores have identical memory systems, thus it's enough
    // to advance the simulation clock only once for the first core's
    // memory system's shared memory instance
    frontend_.memorySystem(0).advanceClockOfSharedMemories();    

    // detect FU pipeline resource conflicts
    size_t conflictDetectorVectorSize = conflictDetectorVector_.size();
    for (std::size_t i = 0; i < conflictDetectorVectorSize; ++i) {
        FUResourceConflictDetector& detector = 
            *conflictDetectorVector_[i];
        if (!detector.isIdle())
            detector.advanceClock();
    }

    frontend_.eventHandler().handleEvent(SimulationEventHandler::SE_CYCLE_END);

    lastExecutedInstruction_ = tmpExecutedInstructions_;

    ++clockCount_;

    if (finished) {
        state_ = STA_FINISHED;
        stopRequested_ = true;
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

        if (selectedMachineState().gcuState().programCounter() == address) {
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

void
SimulationController::findExitPoints(
    const TTAProgram::Program& program,
    const TTAMachine::Machine& machine) {

    std::set<InstructionAddress> exitPoints_ =
        findProgramExitPoints(program, machine);

    for (std::set<InstructionAddress>::iterator it = exitPoints_.begin();
         it != exitPoints_.end(); ++it) {
        for (std::size_t i = 0; i < instructionMemories_.size(); ++i) {
            instructionMemories_[i]->instructionAt(*it).setExitPoint(true);
        }
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
    state_ = STA_INITIALIZED;

    for (int core = 0; core < 1; ++core) {
        machineStates_.at(core)->gcuState().programCounter() = initialPC_;
        machineStates_.at(core)->setFinished(false);
        machineStates_.at(core)->resetAllFUs();
        instructionMemories_.at(core)->resetExecutionCounts();
    }

    for (std::size_t vec = 0; vec < conflictDetectorVector_.size(); ++vec) {
        conflictDetectorVector_.at(vec)->reset();
    }
}

/**
 * Returns the program counter value of the currently selected core.
 */
InstructionAddress
SimulationController::programCounter() const {
    return machineStates_.at(frontend_.selectedCore())->gcuState().programCounter();
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

    return (selectedMachineState().longImmediateUnitState(iuName)).
        registerValue(index);
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

    return (selectedMachineState().portState(portName, fuName)).value();
}

/**
 * Builds the FU resource conflict detectors for each FU in the machine.
 *
 * Uses the "lazy FSA" detection model.
 */
void
SimulationController::buildFUResourceConflictDetectors(
    const TTAMachine::Machine& machine) {


    for (int core = 0; core < 1; ++core) {
        const TTAMachine::Machine::FunctionUnitNavigator nav = 
            machine.functionUnitNavigator();

        fuConflictDetectors_.push_back(FUConflictDetectorIndex());

        for (int i = 0; i < nav.count(); ++i) {
            const TTAMachine::FunctionUnit& fu = *nav.item(i);
            FUResourceConflictDetector* detector = 
                new FSAFUResourceConflictDetector(fu);
            fuConflictDetectors_[core][fu.name()] = detector;
            conflictDetectorVector_.push_back(detector);
        }
    }
}

const InstructionMemory&
SimulationController::instructionMemory(int core) const {
    if (core == -1) core = frontend_.selectedCore();
    return *instructionMemories_.at(core);
}

MachineState&
SimulationController::selectedMachineState() { 
    return *machineStates_.at(frontend_.selectedCore());
}

InstructionMemory&
SimulationController::selectedInstructionMemory() {
    return *instructionMemories_.at(frontend_.selectedCore());
}
