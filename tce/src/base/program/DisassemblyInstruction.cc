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
 * @file DisassemblyInstruction.cc
 *
 * Implementation of DisassemblyInstruction class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
DisassemblyInstruction::move(Word index) const {
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
DisassemblyInstruction::longImmediate(Word index) const {
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

    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }

    disassembly = disassembly + " ;";
    return disassembly;
}


/**
 * Returns annotation of requested index.
 *
 * @param index Index of annotation to return.
 * @return Annotation of requested index.
 */
DisassemblyAnnotation&
DisassemblyInstruction::annotation(int index) const {
    return *(annotations_.at(index));
}

/**
 * Adds an annotation to instruction.
 *
 * @param annotation Annotation to add.
 */
void
DisassemblyInstruction::addAnnotation(DisassemblyAnnotation* annotation) {
    annotations_.push_back(annotation);
}

/**
 * Returns the number of annotationes stored for the instruction.
 *
 * @return The number of annotationes stored for the instruction.
 */
int
DisassemblyInstruction::annotationCount() const {
    return static_cast<int>(annotations_.size());
}
