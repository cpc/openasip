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
 * @file BFLateBypasses.cc
 *
 * Definition of BFLateBypasses class
 *
 * Before scheduling a result move, searches for moves which might be a
 * bypass destinations for this result and then tries to call BFLateBypass
 * class to bypass these.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */
#include "BFLateBypasses.hh"
#include "DataDependenceGraph.hh"
#include "MachineConnectivityCheck.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "BFLateBypass.hh"
#include "BFDRELate.hh"
#include "ResourceManager.hh"
#include "SimpleResourceManager.hh"
#include "Port.hh"
#include "SchedulerCmdLineOptions.hh"
#include "Machine.hh"

#include <map>

BFLateBypasses::BFLateBypasses(BF2Scheduler& sched, MoveNode& src, int lc) :
    BFOptimization(sched), src_(src), lc_(lc), removedSource_(false),
    bypassDistance_(5) {
    SchedulerCmdLineOptions* opts =
        dynamic_cast<SchedulerCmdLineOptions*>(Application::cmdLineOptions());
    if (opts != NULL) {
        if (opts->bypassDistance() > -1) {
            bypassDistance_ = opts->bypassDistance();
        }
    }
}

bool
BFLateBypasses::operator()() {

    int variableDstCount = 0;
    bool bypassed = false;
    auto rrDestinations =
        ddg().onlyRegisterRawDestinationsWithEdges(src_, false);

    // the boolean specifies whether this is normal or guard bypass.
    std::map<MoveNode*, bool, MoveNode::Comparator> bypassDestinations;

    int earliestDst = INT_MAX;
    // first just search all potentials.
    for (auto i : rrDestinations) {
        MoveNode* n = i.second;
        int guardParam = i.first->guardUse() ? 1 : 2;
        // TODO: the rootgraph here prevents over-loop-edge bypass.
        // TODO: enable over-loop-edge bypass when it works.
        // Needs update to ddg.mergeAndKeep() to put backedge
        // properties and BFOptimization::assign() to keep original
        // register in prolog. (create copy MN of original for prolog
        // when bypassing?
        if ((static_cast<DataDependenceGraph*>(ddg().rootGraph()))->
            onlyRegisterRawSource(*n, guardParam) == NULL) {
            continue;
        }

        // not guard but normal read
        if (guardParam == 2) {
            MachineConnectivityCheck::PortSet destinationPorts;
            destinationPorts.insert(&n->move().destination().port());

            if (MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
                    src_, destinationPorts)) {
                if (n->isScheduled() == false
                    && (static_cast<SimpleResourceManager&>(
                            rm()).initiationInterval() != 0)) {
                    // Found node connected by loop edge, ignore it.
                    continue;
                }
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\tFound ok bypass dest: " << n->toString()
                          << std::endl;
#endif

                if (!ddg().guardsAllowBypass(src_, *n)) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "\t\tGuards do not allow bypass to: "
                              << n->toString() << std::endl;
#endif
                    continue;
                }

                assert(n->isScheduled());
                int originalCycle = n->cycle();
                int earliestLimit = originalCycle - bypassDistance_;
                if (lc_ < earliestLimit) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                    std::cerr << "\t\t\tcycle limites prevent late bypass "
                              << std::endl;
#endif
                    continue;
                }
                earliestDst = std::min(earliestDst, originalCycle);
                bypassDestinations.insert(std::make_pair(n, false));
            }
        } else {
            // TODO: test if guard bypass possible.
            // now handled in the guard bypass class itself.
            assert(n->isScheduled());
            int originalCycle = n->cycle();
            int earliestLimit = originalCycle - bypassDistance_;
            if (lc_ < earliestLimit) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
                std::cerr << "\t\t\tcycle limites prevent late bypass "
                          << std::endl;
#endif
                continue;
            }
            earliestDst = std::min(earliestDst, originalCycle);
            bypassDestinations.insert(std::make_pair(n, true));
        }
    }

    // Do the critical one first. This may force the source FU.
    for (auto n : bypassDestinations) {
        if (n.first->cycle() == earliestDst) {
            if (!n.second) {

                // if always write results,
                // op cannot have more than one register target.
                if (targetMachine().alwaysWriteResults() &&
                    src_.isSourceOperation() &&
                    n.first->isDestinationVariable() &&
                    variableDstCount>0) {
                    continue;
                }

                BFLateBypass* lbp =
                    new BFLateBypass(sched_, src_, *n.first, lc_);
                if (runPreChild(lbp)) {
                    bypassed = true;
                    if (n.first->isDestinationVariable()) {
                        variableDstCount++;
                    }
                }
                bypassDestinations.erase(n.first);
                break;
            } else {
                // Guard bypass not yet supported.
                bypassDestinations.erase(n.first);
                break;
            }
        }
    }

    // then actually do the bypass for the other, non-critical moves
    for (auto n : bypassDestinations) {
        if (!n.second) {
            // if always write results,
            // op cannot have more than one register target.
            if (targetMachine().alwaysWriteResults() &&
                src_.isSourceOperation() &&
                n.first->isDestinationVariable() &&
                variableDstCount>0) {
                continue;
            }

            int originalCycle = n.first->cycle();
            if (originalCycle > earliestDst + bypassDistance_) {
                continue;
            }
            BFLateBypass* lbp = new BFLateBypass(sched_, src_, *n.first, lc_);
            if (runPreChild(lbp)) {
                bypassed = true;
                if (n.first->isDestinationVariable()) {
                    variableDstCount++;
                }
            }
        }
    }
    if (!bypassed) {
        return false;
    }

    // if always write results,
    // op must have exactly one register target.
    if (targetMachine().alwaysWriteResults() &&
        src_.isSourceOperation()) {
        // if none of the bypassed are to reg,
        // may not DRE
        if (variableDstCount == 0) {
            return true;
        }
        assert(variableDstCount == 1);
        // if one of the bypasses are to reg,
        // MUST dre.
        BFDRELate* dre = new BFDRELate(sched_, src_);
        if (runPostChild(dre)) {
            removedSource_ = true;
            return true;
        } else {
            undoAndRemovePreChildren();
            return false;
        }
    }

    BFDRELate* dre = new BFDRELate(sched_, src_);
    if (runPostChild(dre)) {
        removedSource_ = true;
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "DRE ok!" << std::endl;
    } else {
        std::cerr << "Could not DRE away: " << src_.toString() << std::endl;
#endif
    }
    return true;
}
