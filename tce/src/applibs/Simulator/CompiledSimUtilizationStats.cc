/**
 * @file CompiledSimUtilizationStats.cc
 *
 * Definition of CompiledSimUtilizationStats class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
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
