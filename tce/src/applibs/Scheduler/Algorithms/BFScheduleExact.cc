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
 * @file BFScheduleExact.cc
 *
 * Declaration of BFScheduleExact class
 *
 * Tries to schedule a move to an exact cycle (no order).
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFScheduleExact.hh"
#include "BFRegCopyBefore.hh"
#include "BFRegCopyAfter.hh"
#include "SimpleResourceManager.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"
#include "Move.hh"
#include "Machine.hh"
#include "ControlUnit.hh"
#include "TerminalImmediate.hh"
#include "BF2Scheduler.hh"
#include "BFScheduleBU.hh"
#include "MoveGuard.hh"
#include "Guard.hh"

bool
BFScheduleExact::operator()() {

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tScheduling to exact cycle: " << mn_.toString()
              << " to cycle: " << c_ << std::endl;
#endif
    BFRegCopy* regCopyBefore = NULL;
    //BFRegCopy* regCopyAfter = NULL;
    BFRegCopy* regCopy = NULL;

    if (allowRegCopy_ && !canBeScheduled(mn_)) {
        if (mn_.isSourceOperation()) {
            // assumes the result read is wanted to the exact cycle.
            // reg cpy after both splits the move and schedules the copy.
            // TODO: should it be scheudled TD instead of BU then?
            regCopy = new BFRegCopyAfter(sched_,mn_, INT_MAX);
        } else {
            // this only splits the move. scheduled later.
            regCopyBefore = regCopy = new BFRegCopyBefore(sched_,mn_, INT_MAX);
        }

        if (!runPreChild(regCopy)) {
            return false;
        }
    }
    int ddglc = ddg().latestCycle(mn_, ii(), false, false);
    int ddgec = ddg().earliestCycle(mn_, ii(), false, false);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tddg lc: " << ddglc << std::endl;
    std::cerr << "\t\t\tddg ec: " << ddgec << std::endl;
#endif
    if (ddglc >= c_ && ddgec <= c_ && canAssign(c_, mn_, nullptr, srcFU_)) {
        createdCopy_ = assign(c_, mn_, nullptr, srcFU_);
        if (regCopyBefore != NULL) {
            MoveNode* regCopy = regCopyBefore->getRegCopy();
            BFScheduleBU* regCopySched =
                new BFScheduleBU(sched_, *regCopy, c_-1, false, false, false);
            if (!runPostChild(regCopySched)) {
                undo();
                return false;
            }
        }
        return true;
    } else {
        undoAndRemovePreChildren();
        return false;
    }
}

void
BFScheduleExact::undoOnlyMe() {
    unassign(mn_, createdCopy_);
    if (jumpLimitMove_) {
        jumpLimitMove_->setSource(
            new TTAProgram::TerminalImmediate(
                SimValue(oldJumpLimit_, oldJumpLimitWidth_)));
        jumpLimitMove_ = NULL;
    }
}
