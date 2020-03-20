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
 * @file BFLateBypass.hh
 *
 * Declaration of BFLateBypass class
 *
 * Performs a bypass of noy-yet-scheduled result into already scheduled
 * use of the value.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_LATE_BYPASS_HH
#define BF_LATE_BYPASS_HH

#include "BFOptimization.hh"

class MoveNode;
namespace TTAMachine {
    class Bus;
}

class BFLateBypass : public BFOptimization {
public:
    BFLateBypass(BF2Scheduler& sched, MoveNode& src, MoveNode& dst, int lc) :
        BFOptimization(sched),
        src_(src), dst_(dst), lc_(lc), prologBus_(NULL) {}

    virtual bool operator()();
    virtual void undoOnlyMe();
protected:
    int originalCycle_;
    MoveNode& src_;
    MoveNode& dst_;
    int lc_;
    const TTAMachine::Bus* originalBus_;
    const TTAMachine::Bus* prologBus_;
};

#endif
