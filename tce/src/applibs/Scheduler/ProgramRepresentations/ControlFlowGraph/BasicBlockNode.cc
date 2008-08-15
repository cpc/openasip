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
 * @file BasicBlockNode.cc
 *
 * Prototype control flow graph of TTA program representation:
 * implementation of graph node (basic block).
 *
 * @author Andrea Cilio 2005 (cilio@cs.tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma@tut.fi)
 * @note rating: red
 */

#include "BasicBlockNode.hh"
#include "Exception.hh"
#include "BasicBlock.hh"
#include "Conversion.hh"
#include "POMDisassembler.hh"

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
    hasOriginalAddress_(true), basicBlock_(new BasicBlock()), bbOwned_(true),
    entry_(entry), exit_(exit) {

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
BasicBlockNode::BasicBlockNode(BasicBlock& bb) :
    originalStartAddress_(0), originalEndAddress_(0), 
    hasOriginalAddress_(false), basicBlock_(&bb), bbOwned_(false) {
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
BasicBlock&
BasicBlockNode::basicBlock() {
    return *basicBlock_;
}

/**
 * Returns the basic block object this node represents (const version).
 *
 * @return The basic block object (can be modified).
 */
const BasicBlock&
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
const BasicBlockStatistics&
BasicBlockNode::statistics() {
    if (isNormalBB()) {
        return basicBlock_->statistics();
    }
    BasicBlockStatistics* bbs = new BasicBlockStatistics();    
    return *bbs;
}
