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
 * @file BFShareOperandWithScheduled.hh
 *
 * Declaration of BFShareOperandWithScheduled class
 *
 * Performs an operands sharing between two (or more) operations,
 * between scheudling these operations.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFShareOperand.hh"

namespace TTAMachine {
    class Bus;
    class ImmediateUnit;
}

class BFShareOperandWithScheduled : public BFShareOperand {
public:
    BFShareOperandWithScheduled(BF2Scheduler& sched, MoveNode& removed,
				MoveNode& scheduled):
        BFShareOperand(sched, removed, scheduled), originalCycle_(-1),
        originalRemovedBus_(NULL), prologBus_(NULL), oldImmWriteCycle_(-1),
        prologImmWriteCycle_(-1), immu_(NULL), immRegIndex_(-1)
	{}
    virtual bool operator()();
    virtual ~BFShareOperandWithScheduled() {}
protected:
    virtual void undoOnlyMe();
    int originalCycle_;
    const TTAMachine::Bus* originalRemovedBus_;
    const TTAMachine::Bus* prologBus_;

    int oldImmWriteCycle_;
    int prologImmWriteCycle_;
    const TTAMachine::ImmediateUnit* immu_;
    int immRegIndex_;
};
