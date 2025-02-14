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
 * @file LimmTestSnippetGenerator.hh
 *
 * Declaration of LimmTestSnippetGenerator class.
 *
 * Created on: 2.8.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef LIMMTESTSNIPPETGENERATOR_HH
#define LIMMTESTSNIPPETGENERATOR_HH

#include <cstdint>
#include <vector>
#include <functional>

namespace TTAMachine {
    class Machine;
    class InstructionTemplate;
    class ImmediateUnit;
}

namespace TTAProgram {
    class CodeSnippet;
    class Procedure;
    class Instruction;
}

/*
 * Generator that creates code snippet to load values into IUs specified in
 * the instruction template.
 */
class LimmTestSnippetGenerator {
public:
    LimmTestSnippetGenerator() = delete;
    LimmTestSnippetGenerator(
        const TTAMachine::InstructionTemplate& it);
    virtual ~LimmTestSnippetGenerator();
    /**
     * True, if the object is valid for use. Determined at construction time.
     */
    bool valid() const { return valid_; }

    int inputCount() const;
    int inputBitWidth(int i) const;
    void assignInput(int i, uint64_t val);

    TTAProgram::CodeSnippet* generateSnippet() const;

private:

    /// Indicates if the object is valid to generate snippets.
    bool valid_ = false;

    /// Data used for snippet generation. One entry for each IU in the
    /// instruction template.
    struct targetIU {
        /// The value to be loaded into IU
        uint64_t testInput = 0;
        // The maximum allowed value in bits to be loaded into IU.
        int bitWidth = 0;
        /// The targeted IU
        const TTAMachine::ImmediateUnit& iu;
        /// Lambda function to create instruction that does something to the
        /// loaded value.
        std::function<TTAProgram::Instruction*(int)> sinkFn = nullptr;

        targetIU() = delete;
        targetIU(int bitWidth, const TTAMachine::ImmediateUnit& iu)
            : bitWidth(bitWidth), iu(iu) {}
    };

    std::vector<targetIU> testInputs_;
    const TTAMachine::InstructionTemplate& it_;

    static std::function<TTAProgram::Instruction*(int)> getSinkRoute(
        const TTAMachine::ImmediateUnit& iu, int transportWidth);
    static TTAProgram::Instruction* makeLimmLoadInstruction(
        const std::vector<targetIU>& inputValues,
        const TTAMachine::InstructionTemplate& it);
};

#endif /* LIMMTESTSNIPPETGENERATOR_HH */
