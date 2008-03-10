/**
 * @file InstructionMemory.cc
 *
 * Definition of InstructionMemory class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "InstructionMemory.hh"
#include "ExecutableInstruction.hh"
#include "SequenceTools.hh"
#include "Application.hh"
#include "Conversion.hh"

/**
 * Constructor.
 *
 * This constructor is for constant width instructions only.
 *
 * @param startAddress The starting address of the instruction memory.
 */
InstructionMemory::InstructionMemory(
    InstructionAddress startAddress) : 
    startAddress_(startAddress) {
}

/**
 * Destructor.
 */
InstructionMemory::~InstructionMemory() {
    SequenceTools::deleteAllItems(instructions_);
}

/**
 * Adds new ExecutableInstruction to memory.
 *
 * @param instruction Instruction to be added.
 */
void
InstructionMemory::addExecutableInstruction(
    ExecutableInstruction* instruction) {

    instructions_.push_back(instruction);
}

/**
 * Resets execution counters of all instructions.
 *
 */
void
InstructionMemory::resetExecutionCounts() {
    for (InstructionContainer::iterator i = instructions_.begin();
         i != instructions_.end(); ++i) {
        (*i)->resetExecutionCounts();
    }
}
