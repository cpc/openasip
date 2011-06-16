/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file BasicBlockNode.hh
 *
 * Prototype control flow graph of TTA program representation: declaration
 * of graph node (basic block).
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASIC_BLOCK_NODE_HH
#define TTA_BASIC_BLOCK_NODE_HH

#include <string>

#include "GraphNode.hh"
#include "BasicBlock.hh"
#include "Instruction.hh"

namespace TTAProgram {
    class Instruction;
    class CodeSnippet;
    class BasicBlock;
    class BasicBlockStatistics;
}


/**
 * Node of the control flow graph. Each node represents one basic block.
 *
 * A basic block is defined as an ordered sequence of adjacent instructions
 * in which only the first can be target of jump and only the last one (save
 * jump delay slots) can contain jumps.
 *
 * @note Instructions that contain function calls are control flow barriers
 * also. For now they split basic blocks into two, with special type of edge
 * connecting them.
 */
class BasicBlockNode : public GraphNode {
public:
    BasicBlockNode(
        InstructionAddress originalStartAddress,
        InstructionAddress originalEndAddress,
        bool entry = false,
        bool exit = false);
    explicit BasicBlockNode(
        TTAProgram::BasicBlock& bb, bool scheduled = false, 
        bool refsUpdated = false,
        int originalStartAddress = 0, bool loopScheduled = false);
    virtual ~BasicBlockNode();

    bool isBasicBlockNode() const { return true; }
    bool isNormalBB() const;
    bool isEntryBB() const;
    bool isExitBB() const;

    std::string toString() const;

    TTAProgram::BasicBlock& basicBlock();
    const TTAProgram::BasicBlock& basicBlock() const;

    bool hasOriginalAddress() const;
    InstructionAddress originalStartAddress() const;
    InstructionAddress originalEndAddress() const;

    const TTAProgram::BasicBlockStatistics& statistics();
    
    bool isScheduled() const { return scheduled_; }
    void setScheduled(bool state=true) { scheduled_ = state;}

    std::pair<TTAProgram::Move*,TTAProgram::Move*> findJumps();

    void updateReferencesFromProcToCfg(TTAProgram::Program& prog);

    bool isLoopScheduled() const { return loopScheduled_; }
    void setLoopScheduled() { loopScheduled_ = true; }
    void setBBOwnership(bool ownership = true) { bbOwned_ = ownership; }

    // Ordering of basic blocks in a procedure.
    // These are NULL if not set/decided.
    BasicBlockNode* successor() { return successor_; }
    BasicBlockNode* predecessor() { return predecessor_; }

    void link(BasicBlockNode* succ);
private:
    /// start address of the original basic block, used for reconstructing
    /// the original program after modifying the CFG and its nodes
    InstructionAddress originalStartAddress_;
    /// end address of the original basic block, used for reconstructing
    /// the original program after modifying the CFG and its nodes
    InstructionAddress originalEndAddress_;
    /// not all basic blocks have original addresses (completely new
    /// basic blocks, etc.), this flag is true in case it does
    bool hasOriginalAddress_;
    /// the actual payload data of the graph node (the basic block)
    TTAProgram::BasicBlock* basicBlock_;
    /// true if the BasicBlock is owned by the BasicBlockNode
    bool bbOwned_;
    /// true if this is an entry basic block (not real one)
    bool entry_;
    /// true if this is an exit basic block (not real one)
    bool exit_;    

    bool scheduled_;
    bool refsUpdated_;
    // if this bb was scheduled with loop scheduler
    bool loopScheduled_; 

    BasicBlockNode* successor_;
    BasicBlockNode* predecessor_;

};

#endif
