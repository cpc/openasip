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
ExecutableInstruction::ExecutableInstruction() : exitPoint_(false) {
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
