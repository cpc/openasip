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
 * @file BFDropPreShared.cc
 *
 * Definition of BFDropPreShared class
 *
 * Removes an operand write which is not needed because the operand is
 * loop-operand shared and put to the prolog of the loop.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFDropPreShared.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "BFSwapOperands.hh"
#include "Operation.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "MoveNodeSet.hh"

bool
BFDropPreShared::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tDropping pre-shared: " << mn_.toString() << std::endl;
#endif
    MoveNode* trigger = getSisterTrigger(mn_, targetMachine());
    if (trigger == &mn_) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Cannot pre-share trigger operand, must swap operands"
                  << std::endl;
#endif
        ProgramOperationPtr destPO = mn_.destinationOperationPtr();
        const Operation& op = destPO->operation();
        int operandCount  = op.numberOfInputs();
        bool swapped = false;
        for (int i = 1 ; i <= operandCount && !swapped; i++) {
            int idx = mn_.move().destination().operationIndex();
            MoveNode& mn = destPO->inputNode(i).at(0);
            if (i != idx && op.canSwap(i, idx) &&
                !sched_.isPreLoopSharedOperand(mn)) {
                // TODO: force the indeces here
                BFSwapOperands* bfswo =
                    new BFSwapOperands(sched_, destPO, mn_, mn);
                if (runPreChild(bfswo)) {
                    swapped = true;
                }
            }
        }
        if (!swapped) {
            return false;
        }
    }
    ddg().dropNode(mn_);
    sched_.nodeKilled(mn_);
    return true;
}

void
BFDropPreShared::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tUndoing dropping of pre-shared: " << mn_.toString()
              << std::endl;
#endif
    ddg().restoreNodeFromParent(mn_);
    sched_.nodeResurrected(mn_);
}
