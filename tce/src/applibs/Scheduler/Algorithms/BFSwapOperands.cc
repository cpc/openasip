/*
    Copyright (c) 2002-2014 Tampere University.

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
 * @file BFSwapOperands.cc
 *
 * Definition of BFSwapOperands class
 *
 * Swaps the operands of a commutative operation.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFSwapOperands.hh"
#include "ProgramOperation.hh"
#include "BasicBlockScheduler.hh"
#include "Operation.hh"
#include "MoveNodeSet.hh"
#include "Move.hh"
#include "BF2Scheduler.hh"
#include "Terminal.hh"

BFSwapOperands::BFSwapOperands(BF2Scheduler& sched, ProgramOperationPtr po,
                                   MoveNode& mn1, MoveNode& mn2):
    BFOptimization(sched), switched_(&mn1, &mn2), poPtr_(po),
    idx1_(mn1.move().destination().operationIndex()),
    idx2_(mn2.move().destination().operationIndex()) {}


/**
 * Tries to switch operand and trigger so that it makees immediates triggers,
 * but minimizes the critical path if both operands come from variables.
 */
BFSwapOperands::BFSwapOperands(BF2Scheduler& sched, MoveNode& trigger) :
    BFOptimization(sched), switched_(NULL,NULL), poPtr_(),
    idx1_(0), idx2_(0) {

    if (trigger.destinationOperationCount() != 1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "not exactly one destop:" << trigger.toString()
                  << std::endl;
#endif
        return;
    }

    // keep immediates as triggers
    if (trigger.move().source().isImmediate()) {
        return;
    }

    int index = trigger.move().destination().operationIndex();
    ProgramOperationPtr poPtr = trigger.destinationOperationPtr(0);
    const Operation &op = poPtr->operation();
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "trying to swap operand of po: "
              << poPtr->toString() << std::endl;
#endif

    for (int i = 1; i <= op.numberOfInputs(); i++) {
        if (i == index) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tNot swapping with itself" << std::endl;
#endif
            continue;
        }
        if (!op.canSwap(i, index)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tnot commutative" << std::endl;
#endif
            continue;
        }

        MoveNodeSet& otherInputs = poPtr->inputNode(i);
        if (otherInputs.count() != 1) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tother input count not 1" << std::endl;
#endif
            continue;
        }

        MoveNode& other = otherInputs.at(0);
        if (other.isScheduled()) {
            continue;
        }

        if (sched_.isPreLoopSharedOperand(other)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tCannot swap operand with pre-loop-shared" << std::endl;
#endif
            continue;
        }


        // make longer path OR immediate into trigger
        // currently only immediate.
        if (other.move().source().isImmediate()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\tshould swap indeces: " << i << " and  " << index
                      << std::endl;
#endif
            idx1_ = index;
            idx2_ = i;
            poPtr_ = poPtr;
            switched_.first = &trigger;
            switched_.second = &other;
            return;
        }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\tother input is: " << other.toString() << std::endl;
#endif
    }
    return;
}

bool BFSwapOperands::operator()() {
    if (poPtr_ == NULL) {
        return false;
    }
    const Operation &op = poPtr_->operation();
    if (!op.canSwap(idx1_, idx2_)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Cannot swap: " << idx1_ << " and " << idx2_ << std::endl;
#endif
        return false;
    }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Swapping inputs: " << poPtr_->toString() << std::endl;
#endif
    poPtr_->switchInputs(idx1_, idx2_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Swapped inputs: " << poPtr_->toString() << std::endl;
#endif
    return true;
}

void BFSwapOperands::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "undoing swap: " << poPtr_->toString() << std::endl;
#endif
    poPtr_->switchInputs(idx1_, idx2_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "undid swap: " << poPtr_->toString() << std::endl;
#endif
}
