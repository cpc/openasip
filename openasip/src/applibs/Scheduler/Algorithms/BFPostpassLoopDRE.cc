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
 * @file BFPostpassLoopDRE.cc
 *
 * Definition of BFPostpassLoopDRE class
 *
 * Performs a dead-result elimination of a loop-bypassed move.
 * The move which is eliminated from the loop body is still left
 * in the epilog, to have correct value for the register after
 * the loop.
 *
 * This class is called in the postpass-bypass phase.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFPostpassLoopDRE.hh"

#include "BF2Scheduler.hh"
#include "DataDependenceGraph.hh"

#include "BFPostpassDRE.hh"
#include "BFUnscheduleMove.hh"

#include "MoveNodeDuplicator.hh"
#include "SimpleResourceManager.hh"


bool
BFPostpassLoopDRE::operator()() {
    if (!sched_.killDeadResults()) {
        return false;
    }

    if (!dreAllowed()) {
        return false;
    }

    // Create a copy to prolog.
    prologMN_ = duplicator().getMoveNode(mn_);
    if (prologMN_ == NULL) {
        return false;
    }

    int prologCycle = BF2Scheduler::PROLOG_CYCLE_BIAS + srcCycle_;
    if (prologRM()->canAssign(prologCycle, *prologMN_)) {
        prologRM()->assign(prologCycle, *prologMN_);

        // then remove
        ddg().copyDepsOver(mn_, true, false);
        ddg().dropNode(mn_);
        sched_.nodeKilled(mn_);
        return true;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    } else {
        std::cerr << "\t\t\t\tProlog cycle assign fail for loop-dre: "
                  << prologMN_->toString()
                  << " prolog cycle: " << prologCycle << std::endl;
#endif
    }
    return false;
}

bool BFPostpassLoopDRE::dreAllowed() {
    DataDependenceGraph::EdgeSet edges = ddg().outEdges(mn_);
    for (auto edge: edges) {
        if (edge->edgeReason() == DataDependenceEdge::EDGE_OPERATION) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "DRE not allowed due op edge: "
                      << edge->toString()  << std::endl;
#endif
            return false;
        }
        if ((edge->edgeReason() == DataDependenceEdge::EDGE_REGISTER ||
             edge->edgeReason() ==DataDependenceEdge::EDGE_RA) &&
            edge->dependenceType() == DataDependenceEdge::DEP_RAW) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "DRE not allowed due edge: "
                      << edge->toString()  << std::endl;
#endif
            return false;
        }
    }
    return true;
}

void BFPostpassLoopDRE::undoOnlyMe() {
    ddg().restoreNodeFromParent(mn_);
    sched_.nodeResurrected(mn_);
    prologRM()->unassign(*prologMN_);
}
