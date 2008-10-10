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
 * @file PomDisassembler.hh
 *
 * Declaration of POMDisassembler class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_POM_DISASSEMBLER_HH
#define TTA_POM_DISASSEMBLER_HH

#include "Disassembler.hh"
#include "Program.hh"
#include "BaseType.hh"
#include "Terminal.hh"
#include "MoveGuard.hh"
#include "Move.hh"
#include "Exception.hh"
#include "Immediate.hh"

class DisassemblyInstruction;
class DisassemblyMove;
class DisassemblyGuard;
class DisassemblyElement;
class DisassemblyImmediate;
class DisassemblyImmediateRegister;
class DisassemblyImmediateAssignment;

/**
 * Program object model disassembler.
 */
class POMDisassembler : public Disassembler {
public:
    explicit POMDisassembler(const TTAProgram::Program& program);
    virtual ~POMDisassembler();

    virtual DisassemblyInstruction*
    createInstruction(Word instructionIndex) const;

    virtual Word startAddress() const;
    virtual Word instructionCount() const;

    int labelCount(Word address) const;
    std::string label(Word address, int index) const
        throw (OutOfRange);

    static std::string disassemble(const TTAProgram::Move& move);
    static std::string disassemble(
        const TTAProgram::Instruction& instruction, bool indices=false);  
    static std::string disassemble(
        const TTAProgram::Procedure& proc, bool indices=false)
        throw (Exception);    
    static std::string disassemble(
        const TTAProgram::Program& program, bool indices=false)
        throw (Exception);

private:
    static int labelCount(const TTAProgram::Program& program, Word address);
    static std::string label(
        const TTAProgram::Program&, Word address, int index)
        throw (OutOfRange);

    static DisassemblyInstruction* createInstruction(
        const TTAProgram::Program& program,
        const TTAProgram::Instruction& instruction);
    static DisassemblyMove* createMove(const TTAProgram::Move& move);
    static DisassemblyImmediateAssignment* createImmediateAssignment(
	const  TTAProgram::Immediate& immediate);
    static DisassemblyGuard* createGuard(
        const TTAProgram::MoveGuard& guard);
    static DisassemblyElement* createTerminal(
        const TTAProgram::Terminal& terminal) 
        throw(WrongSubclass, InstanceNotFound);
    static DisassemblyElement* createFUPort(
        const TTAProgram::Terminal& terminal) throw(InstanceNotFound);
    static DisassemblyElement* createRegister(
        const TTAProgram::Terminal& terminal) throw(WrongSubclass);
    static DisassemblyImmediate* createInlineImmediate(
        const TTAProgram::Terminal& terminal);
    static DisassemblyImmediateRegister* createImmediateRegister(
        const TTAProgram::Terminal& terminal);

    static bool isCallOrJump(const TTAProgram::Terminal& terminal);

    /// Program object model to disassemble.
    const TTAProgram::Program& program_;
};
#endif
