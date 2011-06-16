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
 * @file BasicBlockNode.cc
 *
 * Prototype control flow graph of TTA program representation:
 * implementation of graph node (basic block).
 *
 * @author Andrea Cilio 2005 (cilio-no.spam-cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include "BasicBlockNode.hh"
#include "Exception.hh"
#include "BasicBlock.hh"
#include "Conversion.hh"
#include "POMDisassembler.hh"
#include "InstructionReferenceManager.hh"

/**
 * Constructor.
 *
 * @param originalStartAddress The starting address of the basic block in
 * the original program (address of the first instruction).
 * @param originalEndAddress The ending address of the basic block in
 * the original program (address of the last instruction).
 * @param entry True if the basic block is a (pseudo) entry basic block.
 * @param exit True if the basic block is a (pseudo) exit basic block.
 */
BasicBlockNode::BasicBlockNode(
    InstructionAddress originalStartAddress,
    InstructionAddress originalEndAddress,
    bool entry,
    bool exit) :
    originalStartAddress_(originalStartAddress),
    originalEndAddress_(originalEndAddress),
    hasOriginalAddress_(true), 
    basicBlock_(new TTAProgram::BasicBlock(originalStartAddress)), 
    bbOwned_(true),
    entry_(entry), exit_(exit),
    scheduled_(false), refsUpdated_(false), loopScheduled_(false),
    successor_(NULL), predecessor_(NULL) {

    if (entry || exit) {
        hasOriginalAddress_ = false;
    } else {
        if (originalStartAddress > originalEndAddress){
            throw InvalidData(
                __FILE__, __LINE__, __func__, 
                "Basic block start address is higher then it's end address");
        }                                               
    }
}

/**
 * Constructor.
 *
 * A wrapper for BasicBlock. When constructed with this one, the given bb
 * will not be deleted in the destructor.
 */
BasicBlockNode::BasicBlockNode(
    TTAProgram::BasicBlock& bb, bool scheduled, bool refsUpdated, 
    int originalStartAddress, bool loopScheduled) :
    originalStartAddress_(originalStartAddress), originalEndAddress_(0), 
    hasOriginalAddress_(false), basicBlock_(&bb), bbOwned_(false), 
    entry_(false), exit_(false),
    scheduled_(scheduled), refsUpdated_(refsUpdated), 
    loopScheduled_(loopScheduled),
    successor_(NULL), predecessor_(NULL) {
}

/**
 * Destructor.
 */
BasicBlockNode::~BasicBlockNode() {
    if (bbOwned_)
        delete basicBlock_;
    basicBlock_ = NULL;
}

/**
 * Returns the basic block object this node represents.
 *
 * @return The basic block object (can be modified).
 */
TTAProgram::BasicBlock&
BasicBlockNode::basicBlock() {
    return *basicBlock_;
}

/**
 * Returns the basic block object this node represents (const version).
 *
 * @return The basic block object (can be modified).
 */
const TTAProgram::BasicBlock&
BasicBlockNode::basicBlock() const {
    return *basicBlock_;
}

/**
 * Returns true if the original adress of this basic block is known.
 *
 * The basic block might not have original program address in case it's a
 * pseudo basic block, or a completely new basic block which did not exist
 * in the original program.
 *
 * @return True in case original address is known.
 */
bool
BasicBlockNode::hasOriginalAddress() const {
    return hasOriginalAddress_;
}

/**
 * The starting address of the basic block in the original program.
 *
 * Returned value is undefined in case hasOriginalAddress() returns false.
 *
 * @return The original starting address of the basic block.
 */
InstructionAddress
BasicBlockNode::originalStartAddress() const {
    return originalStartAddress_;
}

/**
 * The end address of the basic block in the original program.
 *
 * Returned value is undefined in case hasOriginalAddress() returns false.
 *
 * @return The original ending address of the basic block.
 */
InstructionAddress
BasicBlockNode::originalEndAddress() const {
    return originalEndAddress_;
}

/**
 * Returns the description of basic block as string.
 *
 * @note Used in writting graph to .dot file.
 * @return The description of basic block
 */
std::string
BasicBlockNode::toString() const {

    if (isNormalBB()) {
        std::string content = "";
        int iCount = basicBlock().instructionCount();
        if (iCount > 0) {
            content += Conversion::toString(originalStartAddress_);
            content += " - ";
            content += 
		Conversion::toString(originalStartAddress_ + iCount - 1);
        }
        return content;
    } else if (isEntryBB()) {
        return "Entry";
    } else if (isExitBB()) {
        return "Exit";
    }
    return "";
}



/**
 * Returns true if object is ordinary basic block containing
 * code snippet with instructions.
 *
 * @return True if the basic block is normal storage for instructions.
 */
bool
BasicBlockNode::isNormalBB() const {
    return (!entry_) && (!exit_);
}
/**
 * Returns true if the basic block is representing artificial Entry node.
 *
 * @return True if the basic block is artificially added Entry node.
 */
bool
BasicBlockNode::isEntryBB() const {
    return entry_;
}
/**
 * Return true if basic block is representing artificial Exit node.
 *
 * @return True if basic block is Exit node.
 */
bool
BasicBlockNode::isExitBB() const {
    return exit_;
}

/**
 * Updates and returns the statistics about Basic Block
 * 
 * @return refrence to structure with information about basic block
 */
const TTAProgram::BasicBlockStatistics&
BasicBlockNode::statistics() {
    if (isNormalBB()) {
        return basicBlock_->statistics();
    }
    TTAProgram::BasicBlockStatistics* bbs = 
        new TTAProgram::BasicBlockStatistics();    
    return *bbs;
}

/** 
 * Finds jumps from a BasicBlockNode.
 * 
 * @return second is last jump or NULL if no jumps, first NULL or first jump 
 * if the BB has two jumps
 */
std::pair<TTAProgram::Move*,TTAProgram::Move*> 
BasicBlockNode::findJumps() {
    std::pair<TTAProgram::Move*, TTAProgram::Move*> moves(NULL,NULL);
    for (int i = basicBlock_->instructionCount()-1; i >= 0; i--) {
        TTAProgram::Instruction& ins = basicBlock_->instructionAtIndex(i);
        for (int j = 0; j < ins.moveCount(); j++) {
            TTAProgram::Move& move = ins.move(j);
            if (move.isJump()) {
                if (moves.second == NULL) {
                    moves.second = &move;
                } else {
                    moves.first = &move;
                    return moves;
                    break;
                }
            }
        }
    }
    return moves;
}

/**
 * Updates instruction references to this BB from procedure to cfg
 *
 * @param prog program where instructions are.
 */
void 
BasicBlockNode::updateReferencesFromProcToCfg(TTAProgram::Program& prog) {

    if (refsUpdated_) { 
        return;
    }
    TTAProgram::InstructionReferenceManager& refManager =
        prog.instructionReferenceManager();
    
    if (isNormalBB()) {
        if (basicBlock_->instructionCount() > 0) {
            TTAProgram::Instruction& newIns = basicBlock_->firstInstruction();
            TTAProgram::Instruction& oldIns = prog.instructionAt(
                originalStartAddress_);
            if (refManager.hasReference(oldIns)) {
                refManager.replace(oldIns, newIns);
            }
        }
    }
    refsUpdated_ = true;
}

void BasicBlockNode::link(BasicBlockNode* successor) {
    if (successor != NULL) {
        // make sure no inconsistent links.
        if (successor->predecessor_ != NULL) {
            successor->predecessor_->successor_ = NULL;
        }
        successor->predecessor_ = this;
    }
    successor_ = successor;
}
