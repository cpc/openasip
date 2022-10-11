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
 * @file BFAssignToBody.hh
 *
 * Declaration of BFAssignToBody class.
 *
 * Scheudles a single move only into the main BB or loop body,
 * but not into prolog/epilog.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef TTA_BF_ASSIGN_TO_BODY
#define TTA_BF_ASSIGN_TO_BODY

#include "BFOptimization.hh"

class BFAssignToBody : public BFOptimization {
public:
    BFAssignToBody(BF2Scheduler& sched, MoveNode& mn, int cycle,
                   const TTAMachine::FunctionUnit* srcFU,
                   const TTAMachine::FunctionUnit* dstFU) :
        BFOptimization(sched), mn_(mn), cycle_(cycle), srcFU_(srcFU),
        dstFU_(dstFU) {}
    bool operator()() override;
    void undoOnlyMe() override;
private:
    MoveNode& mn_;
    int cycle_;
    const TTAMachine::FunctionUnit* srcFU_;
    const TTAMachine::FunctionUnit* dstFU_;
};

#endif
