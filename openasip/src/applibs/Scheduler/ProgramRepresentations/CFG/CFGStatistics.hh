/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CFGStatistics.hh
 *
 * Declaration of prototype class that collects statistics about
 * control flow graph.
 *
 * @author Vladimir Guzma 2007 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TCE_CFGSTATISTICS_HH
#define TCE_CFGSTATISTICS_HH

#include "BasicBlock.hh"
/**
 * Represents statistics about single control flow graph.
 * Count of BB, sum of moves, immediates, instructions and bypasses as well as
 * largest basic block and immediates, instructions and bypassed in it.
 */

class CFGStatistics : public TTAProgram::BasicBlockStatistics {
public:
    CFGStatistics();
    virtual ~CFGStatistics();
    virtual int normalBBCount() const;
    virtual int maxMoveCount() const;
    virtual int maxImmediateCount() const;
    virtual int maxInstructionCount() const;
    virtual int maxBypassedCount() const;
    virtual void setNormalBBCount(int);
    virtual void setMaxMoveCount(int);
    virtual void setMaxImmediateCount(int);
    virtual void setMaxInstructionCount(int);
    virtual void setMaxBypassedCount(int);

private:    
    int normalBBCount_;
    int maxMoveCount_;
    int maxImmediateCount_;
    int maxInstructionCount_;
    int maxBypassCount_;
};

#endif
