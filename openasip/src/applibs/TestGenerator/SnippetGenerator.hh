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
/*
 * @file SnippetGenerator.hh
 *
 * Declaration of SnippetGenerator class.
 *
 * Created on: 5.8.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef SNIPPETGENERATOR_HH
#define SNIPPETGENERATOR_HH

#include <cstdint>

namespace TTAMachine {
    class Machine;
    class Bus;
    class RegisterFile;
    class RFPort;
}

namespace TTAProgram {
    class CodeSnippet;
    class Instruction;
    class Move;
    class Terminal;
    class TerminalRegister;
}

/*
 * Helper class to generate various code snippets, instructions, moves and
 * others for test code generation purpose.
 *
 * All public methods returns valid, complete generated objects or nullptr
 * if generation failed. Therefore, return values should always checked for
 * nullptr. Also, generally the public methods do not deal with universal
 * machine part.
 *
 * Todo when implemented and needed: about basic resource management by
 *      resource exclusion object.
 */
class SnippetGenerator {
public:
    SnippetGenerator();
    virtual ~SnippetGenerator();

    static TTAProgram::CodeSnippet* getValue(
        int64_t value, const TTAMachine::RegisterFile&, int registerIndex);

    static TTAProgram::Instruction* getInstruction(
        TTAProgram::Move* m1);

    static TTAProgram::CodeSnippet* getNOP(const TTAMachine::Machine& mach);

    static TTAProgram::Terminal* getAnyWritablePort(
        const TTAMachine::Bus& bus);

    static TTAProgram::TerminalRegister* getAnyWritablePort(
        const TTAMachine::Bus& bus,
        const TTAMachine::RegisterFile& rf,
        int registerIndex);

private:

    static const TTAMachine::Machine* getRealMachine(
        const TTAMachine::RegisterFile& rf);
    static const TTAMachine::Machine* getRealMachine(
        const TTAProgram::Move& move);
    static const TTAMachine::Machine* getRealMachine(
        const TTAMachine::Bus& bus);
    static TTAProgram::CodeSnippet* getCodeSnippet(
        TTAProgram::Instruction* instr);
    static TTAProgram::Move* getUncheckedSimmMove(
        int64_t value,
        TTAMachine::Bus& bus,
        const TTAMachine::RegisterFile&,
        int index);
};

#endif /* SNIPPETGENERATOR_HH */
