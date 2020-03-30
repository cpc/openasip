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
 * @file BFShareOperandLate.hh
 *
 * Declaration of BFShareOperandLate class.
 *
 * Performs an operands sharing between two (or more) operations,
 * after already scheduling these operations.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#include "BFShareOperand.hh"
//#include "BFOptimization.hh"

namespace TTAMachine {
    class Bus;
}

class BF2Scheduler;

class BFShareOperandLate : public BFShareOperand {
public:
    BFShareOperandLate(
        BF2Scheduler& sched,
        MoveNode& removed,
        MoveNode& scheduled) :
        BFShareOperand(sched, removed, scheduled), originalCycle_(-1),
        originalRemovedPrologBus_(NULL),
        originalSharedPrologBus_(NULL),
        removedImmWriteCycle_(-1),
        removedPrologImmWriteCycle_(-1),
        removedImmu_(NULL),
        removedImmRegIndex_(-1),
        sharedImmWriteCycle_(-1),
        sharedPrologImmWriteCycle_(-1),
        sharedImmu_(NULL),
        sharedImmRegIndex_(-1) {}
    virtual bool operator()();
    virtual ~BFShareOperandLate() {}
protected:
    virtual void undoOnlyMe();
private:
    void revert(int cycle);
    int originalCycle_;
    const TTAMachine::Bus* originalRemovedBus_;
    const TTAMachine::Bus* originalSharedBus_;
    const TTAMachine::Bus* originalRemovedPrologBus_;
    const TTAMachine::Bus* originalSharedPrologBus_;

    int removedImmWriteCycle_;
    int removedPrologImmWriteCycle_;
    const TTAMachine::ImmediateUnit* removedImmu_;
    int removedImmRegIndex_;

    int sharedImmWriteCycle_;
    int sharedPrologImmWriteCycle_;
    const TTAMachine::ImmediateUnit* sharedImmu_;
    int sharedImmRegIndex_;
};
