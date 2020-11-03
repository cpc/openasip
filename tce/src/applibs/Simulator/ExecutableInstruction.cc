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
 * @file ExecutableInstruction.cc
 *
 * Definition of ExecutableInstruction class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ExecutableInstruction.hh"
#include "ExecutableMove.hh"
#include "LongImmUpdateAction.hh"
#include "SequenceTools.hh"

/**
 * Constructor.
 */
ExecutableInstruction::ExecutableInstruction() :
    exitPoint_(false) {
    resetExecutionCounts();
}

/**
 * Destructor.
 */
ExecutableInstruction::~ExecutableInstruction() {
    SequenceTools::deleteAllItems(moves_);
    SequenceTools::deleteAllItems(updateActions_);
}

/**
 * Adds move to the instruction.
 *
 * Moves must be added in the same order they are in source Program model's
 * instruction.
 *
 * @param move Move to be added.
 */
void
ExecutableInstruction::addExecutableMove(ExecutableMove* move) {
    moves_.push_back(move);
}

/**
 * Adds long immediate update action.
 *
 * @param action Action to be added.
 */
void
ExecutableInstruction::addLongImmediateUpdateAction(
    LongImmUpdateAction* action) {
    updateActions_.push_back(action);
}

/**
 * Returns the count of times this instruction has been executed so far.
 *
 * @return The execution count of this instruction.
 */
ClockCycleCount 
ExecutableInstruction::executionCount() const {
    return executionCount_;
}

/**
 * Resets the execution counters of this instruction and its moves.
 */
void
ExecutableInstruction::resetExecutionCounts() {
    executionCount_ = 0;
    for (std::size_t i = 0; i < moves_.size(); ++i) {
        moves_[i]->resetExecutionCount();
    }
}

/**
 * Returns the count of full executions so far of move at given index.
 *
 * Squashed moves are not included in the count. Thus, to get count of 
 * times a move has be squashed, one should get the instruction execution 
 * count and subsract the move's execution count from it.
 *
 * @return Count of executions of move at given move slot.
 */
ClockCycleCount 
ExecutableInstruction::moveExecutionCount(std::size_t moveIndex) const {
    return moves_[moveIndex]->executionCount();
}
