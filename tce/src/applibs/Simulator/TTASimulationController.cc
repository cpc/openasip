/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CompiledSimController.cc
 *
 * Definition of TTASimulationController class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <climits>

#include "TTASimulationController.hh"
#include "MemorySystem.hh"
#include "Machine.hh"
#include "IdealSRAM.hh"
#include "ControlUnit.hh"
#include "MemoryProxy.hh"
#include "UniversalMachine.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "InstructionMemory.hh"
#include "Move.hh"
#include "ExecutableInstruction.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "SimulatorFrontend.hh"

using namespace TTAMachine;
using namespace TTAProgram;

/**
 * Constructor.
 *
 * @param machine Machine to be simulated.
 * @param memSys Memory system.
 * @param fuResourceConflictDetection Should the model detect FU resource
 * conflicts.
 * @param memoryAccessTracking Sets memory access tracking on and off.
 * @exception Exception Exceptions while building the simulation models
 * are thrown forward.
 */
TTASimulationController::TTASimulationController(
    SimulatorFrontend & frontend,
    const Machine& machine, 
    const Program& program) :
    frontend_(frontend),
    sourceMachine_(machine), program_(program),
    state_(STA_INITIALIZING), clockCount_(0), 
    initialPC_( program.entryAddress().location()),  
    automaticFinishImpossible_(true),
    firstIllegalInstructionIndex_(UINT_MAX) {
}

/**
 * Destructor.
 */
TTASimulationController::~TTASimulationController() {
}

/**
 * Get ready to return control to the client.
 *
 * Functions name is has "prepare" in its name even though it always
 * is able to stop the simulation. The reason for this is that it does not
 * stop the simulation in the middle of simulating a clock cycle, but after
 * the current clock cycle is simulated.
 *
 * @param reason The reason why simulation should be stopped.
 */
void
TTASimulationController::prepareToStop(StopReason reason) {
    stopRequested_ = true;
    stopReasons_.insert(reason);
}

/**
 * Returns the count of stop reasons.
 *
 * @return The count of stop reasons.
 */
unsigned int 
TTASimulationController::stopReasonCount() const {
    return stopReasons_.size();
}

/**
 * Returns the stop reason with the given index.
 *
 * @param index The wanted index.
 * @return The stop reason at the given index.
 * @exception OutOfRange If the given index is out of range.
 */
StopReason
TTASimulationController::stopReason(unsigned int index) const {
    if (index >= stopReasonCount()) {
        throw OutOfRange(
            __FILE__, __LINE__, __func__, "Stop reason index out of range.");
    }
    StopReasonContainer::const_iterator i = stopReasons_.begin();
    unsigned int count = 0;
    while (i != stopReasons_.end()) {
        if (index == count) {
            return (*i);
        }
        ++count;
        ++i;
    }
    // dummy to stop compiler from warning
    throw 0;
}

/**
 * Returns the state of the simulation.
 *
 * @return The state of the simulation.
 */
TTASimulationController::SimulationStatus 
TTASimulationController::state() const {
    return state_;
}

/**
 * Returns the address of the last executed instruction.
 *
 * @return Address of the last executed instruction.
 */
InstructionAddress 
TTASimulationController::lastExecutedInstruction() const {
    return lastExecutedInstruction_;
}

/**
 * Returns the count of clock cycles simulated.
 *
 * @return Count of simulated clock cycles.
 */
ClockCycleCount 
TTASimulationController::clockCount() const {
    return clockCount_;
}

/**
 * Returns a reference to the memory system.
 *
 * @return A reference to the memory system.
 */
MemorySystem& 
TTASimulationController::memorySystem() {
    return frontend_.memorySystem();
}
    
/**
 * Returns the simulator frontend.
 * 
 * @return A reference to the simulator frontend.
 */
SimulatorFrontend&
TTASimulationController::frontend() {
    return frontend_;
}

/**
 * Returns true in case simulation cannot be finished automatically.
 *
 * In order for this method to return false, it means that while initializing
 * the SimulationController, a *probable* ending point in the program was 
 * detected and it is possible that when running the simulation it is possible 
 * to finish it automatically at that position. If this method returns true
 * it is *impossible* to finish simulation automatically.
 *
 * @return True if it's not possible to end simulation automatically.
 */
bool
TTASimulationController::automaticFinishImpossible() const {
    return automaticFinishImpossible_;
}

/**
 * Initializes the variables that are used in programEnded() to evaluate 
 * whether the simulated program has simulated to its end.
 *
 * @param program The simulated program.
 * @param machine The simulated machine.
 */
std::set<InstructionAddress>
TTASimulationController::findProgramExitPoints(
    const TTAProgram::Program& program,
    const TTAMachine::Machine& machine) const {
    std::set<InstructionAddress> exitPoints;          
          
    /* Set return points to be all the returns from the first executed
       procedure. When control returns from that (usually the crt0(),
       start(), or main()), we should stop simulation. This is for
       convenience of simulating unmodified benchmark programs without 
       having infinite loops etc. */
          
    // find the entry procedure
    Address entryAddr = program.entryAddress();
    Procedure* entryProc = NULL;
    for(int i = 0; i < program.procedureCount(); i++) {
        Procedure &currProc = program.procedure(i);
        
        if (currProc.startAddress().location() <= entryAddr.location() &&
            currProc.endAddress().location() > entryAddr.location()) {
            entryProc = &currProc;
            break;
        }          
    }
   
    // If __exit procedure exists, the first instruction in it is set
    // as an exit point.
    for(int i = 0; i < program.procedureCount(); i++) {
        Procedure &currProc = program.procedure(i);
        if (currProc.name() == "_exit" || currProc.name() == "__exit") {
            exitPoints.insert(currProc.firstInstruction().address().location());
            automaticFinishImpossible_ = false;
       }
    }

    if (entryProc == NULL)
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "The entry point of the program does not point to a procedure.");
        
    const int delaySlots = machine.controlUnit()->delaySlots();
    // check instructions of entry procedure if they are "ra ->jump.1"
    for (InstructionAddress i = entryProc->startAddress().location(); 
         i < entryProc->endAddress().location(); i++) {
        
        const Instruction& currInstr = program.instructionAt(i);

        // check if the instruction has a return move
        for (int m = 0; m < currInstr.moveCount(); ++m) {

            const Move& currMove = currInstr.move(m);

            if (currMove.isReturn()) {

                // set an exit point at the return + delay slots to allow
                // executing the delay slot code of the final return
                if (i + machine.controlUnit()->delaySlots() <=
                    entryProc->endAddress().location()) {
                    exitPoints.insert(i + delaySlots);
                    
                    automaticFinishImpossible_ = false;
                }
                break; // check the next instruction
            }
        }
    }

    /*  In case the last instruction of the first procedure is *not*
        a jump and it's the last procedure in the program, set it as an exit 
        point too (after executing the instruction we should stop simulation
        because there's nothing sensible to execute next). This is to allow
        simulating some obscure assembler programs that do not loop
        forever but just fall through the first procedure after done.

        The detection in that case is done by comparing the PC+1 to
        firstIllegalInstructionIndex_. */
    
    if (program.procedureCount() == 1) {
        // such assembly programs are usually stored in one procedure
        automaticFinishImpossible_ = false;
    }
    firstIllegalInstructionIndex_ = 
        program.lastInstruction().address().location() + 1;
    
    return exitPoints;
}
