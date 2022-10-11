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
 * @file BFShareOperandLate.cc
 *
 * Definition of BFShareOperandLate class.
 *
 * Performs an operands sharing between two (or more) operations,
 * after already scheduling these operations.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFShareOperandLate.hh"
#include "MoveNode.hh"
#include "DataDependenceGraph.hh"
#include "BF2Scheduler.hh"
#include "Move.hh"

#include "POMDisassembler.hh"
#include "SimpleResourceManager.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Bus.hh"
#include "MoveNodeDuplicator.hh"
#include "Terminal.hh"
#include "BFRescheduleResultClose.hh"

bool BFShareOperandLate::operator()() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "Sharing oeprand late: removing: " << removed_.toString()
              << " shared: " << shared_.toString() << std::endl;
#endif
    originalCycle_ = removed_.cycle();
    originalRemovedBus_ = &removed_.move().bus();
    originalSharedBus_ = &shared_.move().bus();

    if (removed_.move().source().isImmediateRegister()) {
        removedImmWriteCycle_ = rm().immediateWriteCycle(removed_);
        removedImmRegIndex_ = removed_.move().source().index();
        removedImmu_ = &removed_.move().source().immediateUnit();
    }

    if (shared_.move().source().isImmediateRegister()) {
        sharedImmWriteCycle_ = rm().immediateWriteCycle(shared_);
        sharedImmRegIndex_ = shared_.move().source().index();
        sharedImmu_ = &shared_.move().source().immediateUnit();
    }

    MoveNode* removedPrologMN = ii() ?  duplicator().getMoveNode(removed_) : NULL;
    if (removedPrologMN != NULL) {
        originalRemovedPrologBus_ = &removedPrologMN->move().bus();
        if (removedPrologMN->move().source().isImmediateRegister()) {
            removedPrologImmWriteCycle_ =
                prologRM()->immediateWriteCycle(*removedPrologMN);
        }
    }
    MoveNode* sharedPrologMN = ii() ?  duplicator().getMoveNode(shared_) : NULL;
    if (sharedPrologMN != NULL) {
        originalSharedPrologBus_ = &sharedPrologMN->move().bus();
        if (sharedPrologMN->move().source().isImmediateRegister()) {
            sharedPrologImmWriteCycle_ =
                prologRM()->immediateWriteCycle(*sharedPrologMN);
        }
    }

    int cycle = shared_.cycle();
    MoveNode* removedOpTrigger = removed_.destinationOperation().triggeringMove();

    unassign(shared_, false);
    unassign(removed_, false);
    ddg().dropNode(removed_);
    sched_.nodeAndCopyKilled(removed_);
    if (BFShareOperand::operator()()) {
        if (canAssign(cycle, shared_)) {
            assign(cycle, shared_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\t\tShare operand late OK for: "
                      << shared_.toString() << std::endl;
            std::cerr << "\t\t\t\tRescheduling trigger closer:"
                      << removedOpTrigger->toString() << std::endl;
#endif

            auto resched =
                new BFRescheduleResultClose(sched_, *removedOpTrigger, 0);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            if (runPostChild(resched)) {
                std::cerr << "\t\t\tRescheduled trigger closer." << std::endl;
            }
#else
            runPostChild(resched);
#endif
            return true;
        } else {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
            std::cerr << "\t\t\t\tShare operand late FAIL on assign for: "
                      << shared_.toString() << std::endl;
#endif
            BFShareOperand::undoOnlyMe();
            revert(cycle);
            return false;
        }
    } else {
        revert(cycle);
        return false;
    }
}

void BFShareOperandLate::undoOnlyMe() {
    int cycle = shared_.cycle();
    unassign(shared_, false);
    BFShareOperand::undoOnlyMe();
    revert(cycle);
}

void BFShareOperandLate::revert(int cycle) {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    assert(
        canAssign(
            cycle, shared_, originalSharedBus_, NULL, NULL,
            originalSharedPrologBus_,
            sharedImmWriteCycle_, sharedPrologImmWriteCycle_,
            sharedImmu_, sharedImmRegIndex_));

#endif
    assign(cycle, shared_, originalSharedBus_, NULL, NULL,
           originalSharedPrologBus_,
           sharedImmWriteCycle_, sharedPrologImmWriteCycle_,
           sharedImmu_, sharedImmRegIndex_);

    ddg().restoreNodeFromParent(removed_);
    sched_.nodeResurrected(removed_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    if (!canAssign(originalCycle_, removed_, originalRemovedBus_,
                   NULL, NULL, originalRemovedPrologBus_,
                   removedImmWriteCycle_, removedPrologImmWriteCycle_,
                   removedImmu_, removedImmRegIndex_)) {
        std::cerr << "Cannot return to original: " << removed_.toString()
                  << std::endl
                  << "dest PO: " << removed_.destinationOperation().toString()
                  << std::endl
                  << "original cycle: " << originalCycle_ << std::endl;
        TTAProgram::Instruction* ins = rm().instruction(originalCycle_);
        for (int i = 0; i < ins->moveCount(); i++) {
            TTAProgram::Move& m = ins->move(i);
            std::cerr << "\t" << POMDisassembler::disassemble(m)
                      << " bus: " << m.bus().name() << std::endl;
        }
        assert(false);
    }
#endif
    assign(originalCycle_,removed_, originalRemovedBus_,
           NULL, NULL, originalRemovedPrologBus_,
           removedImmWriteCycle_, removedPrologImmWriteCycle_,
           removedImmu_, removedImmRegIndex_);
}
