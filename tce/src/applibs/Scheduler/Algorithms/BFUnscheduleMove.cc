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
 * @file BFUnscheduleMove.cc
 *
 * Definition of BFUnscheduleMove class
 *
 * Unschedules a move.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFUnscheduleMove.hh"
#include "Move.hh"
#include "DataDependenceGraph.hh"
#include "Bus.hh"
#include "POMDisassembler.hh"
#include "SimpleResourceManager.hh"
#include "Instruction.hh"
#include "MoveNodeDuplicator.hh"
#include "Terminal.hh"
#include "BF2Scheduler.hh"

void BFUnscheduleMove::unscheduleOriginal() {
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

    MoveNode* prologMN = ii() ?  duplicator().getMoveNode(mn_) : NULL;
    if (prologMN != NULL) {
        prologBus_ = &prologMN->move().bus();

        if (prologMN->move().source().isImmediateRegister()) {
            prologImmWriteCycle_ = prologRM()->immediateWriteCycle(*prologMN);
        }

    }
    unassign(mn_, false);
}

void BFUnscheduleMove::returnOriginal() {
    assign(oldCycle_, mn_, oldBus_, srcFU_, dstFU_, prologBus_,
	   oldImmWriteCycle_, prologImmWriteCycle_, immu_, immRegIndex_);
}

void BFUnscheduleMove::undoOnlyMe() {
    returnOriginal();
}

bool
BFUnscheduleMove::operator()() {
    unscheduleOriginal();
    return true;
}

int BFUnscheduleMove::recurseCounter_ = 0;
