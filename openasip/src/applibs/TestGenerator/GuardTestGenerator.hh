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
 * @file GuardTestGenerator.hh
 *
 * Declaration of GuardTestGenerator class.
 *
 * Created on: 5.8.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef GUARDTESTGENERATOR_HH
#define GUARDTESTGENERATOR_HH

#include "TestGeneratorBase.hh"

#include <set>
#include <vector>

namespace TTAMachine {
    class Bus;
    class RegisterGuard;
}

namespace TTAProgram {
    class CodeSnippet;
}

/*
 * Generates tests for guards.
 */
class GuardTestGenerator : public TestGeneratorBase {
public:
    GuardTestGenerator();
    virtual ~GuardTestGenerator();

protected:
    virtual std::vector<TestCase> generateTestCasesImpl(
        RandomNumberGenerator::SeedType seed =
            RandomNumberGenerator::DEFAULTSEED) override;

private:

    static bool hasRFGuards(const TTAMachine::Bus& bus);
    static std::set<TTAMachine::RegisterGuard*> registerGuards(
        const TTAMachine::Bus& bus);
    static TTAProgram::CodeSnippet* getGuardTests(
        const TTAMachine::Bus& bus,
        RandomNumberGenerator::SeedType seed);
    static std::vector<int64_t> generateGuardStates(
        const TTAMachine::RegisterGuard& rfGuard,
        RandomNumberGenerator::SeedType seed);
    static TTAProgram::CodeSnippet* getSetGuardCode(
        const TTAMachine::RegisterGuard& rfGuard,
        int64_t value);
    static TTAProgram::CodeSnippet* getMovementForGuard(
        TTAMachine::RegisterGuard& rfGuard,
        unsigned guardEvalCycle,
        bool addNoiseMoves,
        RandomNumberGenerator::SeedType seed);
};

#endif /* GUARDTESTGENERATOR_HH */
