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
 * @file BFShareOperandWithScheduled.cc
 *
 * Definition of BFShareOperandWithScheduled class
 *
 * Performs an operands sharing between two (or more) operations,
 * between scheudling these operations.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */


#include "BFShareOperandWithScheduled.hh"
#include "MoveNode.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "Bus.hh"
#include "MoveNodeDuplicator.hh"
#include "SimpleResourceManager.hh"

bool BFShareOperandWithScheduled::operator()() {
    if (removed_.move().destination().operationIndex() !=
        shared_.move().destination().operationIndex()) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\tDestination operand index mismatch"
                  << std::endl;
#endif
        return false;
    }

    originalRemovedBus_ = &removed_.move().bus();
    originalCycle_ = removed_.cycle();

    if (removed_.move().source().isImmediateRegister()) {
        oldImmWriteCycle_ = rm().immediateWriteCycle(removed_);
        immRegIndex_ = removed_.move().source().index();
        immu_ = &removed_.move().source().immediateUnit();
    }

    MoveNode* removedPrologMN = ii() ?
        duplicator().getMoveNode(removed_) :
        NULL;
    if (removedPrologMN != NULL) {
        prologBus_ = &removedPrologMN->move().bus();

	if (removedPrologMN->move().source().isImmediateRegister()) {
	    prologImmWriteCycle_ =
            prologRM()->immediateWriteCycle(*removedPrologMN);
	}

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\tShareOpWithSched Saving prolog bus: "
                  << prologBus_->name() << std::endl;
        std::cerr << "\t\t\t\t\tProlog move: " << removedPrologMN->toString()
                  << std::endl;
#endif
    }

    unassign(removed_, false);
    if (BFShareOperand::operator()()) {
        ddg().dropNode(removed_);
        sched_.nodeAndCopyKilled(removed_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
        std::cerr << "\t\t\t\t\tShared operand ok!" << std::endl;
#endif
        return true;
    } else {
        sched_.nodeResurrected(removed_);
        ddg().restoreNodeFromParent(removed_);
        assign(originalCycle_, removed_, originalRemovedBus_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
	    std::cerr << "\t\t\t\t\tShare operand failed!" << std::endl;
#endif
        return false;
    }
}

void BFShareOperandWithScheduled::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "\t\t\t\t\tUndoing operandshare: " << shared_.toString()
              << std::endl;
#endif
    BFShareOperand::undoOnlyMe();
    sched_.nodeResurrected(removed_);
    ddg().restoreNodeFromParent(removed_);
    assign(originalCycle_, removed_, originalRemovedBus_, NULL, NULL,
           prologBus_, oldImmWriteCycle_, prologImmWriteCycle_, immu_,
           immRegIndex_);
}
