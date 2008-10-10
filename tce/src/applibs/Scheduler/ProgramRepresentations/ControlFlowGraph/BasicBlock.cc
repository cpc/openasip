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
 * @file BasicBlock.cc
 *
 * Implementation of graph node (basic block).
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <algorithm>

#include "BasicBlock.hh"
#include "Exception.hh"
#include "Instruction.hh"
#include "POMDisassembler.hh"
#include "NullInstruction.hh"

using namespace TTAProgram;

/**
 * Constructor.
 */
BasicBlock::BasicBlock() : 
    skippedFirstInstructions_(0), 
    statisticsValid_(false) {
}

/**
 * To prevent compiler warning of virtual destructor.
 */
BasicBlock::~BasicBlock() {
}

/**
 * Creates a deep copy of the basic block.
 *
 * @return A deep copy of the basic block.
 */
BasicBlock*
BasicBlock::copy() const {

    BasicBlock* newBB = new BasicBlock();
    if (instructionCount() > 0) {
        for (int i = skippedFirstInstructions(); i < instructionCount(); i++) {
            newBB->add(instructionAtIndex(i).copy());
        }
    }
    return newBB;
}

/**
 * Returns the disassembly of the basic block as string.
 *
 * @return The disassembly of the basic block.
 */
std::string
BasicBlock::disassemble() const {

    std::string content = "";
    const int iCount = instructionCount();
    for (int i = 0; i < iCount; ++i) {
        const TTAProgram::Instruction& instr = instructionAtIndex(i);
        content += POMDisassembler::disassemble(instr);
        content += "\n";
    }
    return content;
}

/**
 * Returns the count of instructions in the beginning of this BB that 
 * should not be copied out from this BB, ie. logically don't belong here
 * but are here because they cannot have been removed without messing
 * some data structures ( like RM bookkeeping ) 
 *
 * @return count of first instructions to skip
 */
int 
BasicBlock::skippedFirstInstructions() const {
    return skippedFirstInstructions_;
}
/**
 * Sets n first instructions of this BB to be skipped instructions, ie.
 * instructions that do not logically belong here but are here because cannot
 * have been removed without messing soem data structures (liek RM bookkeeping)
 *
 * @param count number of instructions to mark as skipped instructions
 */
void BasicBlock::skipFirstInstructions(int count) {
    skippedFirstInstructions_ = count;
}


/**
 * Updates and returns the statistics about Basic Block
 * 
 * @return refrence to structure with information about basic block
 */

const BasicBlockStatistics& 
BasicBlock::statistics() { 
    // hack, statistics should be cached when all the operations that
    // could change the moves in instructions are taken care of.
    statisticsValid_ = false;
    if (statisticsValid_) {
        return statistics_;
    } else {
        statistics_.setMoveCount(0);
        statistics_.setImmediateCount(0);
        statistics_.setInstructionCount(instructionCount());
        statistics_.setBypassedCount(0);
        for (int i = 0; i < instructionCount(); i++) {
            int newMoveCount = 
                instructionAtIndex(i).moveCount() + statistics_.moveCount();
            statistics_.setMoveCount(newMoveCount);
            int newImmCount =
                instructionAtIndex(i).immediateCount() + 
                statistics_.immediateCount(); 
            statistics_.setImmediateCount(newImmCount);                            
            for (int j = 0; 
                j < instructionAtIndex(i).moveCount(); 
                j++) {
                TTAProgram::Move& tempMove = 
                    instructionAtIndex(i).move(j);
                if (tempMove.source().isFUPort() &&
                    tempMove.destination().isFUPort()) {
                    int newBypassCount = statistics_.bypassedCount() + 1;
                    statistics_.setBypassedCount(newBypassCount);
                }
            }            
        }
        statisticsValid_ = true;
        return statistics_;
    }
}

/**
 * Trigger recorded statistics about BB invalid and calls clear of
 * code snippet.
 */
void
BasicBlock::clear() {
    statisticsValid_ = false;
    CodeSnippet::clear();
}
/**
 * Constructor, creates statistics structure with zero content.
 */
BasicBlockStatistics::BasicBlockStatistics() :
    moveCount_(0), 
    immediateCount_(0), 
    instructionCount_(0), 
    bypassedCount_(0) {
}

/**
 * To prevent compiler warning of virtual destructor.
 */
BasicBlockStatistics::~BasicBlockStatistics() {
}

/**
 * Returns move count from statistics object.
 * @return count of moves stored in object
 */
int 
BasicBlockStatistics::moveCount() const {
    return moveCount_;
}
/**
 * Returns immediate count from statistics object.
 * @return count of immediates stored in object
 */
int 
BasicBlockStatistics::immediateCount() const {
    return immediateCount_;
}
/**
 * Returns instruction count from statistics object.
 * @return count of instructions stored in object
 */
int 
BasicBlockStatistics::instructionCount() const {
    return instructionCount_;
}
/**
 * Returns bypassed move count from statistics object.
 * @return count of bypassed moves stored in object
 */
int 
BasicBlockStatistics::bypassedCount() const {
    return bypassedCount_;
}
/**
 * Sets the move count in statistic object.
 * 
 * @param count number of moves to store in object
 */
void 
BasicBlockStatistics::setMoveCount(int count) {
    moveCount_ = count;
}
/**
 * Sets the immediate count in statistic object.
 * 
 * @param count number of immediates to store in object
 */

void 
BasicBlockStatistics::setImmediateCount(int count) {
    immediateCount_ = count;
}
/**
 * Sets the instruction count in statistic object.
 * 
 * @param count number of instructions to store in object
 */

void 
BasicBlockStatistics::setInstructionCount(int count) {
    instructionCount_ = count;
}
/**
 * Sets the bypassed move count in statistic object.
 * 
 * @param count number of bypassed moves to store in object
 */
void 
BasicBlockStatistics::setBypassedCount(int count) {
    bypassedCount_ = count;
}
