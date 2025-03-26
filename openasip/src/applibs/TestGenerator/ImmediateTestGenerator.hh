/*
 Copyright (c) 2002-2015 Tampere University.

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
 * @file ImmediateTestGenerator.hh
 *
 * Declaration of ImmediateTestGenerator class.
 *
 * Created on: 9.3.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam.tut.fi)
 * @note rating: red
 */

#ifndef IMMEDIATETESTGENERATOR_HH
#define IMMEDIATETESTGENERATOR_HH

#include "TestGeneratorBase.hh"
#include "RandomNumberGenerator.hh"

namespace TTAProgram {
    class Terminal;
    class Procedure;
    class Instruction;
}

namespace TTAMachine {
    class Bus;
    class ImmediateUnit;
    class InstructionTemplate;
}

/*
 * The test generator for generating short and long immediates.
 */
class ImmediateTestGenerator: public TestGeneratorBase {
public:
    ImmediateTestGenerator();
    virtual ~ImmediateTestGenerator();

protected:
    virtual std::vector<TestCase> generateTestCasesImpl(
        RandomNumberGenerator::SeedType seed);

private:
    static TTAProgram::Terminal* findAnyWritableTerminal(
        const TTAMachine::Bus& bus,
        unsigned transportBitWidth);
    static TTAProgram::Terminal* makeSimmTerminal(
        const TTAMachine::Bus& bus, int value);
    static void generateSIMMMoves(
        TTAProgram::Procedure& targetProcedure,
        TTAMachine::Bus& targetBus,
        const TTAProgram::Terminal& targetTerminal,
        RandomNumberGenerator::SeedType seed);
    static TTAProgram::Instruction* makeSIMMInstruction(
        TTAMachine::Bus& targetBus,
        const TTAProgram::Terminal& targetTerminal,
        int value);
    static bool targetsAnyIU(const TTAMachine::InstructionTemplate& it);
    static void generateLIMMTestCode(
        const TTAMachine::InstructionTemplate& it,
        TTAProgram::Procedure& targetProcedure,
        RandomNumberGenerator::SeedType seed);
};

#endif /* IMMEDIATETESTGENERATOR_HH */
