/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
        UtilizationStats::calculate(*instr, executableInstr);
                
        instr = &program.nextInstruction(*instr);
    }
}
