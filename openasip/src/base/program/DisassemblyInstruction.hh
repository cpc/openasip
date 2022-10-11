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
 * @file DisassemblyInstruction.hh
 *
 * Declaration of DisassemblyInstruction class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_INSTRUCTION_HH
#define TTA_DISASSEMBLY_INSTRUCTION_HH

#include <string>
#include <vector>
#include "DisassemblyInstructionSlot.hh"
#include "DisassemblyImmediateAssignment.hh"
#include "BaseType.hh"
#include "Exception.hh"

/**
 * Represents an instruction in the disassembler.
 */
class DisassemblyInstruction {
public:
    DisassemblyInstruction();
    ~DisassemblyInstruction();

    Word moveCount() const;
    void addMove(DisassemblyInstructionSlot* move);
    DisassemblyInstructionSlot& move(Word index) const;

    Word longImmediateCount() const;
    void addLongImmediate(DisassemblyImmediateAssignment* longImm);
    DisassemblyImmediateAssignment& longImmediate(Word index) const;

    std::string toString() const;

    DisassemblyAnnotation& annotation(int index) const;
    void addAnnotation(DisassemblyAnnotation* annotation);
    int annotationCount() const;

private:
    /// Vector of DisassemblyMoves.
    typedef std::vector<DisassemblyInstructionSlot*> MoveTable;

    /// Vector of DisassemblyMoves.
    typedef std::vector<DisassemblyImmediateAssignment*> LongImmediateTable;

    /// Copying not allowed.
    DisassemblyInstruction(const DisassemblyInstruction&);
    /// Assignment not allowed.
    DisassemblyInstruction operator=(const DisassemblyInstruction&);

    /// List of instruction moves.
    MoveTable moves_;

    /// List of instruction long immediates
    LongImmediateTable longImmediates_;

    /// Annotationes of instruction itself
    std::vector<DisassemblyAnnotation*> annotations_;

};
#endif
