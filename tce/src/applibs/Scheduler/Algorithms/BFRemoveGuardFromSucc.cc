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
 * @file BFRemoveGuardFromSucc.cc
 *
 * Definition of BFRemoveGuardFromSucc class
 *
 * Tries to remove a guard from a move which is a successor of a move
 * being schedule, to allow the guard write move to be scheduled later.
 *
 * The guard can be removed, if the move has no side effects.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFRemoveGuardFromSucc.hh"
#include "MoveGuard.hh"
#include "Move.hh"
#include "BFScheduleMove.hh"
#include "DataDependenceGraph.hh"
#include "BFRemoveEdge.hh"
#include "MoveNodeDuplicator.hh"

bool BFRemoveGuardFromSucc::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tRemoving guard from succ: " << mn_.toString()
              << std::endl;
#endif
    int cycle = mn_.cycle();
    BFOptimization* unsched = new BFUnscheduleMove(sched_, mn_);
    if (!runPreChild(unsched)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tUnsched failed: " << mn_.toString()
                  << std::endl;
#endif
        return false;
    }

    // Remove the guard edges from the ddg
    auto inEdges = ddg().inEdges(mn_);
    for (auto e: inEdges) {
        if (e->guardUse() && !e->isFalseDep()) {
            runPreChild(new BFRemoveEdge(sched_, ddg().tailNode(*e), mn_,*e));
        }
    }
    auto outEdges = ddg().outEdges(mn_);
    for (auto e: outEdges) {
        if (e->guardUse() &&
            e->dependenceType() == DataDependenceEdge::DEP_WAR) {
            runPreChild(new BFRemoveEdge(sched_, mn_,ddg().headNode(*e), *e));
        }
    }

    // If the prolog move has already been created, update also it
    MoveNode* prologMN = ii() ? duplicator().getMoveNode(mn_) : nullptr;
    if (prologMN != nullptr) {
        prologMN->move().setGuard(NULL);
    }

    guard_ = mn_.move().guard().copy();
    mn_.move().setGuard(NULL);
    BFOptimization* sched = new BFSchedule(sched_, mn_, cycle);
    if (runPostChild(sched)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tRe-scheduling ok: " << mn_.toString()
                  << std::endl;
#endif
        return true;
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tRe-scheduling failed, undoing..: "
                  << mn_.toString() << std::endl;
#endif
        undo();
        return false;
    }
}

void BFRemoveGuardFromSucc::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tRestoring guard to succ: " << mn_.toString()
              << std::endl;
#endif

    // If the prolog move has already been created, update also it
    MoveNode* prologMN = ii() ? duplicator().getMoveNode(mn_) : nullptr;
    if (prologMN != nullptr) {
        prologMN->move().setGuard(guard_->copy());
    }

    mn_.move().setGuard(guard_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\tRestored guard to succ: " << mn_.toString()
              << std::endl;
#endif

}
