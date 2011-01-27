/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file BasicBlock.hh
 *
 * Declaration of the BasicBlock class.
 *
 * @author Pekka J‰‰skel‰inen 2007-2010
 * @note rating: red
 */

#ifndef TTA_BASIC_BLOCK_HH
#define TTA_BASIC_BLOCK_HH


#include <vector>
#include <set>
#include <map>


#include "TCEString.hh"
#include "Exception.hh"
#include "CodeSnippet.hh"

namespace TTAProgram {
    class Instruction;
    class CodeSnippet;
}

/**
 * Represents statistics about single basic block.
 */
class BasicBlockStatistics {
public:
    BasicBlockStatistics();
    virtual ~BasicBlockStatistics();
    virtual int moveCount() const;
    virtual int immediateCount() const;
    virtual int instructionCount() const;
    virtual int bypassedCount() const;
    virtual void setMoveCount(int);
    virtual void setImmediateCount(int);
    virtual void setInstructionCount(int);
    virtual void setBypassedCount(int);
private:
    int moveCount_;
    int immediateCount_;
    int instructionCount_;
    int bypassedCount_; 
};

/**
 * Represents contents and information of one basic block.
 *
 * A basic block is defined as an ordered sequence of adjacent instructions
 * in which only the first can be target of jump and only the last one (save
 * jump delay slots) can contain jumps.
 *
 * @note Instructions that contain function calls are control flow barriers
 * also. For now they split basic blocks into two, with special type of edge
 * connecting them.
 */
class BasicBlock : public TTAProgram::CodeSnippet {
public:

    BasicBlock(int startAddress);
    virtual ~BasicBlock();

    BasicBlock* copy() const;
    virtual void clear();
    
    int skippedFirstInstructions() const;
    void skipFirstInstructions(int count);
    const BasicBlockStatistics& statistics();

    /// returns true in case the BB is *known* to be inside an inner loop
    bool isInInnerLoop() const { return innerLoop_; }
    void setInInnerLoop(bool inner = true) { innerLoop_ = inner; }

    /// in case the BB is inside a loop and trip count is known,
    /// returns it, otherwise returns 0
    unsigned tripCount() const { return tripCount_; }
    void setTripCount(unsigned count) { tripCount_ = count; }

private:

    int skippedFirstInstructions_;
    bool statisticsValid_;
    BasicBlockStatistics statistics_;    

    bool innerLoop_;
    unsigned tripCount_;
};

#endif

