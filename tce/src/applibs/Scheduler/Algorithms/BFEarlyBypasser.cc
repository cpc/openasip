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
 * @file BFEarlyBypasser.cc
 *
 * Definition of BFEarlyBypassed class.
 *
 * Before scheduling an operand move, searches for a move which might be a
 * bypass source and then tries class BFEarlyBypass or BFLoopBypass class
 * to perform the actual bypass.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFEarlyBypasser.hh"
#include "BFDREEarly.hh"
#include "MachineConnectivityCheck.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "BF2ScheduleFront.hh"
#include "BFLoopBypass.hh"
#include "BFEarlyBypass.hh"
#include "SchedulerCmdLineOptions.hh"

bool
BFEarlyBypasser::operator()() {

    if (bypassDistance_ == 0) return false;

    DataDependenceEdge* bypassEdge = sched_.findBypassEdge(dst_);
    if (bypassEdge == NULL) {
        return false;
    }
    MoveNode& src = ddg().tailNode(*bypassEdge);

    if (sched_.currentFront()->illegalBypassSources_.count(&src)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "Bypass source illegal: " << src.toString() << std::endl;
#endif
        return 0;
    }

    if (!ddg().guardsAllowBypass(src, dst_, bypassEdge->isBackEdge())) {
        return false;
    }

    if (!MachineConnectivityCheck::canBypass(
            src, dst_, sched_.targetMachine())) {
        return false;
    }

    int bypassSourceLC = bypassSourceLatestCycle(src, dst_);
    if (bypassSourceLC == -1) {
        return false;
    }

    if (bypassEdge->isBackEdge()) {
        if (!src.isScheduled() || src.cycle() < (int)ii()) {
            return false;
        } else {
            return runPreChild(new BFLoopBypass(sched_, src, dst_));
        }
    } else {
        return runPreChild(new BFEarlyBypass(sched_, src, dst_));
    }
}

/**
 * Checks that bypass source does not WHAT??? */
int BFEarlyBypasser::bypassSourceLatestCycle(
    MoveNode& bypassSrc, MoveNode& bypassDst) {
    DataDependenceGraph::NodeSet nodes;

    int lc = INT_MAX;

    if (bypassSrc.isSourceOperation()) {
        ProgramOperation& po = bypassSrc.sourceOperation();
        for (int j = 0; j < po.inputMoveCount(); j++) {
            MoveNode& inputMove = po.inputMove(j);
            // may also be a shared operand
            if (!sched_.isPreLoopSharedOperand(inputMove)) {
                nodes.insert(&inputMove);
            }
        }

        for (int j = 0; j < po.outputMoveCount(); j++) {
            MoveNode& outputMove = po.outputMove(j);
            if (!sched_.isDeadResult(outputMove)) {
                nodes.insert(&outputMove);
            }
        }
    } else {
        nodes.insert(&bypassSrc);
    }

    for (auto mn : nodes) {
        DataDependenceGraph::EdgeSet edges = ddg().outEdges(*mn);
        for (auto e : edges) {
            MoveNode& head = ddg().headNode(*e);
            if (head.isScheduled()) {
                lc = std::min(lc, head.cycle());
                continue;
            }
            if (&head == &bypassDst || head.isInFrontier() ||
                e->isBackEdge() || nodes.count(&head)) {
                continue;
            }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\t\tEdge almost preventing bypass from: "
                      << bypassSrc.toString() << "to: " <<bypassDst.toString()
                      << ": " << e->toString() << " dst: "
                      << head.toString() << std::endl;
#endif
            return -1;
        }
    }
    return lc;
}

BFEarlyBypasser::BFEarlyBypasser(BF2Scheduler& sched, MoveNode& mn) :
    BFOptimization(sched),  dst_(mn) {
    SchedulerCmdLineOptions* opts =
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    if (opts != NULL && opts->bypassDistance() > -1) {
        bypassDistance_ = opts->bypassDistance();
    }
}
