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
#include "NullAddressSpace.hh"
#include "Move.hh"
#include "Terminal.hh"

using namespace TTAProgram;

/**
 * Constructor.
 */
BasicBlock::BasicBlock(int startAddress) : 
    CodeSnippet(Address(startAddress, 
                        TTAMachine::NullAddressSpace::instance())), 
    liveRangeData_(NULL),
    skippedFirstInstructions_(0), 
    statisticsValid_(false), innerLoop_(false), tripCount_(0) {
}

/**
 * To prevent compiler warning of virtual destructor.
 */
BasicBlock::~BasicBlock() {
}

/**
 * Creates a deep copy of the basic block.
 */
BasicBlock*
BasicBlock::copy() const {

    BasicBlock* newBB = new BasicBlock(start_.location());
    if (instructionCount() > 0) {
        for (int i = skippedFirstInstructions(); i < instructionCount(); i++) {
            newBB->add(instructionAtIndex(i).copy());
        }
    }
    return newBB;
}


/**
 * Returns the count of instructions in the beginning of this BB that 
 * should not be copied out from this BB, ie. logically don't belong here
 * but are here because they cannot have been removed without messing
 * some data structures (like RM bookkeeping) 
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

