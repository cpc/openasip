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
 * @file BFUnscheduleFromBody.cc
 *
 * Definition of BFUnscheduleFromBody class
 *
 * Unschedules a move from the loop body but leaves it to the prolog/epilog.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */


#include "BFUnscheduleFromBody.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"
#include "Bus.hh"
#include "POMDisassembler.hh"
#include "SimpleResourceManager.hh"
#include "Instruction.hh"
#include "MoveNodeDuplicator.hh"
#include "Terminal.hh"
#include "BF2Scheduler.hh"

void BFUnscheduleFromBody::unscheduleOriginal() {
    oldCycle_ = mn_.cycle();
    oldBus_ = &mn_.move().bus();

    if (mn_.isSourceOperation()) {
        srcFU_ = &mn_.move().source().functionUnit();
    }

    if (mn_.isDestinationOperation()) {
        dstFU_ = &mn_.move().destination().functionUnit();
    }

    if (mn_.move().source().isImmediateRegister()) {
        oldImmWriteCycle_ = rm().immediateWriteCycle(mn_);
        immRegIndex_ = mn_.move().source().index();
        immu_ = &mn_.move().source().immediateUnit();
    }

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";

    std::cerr << "\t\tUnscheduling original: " << mn_.toString()
              << " bus: " << oldBus_->name() << std::endl;
#endif
    rm().unassign(mn_);
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    if (!rm().canAssign(oldCycle_, mn_, oldBus_, srcFU_, dstFU_,
                   oldImmWriteCycle_)) {
        for (int i = 0; i < recurseCounter_*2; i++)
            std::cerr << "\t";

        std::cerr << "\t\tCannot assign to old cycle: " << mn_.toString()
                  << " old cycle: " << oldCycle_ << std::endl;
        ddg().writeToDotFile("cannot_assign_old_cycle.dot");
        if (mn_.isSourceOperation()) {
            std::cerr << "\t\twhole src PO: "
                      << mn_.sourceOperation().toString() << std::endl;
        }
        if (mn_.isDestinationOperation()) {
            std::cerr << "\t\twhole dst PO: "
                      << mn_.destinationOperation().toString() << std::endl;
        }
        assert(false);
    }
#endif
}

void BFUnscheduleFromBody::returnOriginal() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    for (int i = 0; i < recurseCounter_*2; i++)
        std::cerr << "\t";

    std::cerr << "\t\treturning original: " <<mn_.toString()<<" current bus: "
              << mn_.move().bus().name() << " old bus: " << oldBus_->name()
              << " cycle: " << oldCycle_ << std::endl;
#endif

#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    if (!rm().canAssign(oldCycle_, mn_, oldBus_, srcFU_, dstFU_,
                        oldImmWriteCycle_, immu_, immRegIndex_)) {
        std::cerr << "Cannot return to old cycle: " << mn_.toString()
                  << " cycle: " << oldCycle_  << std::endl;
        if (mn_.isDestinationOperation()) {
            std::cerr << "dstPO: " << mn_.destinationOperation().toString()
                      << std::endl;
        }
        if (mn_.isSourceOperation()) {
            std::cerr << "srcPO: " << mn_.sourceOperation().toString()
                      << std::endl;
        }
        std::cerr << "Original bus: " << oldBus_->name() << std::endl;
        ddg().writeToDotFile("cannot_return_old_cycle.dot");
        TTAProgram::Instruction* ins = rm().instruction(oldCycle_);
        for (int i = 0; i < ins->moveCount(); i++) {
            std::cerr << "\t" << POMDisassembler::disassemble(ins->move(i))
                      << " bus: " << ins->move(i).bus().name()
                      << std::endl;
        }
        std::cerr << "ins contains " << ins->immediateCount()
                  << " immediates." << std::endl;
        assert(false);
    }
#endif
    rm().assign(oldCycle_, mn_, oldBus_, srcFU_, dstFU_,
	   oldImmWriteCycle_, immu_, immRegIndex_);
}

void BFUnscheduleFromBody::undoOnlyMe() {
#ifdef DEBUG_BUBBLEFISH_SCHEDULER
    std::cerr << "BFUnscheudleMove undo" << std::endl;
#endif
    returnOriginal();
}

bool
BFUnscheduleFromBody::operator()() {
    unscheduleOriginal();
    return true;
}

int BFUnscheduleFromBody::recurseCounter_ = 0;
