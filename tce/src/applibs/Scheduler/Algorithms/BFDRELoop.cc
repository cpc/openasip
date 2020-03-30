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
 * @file BFDRELoop.cc
 *
 * Definition of BFDRELoop class
 *
 * Performs a dead-result elimination of a loop-bypassed move.
 * The move which is eliminated from the loop body is still left
 * in the epilog, to have correct value for the register after
 * the loop.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFDRELoop.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "MoveNodeDuplicator.hh"
#include "SimpleResourceManager.hh"
#include "Machine.hh"

bool BFDRELoop::operator()() {
    assert(sched_.hasEpilog());
    if (!sched_.killDeadResults() || targetMachine().alwaysWriteResults()) {
        return false;
    }

    if (srcCycle_ < ii()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tCannot loop-dre due too early result cycle"
                  << std::endl;
#endif
        return false;
    }

    if (!dreAllowed()) {
        return false;
    }

    // Create a copy to prolog.
    auto a = duplicator().duplicateMoveNode(mn_, true, true);
    prologMN_ = a.first;
    createdPrologCopy_ = a.second;

    int prologCycle = BF2Scheduler::PROLOG_CYCLE_BIAS + srcCycle_;
    if (prologRM()->canAssign(prologCycle, *prologMN_)) {
        prologRM()->assign(prologCycle, *prologMN_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tAssigned loop-bypassed dead result to"
                  << " epilog, cycle:" << prologCycle << std::endl;
#endif
        // TODO: where to add to the epilog DDG?
        createdEdges_ = ddg().copyDepsOver(mn_, true, false);
        ddg().dropNode(mn_);
        sched_.nodeKilled(mn_);
        return true;
    } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tProlog cycle assign fail for loop-dre: "
                  << prologMN_->toString()
                  << " prolog cycle: " << prologCycle << std::endl;
#endif
        if (createdPrologCopy_) {
            duplicator().disposeMoveNode(prologMN_);
        }
        return false;
    }
}

void BFDRELoop::undoOnlyMe() {
    assert(prologRM()!=NULL);
    assert(&mn_ != NULL);
    assert(prologMN_->isScheduled());
    prologRM()->unassign(*prologMN_);
    prologMoves_.erase(&mn_);
    if (createdPrologCopy_) {
        duplicator().disposeMoveNode(prologMN_);
    }


    BFKillNode::undoOnlyMe();
}

bool BFDRELoop::dreAllowed() {
    DataDependenceGraph::EdgeSet edges = ddg().outEdges(mn_);
    for (auto edge: edges) {
        if (edge->edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
            return false;
        }
        if ((edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
             edge->edgeReason() ==DataDependenceEdge::EDGE_RA) &&
            edge->dependenceType() == DataDependenceEdge::DEP_RAW) {
            return false;
        }
    }
    return true;
}
