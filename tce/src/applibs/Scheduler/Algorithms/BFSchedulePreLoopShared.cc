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
 * @file BFSchedulePreLoopShared.cc
 *
 * Definition of BFSchedulePreLoopShared class.
 *
 * Schedules an loop-operand-shared move in the prolog of the loop.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFSchedulePreLoopShared.hh"
#include "SimpleResourceManager.hh"
#include "MoveNodeDuplicator.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "Move.hh"

bool BFSchedulePreLoopShared::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tScheduling pre-loop-shared operand: "
              << originalMN_.toString()
              << std::endl;
#endif
    auto a = duplicator().duplicateMoveNode(originalMN_, true, true);
    prologMN_ = a.first;

    // These should always be unconditional. Operand moves have
    // no side effects, and nobody else uses the FU,
    // and the value of the guard might change between the iterations
    if (!prologMN_->move().isUnconditional()) {
        prologMN_->move().setGuard(NULL);
    }
    if (prologMN_ == NULL) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\tMissing prolog MN for the move. " << std::endl;
#endif
        return false;
    }
    int ddgLC = prologDDG()->latestCycle(*prologMN_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\tddg lc: " << ddgLC << std::endl;
#endif
    if (ddgLC < 0) {
        duplicator().disposeMoveNode(prologMN_);
        return false;
    }
    int lastCycleOfProlog = rm().initiationInterval()-1+
        BF2Scheduler::PROLOG_CYCLE_BIAS;
    int rmlc = prologRM()->latestCycle(
        std::min(ddgLC, lastCycleOfProlog), *prologMN_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\trmlc: " << rmlc << std::endl;
#endif
    if (rmlc < 0) {
        duplicator().disposeMoveNode(prologMN_);
        return false;
    }
    prologRM()->assign(rmlc, *prologMN_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "scheduling of pre-loop opshare ok!"
              << prologMN_->toString() << std::endl;
#endif
    return true;
}

void BFSchedulePreLoopShared::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "undoing scheudle of pre-loop opschare: "
              << prologMN_->toString() << std::endl;
#endif
    prologRM()->unassign(*prologMN_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "unassigned scheudle of pre-loop opschare: "
              << prologMN_->toString() << std::endl;
#endif

    duplicator().disposeMoveNode(prologMN_);
}
