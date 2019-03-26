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
 * @file CompiledSimUtilizationStats.cc
 *
 * Definition of CompiledSimUtilizationStats class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompiledSimUtilizationStats.hh"
#include "Program.hh"
#include "CompiledSimulation.hh"
#include "Instruction.hh"
#include "NullInstruction.hh"
#include "ExecutableInstruction.hh"
#include "ExecutableMove.hh"

using namespace TTAProgram;
using namespace TTAMachine;

/**
 * Default constructor
 */
CompiledSimUtilizationStats::CompiledSimUtilizationStats() {
}

/**
 * Default destructor
 */
CompiledSimUtilizationStats::~CompiledSimUtilizationStats() {
}

void 
CompiledSimUtilizationStats::calculate(
    const TTAProgram::Program& program, 
    const CompiledSimulation& compiledSim) {
        
    // Loop all instructions of a program
    const Instruction* instr = &program.firstInstruction();
    
    int moveNumber = 0;
    
    while (instr != &NullInstruction::instance()) {
        
        ExecutableInstruction executableInstr;

        // Add moves and their execution counts
        for (int i = 0; i < instr->moveCount(); ++i, moveNumber++) {
            ClockCycleCount execCount = compiledSim.moveExecutionCount(
                moveNumber, instr->address().location());
            DummyExecutableMove* move = new DummyExecutableMove(execCount);
            executableInstr.addExecutableMove(move);
        }

        // Calculate execution counts so far
        UtilizationStats::calculateForInstruction(*instr, executableInstr);
                
        instr = &program.nextInstruction(*instr);
    }
}
