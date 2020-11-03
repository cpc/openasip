/*
    Copyright (c) 2016 Tampere University.

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
 * @file OTASimulationController.cc
 *
 * @author Pekka Jääskeläinen 2016 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "OTASimulationController.hh"
#include "Machine.hh"
#include "MachineState.hh"
#include "SimulatorFrontend.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "MemorySystem.hh"
#include "SimulationEventHandler.hh"

OTASimulationController::OTASimulationController(
    SimulatorFrontend& frontend,
    const TTAMachine::Machine& machine,
    const TTAProgram::Program& program) :
    SimulationController (frontend, machine, program, false, false) {
}

OTASimulationController::~OTASimulationController() {
}


void
OTASimulationController::advanceMachineCycle(unsigned pcAdd) {

    const unsigned int core = 0;
    MachineState& machineState = *machineStates_[core];
    //InstructionMemory& imem = *instructionMemories_[core];
    MemorySystem& memorySystem = frontend_.memorySystem(core);
    GCUState& gcu = machineState.gcuState();

    machineState.endClockOfAllFUStates();

    if (!gcu.isIdle()) {
        gcu.endClock();
    }

    memorySystem.advanceClockOfLocalMemories();
    memorySystem.advanceClockOfSharedMemories();
    machineState.advanceClockOfAllFUStates();

    gcu.programCounter() += pcAdd;
    if (!gcu.isIdle())
        gcu.advanceClock();

    machineState.advanceClockOfAllGuardStates();
}

/**
 * Simulates an instruction cycle, both its explicit and implicit instructions.
 */
bool
OTASimulationController::simulateCycle() {

    std::vector<InstructionAddress> lastExecutedInstructions =
        lastExecutedInstruction_;

    bool finished = false;

    int finishedCoreCount = 0;

    const unsigned int core = 0;
    MachineState& machineState = *machineStates_[core];
    InstructionMemory& imem = *instructionMemories_[core];
    GCUState& gcu = machineState.gcuState();
    const InstructionAddress pc = gcu.programCounter();

    try {
        machineState.clearBuses();

        bool exitPoint = false;
        if (imem.hasInstructionAt(pc)) {
            ExecutableInstruction& instruction = imem.instructionAt(pc);
            instruction.execute();

            lastExecutedInstructions[core] = pc;

            advanceMachineCycle(4);

            if (imem.hasImplicitInstructionsAt(pc)) {
                const auto& implInstructions = imem.implicitInstructionsAt(pc);
                for (size_t i = 0; i < implInstructions.size(); ++i) {
                    ExecutableInstruction& implInstruction =
                        *implInstructions.at(i);
                    implInstruction.execute();
                    exitPoint |= implInstruction.isExitPoint();
                    advanceMachineCycle(0);
                }
            }

            exitPoint |= instruction.isExitPoint();
        }

        // check if the instruction was a return point from the program or
        // the next executed instruction would be sequentially over the
        // instruction space (PC+1 would overflow out of the program)
        if (exitPoint ||
            gcu.programCounter() == firstIllegalInstructionIndex_) {
            machineState.setFinished();
            ++finishedCoreCount;
        }
    } catch (const Exception& e) {
        frontend_.reportSimulatedProgramError(
            SimulatorFrontend::RES_FATAL,
            e.errorMessage());
        prepareToStop(SRE_RUNTIME_ERROR);
        return false;
    }

    if (finishedCoreCount > 0)
        finished = true;

    frontend_.eventHandler().handleEvent(SimulationEventHandler::SE_CYCLE_END);

    lastExecutedInstruction_ = lastExecutedInstructions;

    // this is the instruction count in case of OTA
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
