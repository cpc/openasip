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
