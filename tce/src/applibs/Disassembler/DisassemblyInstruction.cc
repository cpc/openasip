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
 * @file DisassemblyInstruction.cc
 *
 * Implementation of DisassemblyInstruction class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyInstruction.hh"
#include "SequenceTools.hh"

/**
 * Constructor.
 */
DisassemblyInstruction::DisassemblyInstruction() {
}


/**
 * Destructor.
 */
DisassemblyInstruction::~DisassemblyInstruction() {
    // Delete all moves of the instruction.
   SequenceTools::deleteAllItems(moves_);
}


/**
 * Adds a move to the instruction.
 *
 * @param move Move to be added.
 */
void
DisassemblyInstruction::addMove(DisassemblyInstructionSlot* move) {
    moves_.push_back(move);
}


/**
 * Returns number of moves in the instruction.
 *
 * @return Instruction move count.
 */
Word
DisassemblyInstruction::moveCount() const {
    return moves_.size();
}


/**
 * Returns the isntruction move at a given index.
 *
 * @param index Index of the move.
 * @return The move at the index.
 * @exception OutOfRange If the give index is out of range.
 */
DisassemblyInstructionSlot&
DisassemblyInstruction::move(Word index) const
    throw (OutOfRange) {

    if (index >= moveCount()) {
	std::string procName = "DisassemblyInstruction::move";
	throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return *moves_[index];
}

/**
 * Adds a long immediate to the instruction.
 *
 * @param longImm Immediate to be added.
 */
void
DisassemblyInstruction::addLongImmediate(
    DisassemblyImmediateAssignment* longImm) {

    longImmediates_.push_back(longImm);
}


/**
 * Returns number of long immediates in the instruction.
 *
 * @return Instruction's long immediate count.
 */
Word
DisassemblyInstruction::longImmediateCount() const {
    return longImmediates_.size();
}


/**
 * Returns the long immediate at a given index.
 *
 * @param index Index of the long immediate.
 * @return The long immediate at the index.
 * @exception OutOfRange If the give index is out of range.
 */
DisassemblyImmediateAssignment&
DisassemblyInstruction::longImmediate(Word index) const
    throw (OutOfRange) {

    if (index >= longImmediateCount()) {
        throw OutOfRange(__FILE__, __LINE__, __func__);
    }
    return *longImmediates_[index];
}


/**
 * Returns disassembly of the instruction.
 *
 * @return Disassembly of the instruction as a string.
 */
std::string
DisassemblyInstruction::toString() const {
    std::string disassembly;
    for (Word i = 0; i < moveCount(); i++) {
        if (i > 0) {
            disassembly = disassembly + ", ";
        }
        disassembly = disassembly + move(i).toString();
    }

    for (Word i = 0; i < longImmediateCount(); i++) {

        disassembly = disassembly + " " + longImmediate(i).toString();
    }

    disassembly = disassembly + " ;";
    return disassembly;
}
