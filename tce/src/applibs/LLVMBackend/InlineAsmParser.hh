/*
 * Copyright (c) 2002-2017 Tampere University.
 *
 * This file is part of TTA-Based Codesign Environment (TCE).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
*/
/**
 * @file InlineAsmParser.hh
 *
 * Declaration of inline assembly parser.
 *
 * @author Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef INLINE_ASM_PARSER_HH
#define INLINE_ASM_PARSER_HH

#include <map>
#include <vector>
#include <tuple>

#include "AssemblyParserDiagnostic.hh"

namespace TTAProgram {
    class Program;
    class InstructionReferenceManager;
    class BasicBlock;
}

namespace llvm {
    class TCETargetMachine;
    class Mangler;
    class MachineInstr;
    class MachineOperand;
}

/**
 * The class for parsing TCE assembly listing written as inline assembly in C
 * code.
 *
 * The inline assembly is parsed as parallel code that does not need or should
 * be scheduled.
 *
 */
class InlineAsmParser {
public:
    InlineAsmParser() = delete;
    InlineAsmParser(const InlineAsmParser&) = delete;
    InlineAsmParser& operator=(const InlineAsmParser&) = delete;

    InlineAsmParser(
        const llvm::TCETargetMachine& tm,
        const llvm::Mangler& mangler);

    bool parse(
        const llvm::MachineInstr& inlineAsmMI,
        const std::map<std::string, unsigned>& symbolTable,
        TTAProgram::BasicBlock& bb,
        TTAProgram::InstructionReferenceManager& irm);

    std::string substituteAsmString(
        const llvm::MachineInstr& mi,
        const std::map<std::string, unsigned>& symbolTable,
        const llvm::Mangler& mangler);

    static void addDebugInfoToInlineAsmBB(
        const llvm::MachineInstr& mi,
        TTAProgram::BasicBlock& bb);

    void addLiveRangeData(
        const llvm::MachineInstr& mi,
        TTAProgram::BasicBlock& bb);

    bool sanityChecks(
        const llvm::MachineInstr& mi,
        TTAProgram::BasicBlock& bb) const;

    std::string registerName(const llvm::MachineOperand& mo) const;

    const AssemblyParserDiagnostic& diagnostics() const {
        return parserDiagnostic_;
    }

    // Helper methods
    static bool isInlineAsm(const llvm::MachineInstr& mi);

private:

    void reportError(size_t lineNum, const std::string& errorMsg);
    void reportError(const std::string& errorMsg);

    static void copyInstructions(
        TTAProgram::Program& prog,
        TTAProgram::BasicBlock& targetBB,
        TTAProgram::InstructionReferenceManager& irm);

    /// The target machine parsing context.
    const llvm::TCETargetMachine& tm_;
    /// The symbol name mangler for MIs' symbolic references.
    const llvm::Mangler& mangler_;
    /// The diagnostic object to report parse and compile warnings and
    /// errors to.
    AssemblyParserDiagnostic parserDiagnostic_;
    /// The unique id for "%=" template strings. Each parse() call increases
    /// the count.
    unsigned asmId_ = 0;
};

//#define DEBUG_INLINE_ASM_PARSER

#endif // INLINE_ASM_PARSER_HH
