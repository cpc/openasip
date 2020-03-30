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
 * @file BFRegCopy.hh
 *
 * Declaration of BFRegCopy class
 *
 * Base class for creating a register-to-register copy.
 *
 * @author Heikki Kultala 2014-2020(heikki.kultala-no.spam-tuni.fi)
 * @note rating: red
 */

#ifndef BF_REGCOPY_HH
#define BF_REGCOPY_HH

#include "BFOptimization.hh"

class MoveNode;
class BasicBlockNode;
namespace TTAMachine {
    class RegisterFile;
}

class BFRegCopy: public BFOptimization {
public:
    BFRegCopy(BF2Scheduler& sched, MoveNode& mn, int lc) :
        BFOptimization(sched), mn_(mn), lc_(lc) {}
    bool operator()();
    MoveNode* getRegCopy() { return regCopy_; }
    void undoOnlyMe();
    void undoDDG();
    void createAntidepsForReg(
        MoveNode& firstMove,
        MoveNode& lastMove,
        const TTAMachine::RegisterFile& rf,
        int index,
        TCEString regName,
        BasicBlockNode& bbn,
        bool loopScheduling);

protected:
    virtual bool splitMove(BasicBlockNode& bbn) = 0;
    virtual void undoSplit() = 0;

    MoveNode* createRegCopy(bool after);
    MoveNode& mn_;
    MoveNode* regCopy_;
    int lc_;
};

#endif
