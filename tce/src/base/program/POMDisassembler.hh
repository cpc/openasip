/*
    Copyright (c) 2002-2016 Tampere University.

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
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2016
 * @author Henry Linjamäki 2016-2017
 * @note rating: red
 */

#ifndef TTA_POM_DISASSEMBLER_HH
#define TTA_POM_DISASSEMBLER_HH

#include "BaseType.hh"
#include "Exception.hh"

class DisassemblyInstruction;
class DisassemblyMove;
class DisassemblyGuard;
class DisassemblyElement;
class DisassemblyImmediate;
class DisassemblyImmediateRegister;
class DisassemblyImmediateAssignment;

namespace TTAProgram {
    class Move;
    class Immediate;
    class Instruction;
    class Program;
    class MoveGuard;
    class Procedure;
    class Terminal;
    class DataDefinition;
}

namespace TTAMachine {
    class AddressSpace;
    class Machine;
}

class TCEString;

/**
 * Program object model disassembler.
 */
class POMDisassembler {
public:
    explicit POMDisassembler(bool indices=false);
    explicit POMDisassembler(const TTAProgram::Program& program, bool indices=false);
    virtual ~POMDisassembler();

    virtual DisassemblyInstruction*
    createInstruction(Word instructionIndex) const;

    virtual Word startAddress() const;
    virtual Word instructionCount() const;

    int labelCount(Word address) const;
    std::string label(Word address, int index) const;

    static POMDisassembler* disassembler(
        const TTAMachine::Machine& mach,
        const TTAProgram::Program& program);

    static std::string disassemble(const TTAProgram::Move& move);
    static std::string disassemble(
        const TTAProgram::Instruction& instruction,
        bool indices=false,
        int addr = -1);
    static std::string disassemble(
        const TTAProgram::Procedure& proc, bool indices = false);
    static std::string disassemble(
        const TTAProgram::Program& program, bool indices = false);
    static TCEString disassembleFully(
        const TTAProgram::Program& program, bool indices = false);

    /// "Template methods" that can be overridden in the derived assemblers.
    virtual TCEString disassembleInstruction(
        const TTAProgram::Instruction& instruction,
        int addr = -1);
    virtual TCEString disassembleProcedure(const TTAProgram::Procedure& proc);
    virtual TCEString disassembleProcedures();
    virtual TCEString disassembleProgram();
    virtual TCEString codeSectionDescription(Word startAddress);
    virtual TCEString dataSectionDescription(
        const TTAMachine::AddressSpace& aSpace, Word location);
    virtual TCEString procedureDescription(const TTAProgram::Procedure& proc);
    virtual TCEString destinationLabels(
        const TTAProgram::Procedure& proc, int instrIndex) const;
    virtual TCEString labelPositionDescription(TCEString labelStr) const;
    virtual TCEString dataDefDescription(
        const TTAProgram::DataDefinition& def);
    /// Returns the size of the instruction in memory addresses, if known.
    /// Falls back to address per instruction.
    virtual size_t instructionSize(const TTAProgram::Instruction& /*instruction*/)
        { return 1; }

    static DisassemblyImmediate* createInlineImmediate(
        const TTAProgram::Terminal& terminal, bool signExtend);

    virtual void setPrintAddresses(bool addresses)
        { printAddresses_ = addresses; }

    virtual TCEString printAddress(const TTAProgram::Instruction& instr) const;

protected:
    /// Program object model to disassemble.
    const TTAProgram::Program& program_;
    /// True if instruction indices (addresses) should be printed at the end of lines.
    bool printAddresses_;
private:
    static int labelCount(const TTAProgram::Program& program, Word address);
    static std::string label(
        const TTAProgram::Program&, Word address, int index);

    static DisassemblyInstruction* createInstruction(
        const TTAProgram::Program& program,
        const TTAProgram::Instruction& instruction);
    static DisassemblyMove* createMove(const TTAProgram::Move& move);
    static DisassemblyImmediateAssignment* createImmediateAssignment(
	const  TTAProgram::Immediate& immediate);
    static DisassemblyGuard* createGuard(
        const TTAProgram::MoveGuard& guard);
    static DisassemblyElement* createTerminal(
        const TTAProgram::Terminal& terminal);
    static DisassemblyElement* createFUPort(
        const TTAProgram::Terminal& terminal);
    static DisassemblyElement* createRegister(
        const TTAProgram::Terminal& terminal);
    static DisassemblyImmediateRegister* createImmediateRegister(
        const TTAProgram::Terminal& terminal);

    static bool isCallOrJump(const TTAProgram::Terminal& terminal);
};
#endif
