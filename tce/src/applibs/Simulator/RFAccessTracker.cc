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
 * @file RFAccessTracker.cc
 *
 * Definition of RFAccessTracker class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "RFAccessTracker.hh"
#include "Application.hh"
#include "SimulatorToolbox.hh"
#include "SimulationEventHandler.hh"
#include "SimulatorFrontend.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "Machine.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "MapTools.hh"
#include "Terminal.hh"
#include "hash_map.hh"
#include "NullInstruction.hh"
#include "Program.hh"
#include "boost/tuple/tuple_comparison.hpp"

#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

/**
 * Constructor.
 *
 * @param frontend The SimulationFrontend which is used to access simulation 
 *                 data.
 * @param instructions Used to fetch instruction execution counts.
 */
RFAccessTracker::RFAccessTracker(
    SimulatorFrontend& frontend, 
    const InstructionMemory& instructions) : 
    frontend_(frontend), instructionExecutions_(instructions) {
    frontend.eventHandler().registerListener(
        SimulationEventHandler::SE_CYCLE_END, this);
    frontend.eventHandler().registerListener(
        SimulationEventHandler::SE_SIMULATION_STOPPED, this);
}

/**
 * Destructor.
 */
RFAccessTracker::~RFAccessTracker() {
    frontend_.eventHandler().registerListener(
        SimulationEventHandler::SE_CYCLE_END, this);
    frontend_.eventHandler().unregisterListener(
        SimulationEventHandler::SE_SIMULATION_STOPPED, this);
}

/**
 * Counts the concurrent register file accesses of current clock cycle.
 */
void 
RFAccessTracker::handleEvent(int event) {

    if (event == SimulationEventHandler::SE_SIMULATION_STOPPED) {

        // create a final sum stats which includes the conditional and unconditional
        // executions

        totalAccesses_.clear();       

        const TTAProgram::Program& program = frontend_.program();
        const TTAProgram::Instruction* currentInstruction = 
            &program.instructionAt(program.startAddress().location());
        while (currentInstruction != 
               &TTAProgram::NullInstruction::instance()) {
            
            const ExecutableInstruction& execInstruction = 
                instructionExecutions_.instructionAtConst(
                    currentInstruction->address().location());
            
            if (!currentInstruction->hasRegisterAccesses() ||
                currentInstruction->hasConditionalRegisterAccesses() ||
                execInstruction.executionCount() == 0) {
                currentInstruction = &program.nextInstruction(*currentInstruction);
                continue;
            }           

            accessesInInstruction_.clear();        

            for (int i = 0; i < currentInstruction->moveCount(); ++i) {
                const TTAProgram::Move& move = currentInstruction->move(i);
                if (move.source().isGPR()) {
                    ++accessesInInstruction_[
                        move.source().registerFile().name().c_str()].get<1>();
                }

                if (move.destination().isGPR()) {
                    ++accessesInInstruction_[
                        move.destination().registerFile().name().c_str()].
                        get<0>();
                }
            }

            for (RFAccessIndex::iterator i = accessesInInstruction_.begin();
                 i != accessesInInstruction_.end(); ++i) {

                ConcurrentRFAccess key(
                    (*i).first, (*i).second.get<0>(), (*i).second.get<1>());
                totalAccesses_[key] += execInstruction.executionCount();
            }
                
            currentInstruction = &program.nextInstruction(*currentInstruction);
        }

        // add the access data of the instruction with the conditional accesses
        ConcurrentRFAccessIndex::iterator i = conditionalAccesses_.begin();
        for (; i != conditionalAccesses_.end(); ++i) {
            ConcurrentRFAccess key = (*i).first;
            totalAccesses_[key] += conditionalAccesses_[key];
        }

    } else if (event == SimulationEventHandler::SE_CYCLE_END) {

        const InstructionAddress& address = 
            frontend_.lastExecutedInstruction();
        const TTAProgram::Instruction& instruction = 
            frontend_.program().instructionAt(address);
        if (!instruction.hasConditionalRegisterAccesses())
            return;

        accessesInInstruction_.clear();

        const ExecutableInstruction& execInstruction = 
            instructionExecutions_.instructionAtConst(address);

        for (int i = 0; i < instruction.moveCount(); ++i) {
            if (execInstruction.moveSquashed(i))
                continue;
            const TTAProgram::Move& move = instruction.move(i);
            if (move.source().isGPR()) {
                ++accessesInInstruction_[
                    move.source().registerFile().name().c_str()].get<1>();
            }

            if (move.destination().isGPR()) {
                ++accessesInInstruction_[
                    move.destination().registerFile().name().c_str()].get<0>();
            }
        }

        for (RFAccessIndex::iterator i = accessesInInstruction_.begin();
             i != accessesInInstruction_.end(); ++i) {
            ++conditionalAccesses_[boost::make_tuple(
                                       (*i).first, (*i).second.get<0>(), 
                                       (*i).second.get<1>())];
        }
    } else {
        abortWithError("RFAccessTracker received an unknown event.");
    }
}

/**
 * Returns the count of clock cycles in which the given register file was
 * written and read concurrently the given times.
 *
 * @param rfName Name of the register file.
 * @param concurrentWrites Count of concurrent writes.
 * @param concurrentReads Count of concurrent reads.
 * @return The count of cycles in which the given count of concurrent accesses
 *         was made to the given register file.
 */
ClockCycleCount 
RFAccessTracker::concurrentRegisterFileAccessCount(
    const std::string& rfName,
    std::size_t concurrentWrites,
    std::size_t concurrentReads) const {
    try {
        return MapTools::valueForKey<ClockCycleCount>(
            totalAccesses_, boost::make_tuple(
                rfName, concurrentWrites, concurrentReads));
    } catch (const Exception& e) {
        return 0;
    }
}

/**
 * Returns the access data base.
 *
 * In case statistics are not up-to-date, finalizes the calculation before
 * returning the index.
 *
 * @return The access data base.
 */
const RFAccessTracker::ConcurrentRFAccessIndex&
RFAccessTracker::accessDataBase() const {
    return totalAccesses_;
}

