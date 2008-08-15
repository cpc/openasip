/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file BasicBlockNode.hh
 *
 * Prototype control flow graph of TTA program representation: declaration
 * of graph node (basic block).
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
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
    explicit BasicBlockNode(BasicBlock& bb);
    virtual ~BasicBlockNode();

    bool isBasicBlockNode() const { return true; }
    bool isNormalBB() const;
    bool isEntryBB() const;
    bool isExitBB() const;

    std::string toString() const;

    BasicBlock& basicBlock();
    const BasicBlock& basicBlock() const;

    bool hasOriginalAddress() const;
    InstructionAddress originalStartAddress() const;
    InstructionAddress originalEndAddress() const;

    const BasicBlockStatistics& statistics();
    
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
    BasicBlock* basicBlock_;
    /// true if the BasicBlock is owned by the BasicBlockNode
    bool bbOwned_;
    /// true if this is an entry basic block (not real one)
    bool entry_;
    /// true if this is an exit basic block (not real one)
    bool exit_;    
};

#endif
