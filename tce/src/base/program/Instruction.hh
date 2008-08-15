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
 * @file Instruction.hh
 *
 * Declaration of Instruction class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_INSTRUCTION_HH
#define TTA_INSTRUCTION_HH

#include <vector>

#include "Address.hh"
#include "Exception.hh"
#include "NullInstructionTemplate.hh"

namespace TTAProgram {

class Procedure;
class Move;
class Immediate;
class CodeSnippet;

/**
 * Represents a TTA instruction.
 */
class Instruction {
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
    virtual ~Instruction();

    CodeSnippet& parent() const throw (IllegalRegistration);
    void setParent(CodeSnippet& proc);
    bool isInProcedure() const;

    void addMove(Move* move) throw (ObjectAlreadyExists);
    int moveCount() const;
    Move& move(int i) const throw (OutOfRange);
    void removeMove(Move& move) throw (IllegalRegistration);

    void addImmediate(Immediate* imm) throw (ObjectAlreadyExists);
    int immediateCount() const;
    Immediate& immediate(int i) const throw (OutOfRange);
    void removeImmediate(Immediate& imm) throw (IllegalRegistration);

    Address address() const throw (IllegalRegistration);

    int size() const;

    bool hasRegisterAccesses() const;
    bool hasConditionalRegisterAccesses() const;
    bool hasJump() const;
    bool hasCall() const;
    bool hasControlFlowMove() const;

    Instruction* copy() const;

    void setInstructionTemplate(
        const TTAMachine::InstructionTemplate& insTemp);
    const TTAMachine::InstructionTemplate& instructionTemplate() const;

private:
    /// List for moves.
    typedef std::vector<Move*> MoveList;
    /// List for immediates.
    typedef std::vector<Immediate*> ImmList;

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
    /// Size of instruction in MAU's.
    int size_;
    /// Set to true in case this instruction has moves that access registers.
    bool hasRegisterAccesses_;
    /// Set to true in case this instruction has moves that access registers
    /// and are conditional.
    bool hasConditionalRegisterAccesses_;
    /// Instruction template that is used for this instruction.
    const TTAMachine::InstructionTemplate* insTemplate_;
};

}

#endif
