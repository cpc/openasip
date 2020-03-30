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
 * @file Instruction.hh
 *
 * Declaration of Instruction class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_HH
#define TTA_INSTRUCTION_HH

#include <vector>
#include <memory>

#include "Address.hh"
#include "Exception.hh"
#include "NullInstructionTemplate.hh"
#include "AnnotatedInstructionElement.hh"

namespace TTAProgram {

class Procedure;
class Move;
class Immediate;
class CodeSnippet;

/**
 * Represents a TTA instruction.
 *
 * @note: The annotations added with the AnnotatedInstructionElement
 * are not saved to a TPEF file when the program is written!
 */
class Instruction : public AnnotatedInstructionElement {
public:
    
#ifdef TCE_PYTHON_BINDINGS

    // Py++ generated code has problems with the default values

    Instruction(const TTAMachine::InstructionTemplate& instructionTemplate);

    explicit Instruction(
        int size, const TTAMachine::InstructionTemplate& instructionTemplate);

#else
    Instruction(const TTAMachine::InstructionTemplate& instructionTemplate =
        TTAMachine::NullInstructionTemplate::instance());


    explicit Instruction(
        int size, const TTAMachine::InstructionTemplate& instructionTemplate =
        TTAMachine::NullInstructionTemplate::instance());
#endif
    ~Instruction();

    CodeSnippet& parent() const;
    void setParent(CodeSnippet& proc);
    bool isInProcedure() const;

    void addMove(std::shared_ptr<Move> move);
    int moveCount() const;
    Move& move(int i) const;
    std::shared_ptr<Move> movePtr(int i) const;
    void removeMove(Move& move);

    bool isNOP() const { return moveCount() == 0 && immediateCount() == 0; }

    void addImmediate(std::shared_ptr<Immediate> imm);
    int immediateCount() const;
    Immediate& immediate(int i) const;
    std::shared_ptr<Immediate> immediatePtr(int i) const;
    void removeImmediate(Immediate& imm);

    Address address() const;

    int size() const;

    bool hasRegisterAccesses() const;
    bool hasConditionalRegisterAccesses() const;
    bool hasJump() const;
    bool hasCall() const;
    bool hasReturn() const;
    bool hasControlFlowMove() const;

    Instruction* copy() const;

    void setInstructionTemplate(
        const TTAMachine::InstructionTemplate& insTemp);
    const TTAMachine::InstructionTemplate& instructionTemplate() const;

    std::string toString() const;

private:
    /// List for moves.
    typedef std::vector<std::shared_ptr<Move> > MoveList;
    /// List for immediates.
    typedef std::vector<std::shared_ptr<Immediate> > ImmList;

    /// Copying not allowed.
    Instruction(const Instruction&);
    /// Assignment not allowed.
    Instruction& operator=(const Instruction&);

    /// Moves contained in this instruction.
    MoveList moves_;
    /// Immediates contained in this instruction.
    ImmList immediates_;
    /// Parent procedure.
    CodeSnippet* parent_;

    /// Instruction template that is used for this instruction.
    const TTAMachine::InstructionTemplate* insTemplate_;

    /// cache the instruction's index in the its procedure for faster address()
    mutable InstructionAddress positionInProcedure_;

    unsigned tripCount_;

    /// Size of instruction in MAU's.
    short size_;
    /// Set to true in case this instruction has moves that access registers.
    bool hasRegisterAccesses_;
    /// Set to true in case this instruction has moves that access registers
    /// and are conditional.
    bool hasConditionalRegisterAccesses_;
};

}

#endif
