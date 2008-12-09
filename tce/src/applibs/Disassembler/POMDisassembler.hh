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
