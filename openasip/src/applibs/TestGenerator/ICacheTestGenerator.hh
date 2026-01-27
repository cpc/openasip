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
 * @file ICacheTestGenerator.hh
 *
 * Declaration of ICacheTestGenerator class.
 *
 * Created on: 21.9.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef ICACHETESTGENERATOR_HH
#define ICACHETESTGENERATOR_HH

#include <vector>
#include <tuple>

#include "TestGeneratorBase.hh"

#include "BaseType.hh"

namespace TTAProgram {
    class Program;
    class Procedure;
}

namespace TTAMachine {
    class Bus;
    class FUPort;
    class ImmediateUnit;
    class InstructionTemplate;
}

/*
 * Generates test program to stress instruction caches.
 */
class ICacheTestGenerator : public TestGeneratorBase {
public:
    ICacheTestGenerator();
    virtual ~ICacheTestGenerator();

private:
    virtual std::vector<TestCase> generateTestCasesImpl(
        RandomNumberGenerator::SeedType seed =
            RandomNumberGenerator::DEFAULTSEED) override;
    using LIMMSource = std::tuple<
        unsigned, /* The widest unsigned value in bits that can be carried. */
        TTAMachine::Bus*,
        TTAMachine::ImmediateUnit*,
        TTAMachine::InstructionTemplate*>;

    void generateTestProgram(TTAProgram::Program& prog);
    TTAMachine::Bus* selectSIMMSource(TTAMachine::FUPort& forPort);
    LIMMSource selectLIMMSource(TTAMachine::FUPort& forPort);

    void generateJumpTargets(
        std::vector<InstructionAddress>& jumpTargets,
        InstructionAddress startJumpLoc,
        unsigned maxJumpHeight,
        unsigned jumpBlockSize);

    void generateJumpWithSIMM(
        TTAProgram::Procedure& targetProc,
        TTAMachine::Bus& bus,
        const TTAMachine::FUPort& jumpPort,
        InstructionAddress jumpTarget);
    void generateJumpWithLIMM(
        TTAProgram::Procedure& targetProc,
        TTAMachine::Bus& bus,
        const TTAMachine::FUPort& jumpPort,
        InstructionAddress jumpTarget,
        LIMMSource& limmSource);
    static void generateDummyInstructions(
        TTAProgram::Procedure& targetProc,
        unsigned int numOfInstructions = 1);

    RandomNumberGenerator::SeedType seed_ = RandomNumberGenerator::DEFAULTSEED;

};


#endif /* ICACHETESTGENERATOR_HH */
