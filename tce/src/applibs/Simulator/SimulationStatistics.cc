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
 * @file SimulationStatistics.hh
 *
 * Implementation of SimulationStatistics class.
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Henry Linjamäki 2017 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "SimulationStatistics.hh"
#include "SimulationStatisticsCalculator.hh"
#include "Program.hh"
#include "NullInstruction.hh"
#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"

/**
 * Constructs a new simulation statistics calculation loop.
 *
 * @param programData The program the statistics are calculated from.
 * @param executionCounts Execution counts of instructions and moves in the
 *                        program.
 */
SimulationStatistics::SimulationStatistics(
    const TTAProgram::Program& programData, 
    const InstructionMemory& executionCounts) :
    program_(programData), executionCounts_(executionCounts) {
}


/**
 * Destructor.
 */
SimulationStatistics::~SimulationStatistics() {
}

/**
 * Adds a new statistics type to be included in simulation statistics
 * calculation.
 *
 * @param statisticsType New statistics type.
 */
void
SimulationStatistics::addStatistics(
    SimulationStatisticsCalculator& statisticsType) {
    statisticsTypes_.push_back(&statisticsType);
}

/**
 * Calculates simulation statistics by going through the program and
 * invoking calculate() for all statistics types.
 */
void 
SimulationStatistics::calculate() {

    if (statisticsTypes_.size() == 0)
        return;

    const TTAProgram::Instruction* currentInstruction = 
        &program_.instructionAt(program_.startAddress().location());
    while (currentInstruction != &TTAProgram::NullInstruction::instance()) {

        // Skip implicit instructions they are processed already (see below).
        if (currentInstruction->size() == 0) {
            currentInstruction = &program_.nextInstruction(*currentInstruction);
            continue;
        }

        auto instrAddr = currentInstruction->address().location();
        const ExecutableInstruction& execInstr = 
            executionCounts_.instructionAtConst(instrAddr);
        if (execInstr.executionCount() == 0) {
            currentInstruction = 
                &program_.nextInstruction(*currentInstruction);
            continue;
        }

        // First process explicit instruction ...
        for (std::size_t i = 0; i < statisticsTypes_.size(); ++i) {
            statisticsTypes_[i]->calculateForInstruction(
                *currentInstruction, execInstr);
        }

        // ... and then the following implicit instructions.
        // FIXME: Potential breakage. Can not know if the executable instruction
        // corresponds to the program instruction. It is now assumed the
        // execution instructions are created and added to executionCounts_ in
        // same order as program instructions currently being traversed.
        const TTAProgram::Instruction* implInstr = &program_.nextInstruction(
            *currentInstruction);
        for (auto& implExecInstr : executionCounts_.implicitInstructionsAt(
            instrAddr)) {
            if (implInstr->size() != 0)
                break;
            for (std::size_t i = 0; i < statisticsTypes_.size(); ++i) {
                statisticsTypes_[i]->calculateForInstruction(
                    *implInstr, *implExecInstr);
            }
            implInstr = &program_.nextInstruction(*implInstr);
        }
        currentInstruction = &program_.nextInstruction(*currentInstruction);
    }
}
