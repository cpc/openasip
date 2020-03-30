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
 * @file Instruction.cc
 *
 * Implementation of Instruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <boost/format.hpp>

#include "Instruction.hh"
#include "NullInstruction.hh"
#include "Procedure.hh"
#include "ContainerTools.hh"
#include "Move.hh"
#include "Immediate.hh"
#include "Terminal.hh"
#include "NullInstructionTemplate.hh"
#include "NullProcedure.hh"
#include "POMDisassembler.hh"
#include "Conversion.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Instruction
/////////////////////////////////////////////////////////////////////////////

/**
 * The constructor.
 *
 * The default size of an instruction is 1 MAU.
 */
Instruction::Instruction(
    const TTAMachine::InstructionTemplate& instructionTemplate) :
    parent_(NULL), insTemplate_(&instructionTemplate), 
    positionInProcedure_((InstructionAddress)-1),
    size_(1), hasRegisterAccesses_(false), hasConditionalRegisterAccesses_(false) {
}

/**
 * Constructor.
 *
 * Alternative constructor that takes the instruction size as a parameter.
 *
 * @param size The size of the instruction in MAU's.
 */
Instruction::Instruction(
    int size,
    const TTAMachine::InstructionTemplate& instructionTemplate) :
    parent_(NULL), insTemplate_(&instructionTemplate), 
    positionInProcedure_((InstructionAddress)-1),
    size_(size), hasRegisterAccesses_(false), hasConditionalRegisterAccesses_(false) {
    assert(size == 1 && 
           "Instructions sizes other than 1 not supported in POM at the "
           "moment.");
}

/**
 * The destructor.
 */
Instruction::~Instruction() {
}

/**
 * Return the parent that contains the instruction.
 *
 * @return The parent that contains the instruction.
 * @exception IllegalRegistration If the instruction is not registered
 *                                anywhere.
 */
CodeSnippet&
Instruction::parent() const {
    if (parent_ != NULL) {
        return *parent_;
    } else {
        throw IllegalRegistration(__FILE__, __LINE__, __func__,
				  "Instruction is not registered.");
    }
}

/**
 * Sets the parent procedure of the instruction.
 *
 * @param proc The new parent procedure.
 */
void
Instruction::setParent(CodeSnippet& proc) {
    parent_ = &proc;
}

/**
 * Tells whether the instruction belongs to a procedure.
 *
 * @return True if the instruction belongs to a procedure (it is registered),
 *         false otherwise.
 */
bool
Instruction::isInProcedure() const {
    return (parent_ != NULL && parent_ != &NullProcedure::instance());
}

/**
 * Adds a move to the instruction.
 *
 * The ownership of the move will be passed to the instruction.
 *
 * @param move The move to add.
 */
void
Instruction::addMove(std::shared_ptr<Move> move) {
    if (ContainerTools::containsValue(moves_, move)) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__,
				  "Move is already added.");
    } else {
        if (move->source().isGPR() || move->destination().isGPR()) {
            hasRegisterAccesses_ = true;
            if (!move->isUnconditional())
                hasConditionalRegisterAccesses_ = true;
        }
        moves_.push_back(move);
        move->setParent(*this);
    }
}

/**
 * Returns the number of moves contained in this instruction.
 *
 * @return The number of moves contained in this instruction.
 */
int
Instruction::moveCount() const {
    return moves_.size();
}

/**
 * Return the move at the given index in this instruction.
 *
 * The order of moves is arbitrary, no assumption should be made by
 * clients. Anyways, order of moves in instruction does not change between
 * calls to this method.
 *
 * @param i The index of the move.
 * @return The move at the given index in this instruction.
 * @exception OutOfRange if the given index is negative or greater than
 *                       the number of moves in the instruction.
 */
Move&
Instruction::move(int i) const {
    if (i < 0 || static_cast<unsigned int>(i) >= moves_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__,
			 "No move in instruction for given index: " +
			 Conversion::toString(i));
    } else {
        return *moves_.at(i);
    }
}

/**
 * Return the move at the given index in this instruction.
 *
 * The order of moves is arbitrary, no assumption should be made by
 * clients. Anyways, order of moves in instruction does not change between
 * calls to this method.
 *
 * @param i The index of the move.
 * @return The move at the given index in this instruction.
 * @exception OutOfRange if the given index is negative or greater than
 *                       the number of moves in the instruction.
 */
std::shared_ptr<Move>
Instruction::movePtr(int i) const {
    if (i < 0 || static_cast<unsigned int>(i) >= moves_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__,
			 "No move in instruction for given index: " +
			 Conversion::toString(i));
    } else {
        return moves_.at(i);
    }
}

/**
 * Adds an immediate to the instruction.
 *
 * The ownership of the immediate will be passed to the instruction.
 *
 * @param imm The immediate to add.
 */
void
Instruction::addImmediate(std::shared_ptr<Immediate> imm) {
    if (ContainerTools::containsValue(immediates_, imm)) {
        throw ObjectAlreadyExists(__FILE__, __LINE__, __func__,
				  "Immediate is already added.");
    } else {
        immediates_.push_back(imm);
    }
}

/**
 * Returns the number of immediate registers written by the
 * instruction template of this instruction.
 *
 * @return The number of immediate registers written by the
 *         instruction template of this instruction.
 */
int
Instruction::immediateCount() const {
    return immediates_.size();
}

/**
 * Return the immediate write action at the given index in this
 * instruction.
 *
 * The order of immediates is arbitrary, no assumption should be made
 * by clients.
 *
 * @param i The index of the immediate.
 * @return The immediate write action at the given index in this
 *         instruction.
 * @exception OutOfRange if the index is negative or greater than the
 *                       number of immediates in the instruction.
 */
Immediate&
Instruction::immediate(int i) const {
    if (i < 0 || static_cast<unsigned int>(i) >= immediates_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__,
			 "No immediate in instruction with index: " +
			 Conversion::toString(i));
    } else {
        return *immediates_.at(i);
    }
}

/**
 * Return the immediate write action at the given index in this
 * instruction.
 *
 * The order of immediates is arbitrary, no assumption should be made
 * by clients.
 *
 * @param i The index of the immediate.
 * @return The immediate write action at the given index in this
 *         instruction.
 * @exception OutOfRange if the index is negative or greater than the
 *                       number of immediates in the instruction.
 */
std::shared_ptr<Immediate>
Instruction::immediatePtr(int i) const {
    if (i < 0 || static_cast<unsigned int>(i) >= immediates_.size()) {
        throw OutOfRange(__FILE__, __LINE__, __func__,
			 "No immediate in instruction with index: " +
			 Conversion::toString(i));
    } else {
        return immediates_[i];
    }
}

/**
 * Returns the address of the instruction.
 *
 * @return The address of the instruction.
 * @exception IllegalRegistration if the instruction does not belong
 * to a procedure.
 */
Address
Instruction::address() const {
    if (!isInProcedure()) {
        TCEString msg = "Instruction is not registered in a procedure: ";
        msg += POMDisassembler::disassemble(*this);
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__,
            msg);
    }
    // speed up by caching the Instruction's position in the Procedure
    if (positionInProcedure_ != (InstructionAddress)-1 &&
        positionInProcedure_ < 
        (InstructionAddress)parent().instructionCount() &&
        &parent().instructionAtIndex(positionInProcedure_) == this) {
        // the instruction has not moved in the Procedure, we
        // can compute its address in constant time
        // cannot cache the Address itself because the Procedure might
        // have moved
        Address address(
            parent().startAddress().location() + positionInProcedure_,
            parent().startAddress().space());
        return address;
    } else {
        Address address = parent().address(*this);
        positionInProcedure_ = 
            address.location() - parent().startAddress().location();
        return address;
    }
}

/**
 * Returns the size of the instruction in MAU's.
 *
 * @return The size of the instruction in MAU's.
 */
int
Instruction::size() const {
    return size_;
}

/**
 * Make a complete copy of the instruction.
 *
 * The copy is identical, except that it is not registered to the
 * procedure of the original instruction (and therefore, any address
 * it refers to is not meaningful).
 *
 * @return A complete copy of the instruction.
 */
Instruction*
Instruction::copy() const {
    Instruction* newIns = new Instruction(size_, *insTemplate_);
    for (int i = 0; i < moveCount(); i++) {
        newIns->addMove(move(i).copy());
    }
    for (int i = 0; i < immediateCount(); i++) {
        newIns->addImmediate(immediate(i).copy());
    }
    newIns->hasRegisterAccesses_ = hasRegisterAccesses_;
    newIns->hasConditionalRegisterAccesses_ = hasConditionalRegisterAccesses_;
    newIns->copyAnnotationsFrom(*this);
    return newIns;
}

/**
 * Returns true in case this Instruction contains moves that access registers.
 *
 * This method can be used to optimize register utilization analysis.
 *
 * @return True in case at least one move accesses registers.
 */
bool
Instruction::hasRegisterAccesses() const {
    return hasRegisterAccesses_;
}

/**
 * Returns true in case this Instruction contains moves that access registers
 * and are conditional.
 *
 * This method can be used to optimize register utilization analysis.
 *
 * @return True in case at least one conditional move accesses registers.
 */
bool
Instruction::hasConditionalRegisterAccesses() const {
    return hasConditionalRegisterAccesses_;
}

/**
 * Returns whether this instruction contains moves that are jumps.
 *
 * @return True in case at least one move in this instruction is a jump.
 */
bool
Instruction::hasJump() const {

    for (int i = 0; i < moveCount(); i++ ) {
        if (move(i).isJump()) {
            return true;
        }
    }
    return false;
}

/**
 * Returns whether this instruction contains moves that are calls.
 *
 * @return True in case at least one move in this instruction is a call.
 */
bool
Instruction::hasCall() const {

    for (int i = 0; i < moveCount(); i++ ) {
        if (move(i).isCall()) {
            return true;
        }
    }
    return false;
}

/**
 * Returns whether this instruction contains a procedure return move.
 */
bool
Instruction::hasReturn() const {

    for (int i = 0; i < moveCount(); i++ ) {
        if (move(i).isReturn()) {
            return true;
        }
    }
    return false;
}


/**
 * Returns whether this instruction contains moves that affect the 
 * control flow (branches or calls).
 *
 * @return True in case at least one move in this instruction is a control
 *         flow move.
 */
bool
Instruction::hasControlFlowMove() const {
    return hasJump() || hasCall();
}

/**
 * Sets instruction template.
 *
 * @param insTemp Instruction template for the instruction.
 */
void
Instruction::setInstructionTemplate(
    const TTAMachine::InstructionTemplate& insTemp) {
    insTemplate_= &insTemp;
}


/**
 * Returns the instruction template of instruction.
 *
 * @return The instruction template of instruction.
 */
const TTAMachine::InstructionTemplate&
Instruction::instructionTemplate() const {
    return *insTemplate_;
}

/**
 * Remove move from instruction.
 *
 * Move becomes independent (is not deleted).
 *
 * @param move Move to remove.
 * @exception IllegalRegistration If move doesn't belong to instruction.
 */
void
Instruction::removeMove(Move& move) {
    if (&move.parent() != this) {
        throw IllegalRegistration(__FILE__, __LINE__);
    }

    for (MoveList::iterator iter = moves_.begin();
         iter != moves_.end(); iter++) {
        if ((iter->get()) == &move) {
            moves_.erase(iter);
            break;
        }
    }
    move.setParent(NullInstruction::instance());
}

/**
 * Remove immediate from instruction.
 *
 * Immediate becomes independent (is not deleted).
 *
 * @param immediate Immediate to remove.
 * @exception IllegalRegistration If immediate doesn't belong to instruction.
 */
void
Instruction::removeImmediate(Immediate& imm) {
    for (ImmList::iterator iter = immediates_.begin();
         iter != immediates_.end(); iter++) {
        if ((iter->get()) == &imm) {
            immediates_.erase(iter);
            return;
        }
    }
    throw IllegalRegistration(__FILE__, __LINE__);
}

/**
 * Returns the disassembly of the instruction.
 */
std::string
Instruction::toString() const {
    return POMDisassembler::disassemble(*this);
}

}
