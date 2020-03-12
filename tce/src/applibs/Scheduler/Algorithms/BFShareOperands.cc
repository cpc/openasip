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
 * @file BFShareOperands.cc
 *
 * Definition of BFShareOperands class
 *
 * Searches for potential operations to share operand with, and
 * then calls BFShareOperandWithScheduled to fo the actual operand sharing.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFShareOperands.hh"
#include "DataDependenceGraph.hh"
#include "FunctionUnit.hh"
#include "Terminal.hh"
#include "Move.hh"
#include "Operation.hh"
#include "BF2Scheduler.hh"
#include "BF2ScheduleFront.hh"
#include "BFShareOperandWithScheduled.hh"
#include "HWOperation.hh"
#include "FUPort.hh"

bool BFShareOperands::operator()() {
    // TODO: alctually check the guards
    if (!mn_.move().isUnconditional()) {
        return false;
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\tShareOperands operator() called for:"
              << mn_.toString() <<  std::endl;
#endif

    if (sched_.currentFront()->illegalOperandShares_.find(&mn_) !=
        sched_.currentFront()->illegalOperandShares_.end()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\tIllegal operand share: " << mn_.toString()
                  << std::endl;
#endif
        return false;
    }

    bool didSomething = false;
    ProgramOperation& po = mn_.destinationOperation();
    const Operation& op = po.operation();
    const TTAMachine::FunctionUnit* forcedFU = po.scheduledFU();

    DataDependenceGraph::NodeSet siblings = ddg().regDepSiblings(mn_);
    for (auto i : siblings) {
        if (!i->isDestinationOperation()) {
            continue;
        }

        if (i->isScheduled()) {
            if (i->cycle() > maxCycle_) {
                continue;
            }
            // limit operand sharing for reasonable distances
            // TODO: use the cmd line parameter
            int ddglc = ddg().latestCycle(mn_, ii(), false, false);
            if (i->cycle() < ddglc - 5) {
                continue;
            }

            // try share with already scheduled
            const TTAProgram::Terminal& dt = i->move().destination();
            if (dt.isFUPort()) {
                const TTAMachine::FunctionUnit& fu = dt.functionUnit();
                if (!fu.hasOperation(op.name())) {
                    continue;
                }
                const TTAMachine::HWOperation* hwop = fu.operation(op.name());
                const TTAMachine::FUPort *sharedPort =
                    hwop->port(mn_.move().destination().operationIndex());
                if (sharedPort != &dt.port()) {
                    continue;
                }
                if (dt.isTriggering()) {
                    // TODO: is sibling commutative?
                    continue;
                }
                if (forcedFU != NULL) {
                    if (&fu == forcedFU) {
                        MoveNode* trigger =
                            BF2ScheduleFront::getSisterTrigger(
                                mn_, targetMachine());
                        if (trigger == &mn_) {
                            continue;
                        }

                        BFShareOperandWithScheduled* sows =
                            new BFShareOperandWithScheduled(sched_, *i, mn_);
                        if (runPreChild(sows)) {
                            didSomething = true;
                        }
                    }
                } else {
                    if (fu.hasOperation(op.name())) {
                        if (sched_.isTrigger(fu, mn_)) {
                            // TODO: change commutative
                            continue;
                        }

                        BFShareOperandWithScheduled* sows =
                            new BFShareOperandWithScheduled(sched_, *i, mn_);
                        if (runPreChild(sows)) {
                            didSomething = true;
                        }
                    }
                }
            }
        }
    }
    return didSomething;
}

void BFShareOperands::undoOnlyMe() {}

bool BFShareOperands::removedNode() { return false; }
