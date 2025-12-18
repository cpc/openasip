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
 * @file GuardTestGenerator.cc
 *
 * Implementation/Declaration of GuardTestGenerator class.
 *
 * Created on: 5.8.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "GuardTestGenerator.hh"

#include <memory>
#include <set>
#include <algorithm>

#include "SnippetGenerator.hh"

#include "ControlUnit.hh"
#include "Bus.hh"
#include "AddressSpace.hh"
#include "Guard.hh"

#include "Program.hh"
#include "Procedure.hh"
#include "Move.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"
#include "MoveGuard.hh"

#include "MachineConnectivityCheck.hh"

#include "TCEString.hh"

GuardTestGenerator::GuardTestGenerator()
    : TestGeneratorBase("guard-tests", "Generates tests for move guards.") {

}

GuardTestGenerator::~GuardTestGenerator() {
}

std::vector<TestCase>
GuardTestGenerator::generateTestCasesImpl(
    RandomNumberGenerator::SeedType seed) {

    using namespace TTAMachine;
    using namespace TTAProgram;

    RandomNumberGenerator rng(seed + 129594);

    auto& defaultCodeAS = *machine().controlUnit()->addressSpace();
    std::vector<TestCase> result;

    // Generate separate test case for each bus having RF guards //
    for (auto bus : machine().busNavigator()) {
        if (!hasRFGuards(*bus)) continue;

        std::unique_ptr<Program> tcProg(new Program(defaultCodeAS));
        Procedure* tcProc(new Procedure("guardtests", defaultCodeAS));
        tcProg->addProcedure(tcProc);

        std::unique_ptr<CodeSnippet> tcCode(getGuardTests(*bus, rng()));
        if (tcCode) tcProc->append(tcCode.release());

        if (tcProc->instructionCount() == 0) continue;

        result.push_back(TestCase(
            tcProg.release(), TCEString("rfguard-test-on-") + bus->name()));
    }

    // TODO: Generate separate test case for each bus having fu port guards //

    return result;
}

bool
GuardTestGenerator::hasRFGuards(const TTAMachine::Bus& bus) {
    using TTAMachine::RegisterGuard;
    for (int i = 0; i < bus.guardCount(); i++) {
        auto* rfGuard = dynamic_cast<RegisterGuard*>(bus.guard(i));
        if (rfGuard) return true;
    }
    return false;
}

std::set<TTAMachine::RegisterGuard*>
GuardTestGenerator::registerGuards(
        const TTAMachine::Bus& bus) {
    using TTAMachine::RegisterGuard;
    std::set<TTAMachine::RegisterGuard*> result;
    for (int i = 0; i < bus.guardCount(); i++) {
        auto* rfGuard = dynamic_cast<RegisterGuard*>(bus.guard(i));
        if (rfGuard) result.insert(rfGuard);
    }
    return result;
}

TTAProgram::CodeSnippet*
GuardTestGenerator::getGuardTests(
    const TTAMachine::Bus& bus,
    RandomNumberGenerator::SeedType seed) {

    using TTAProgram::CodeSnippet;

    std::unique_ptr<CodeSnippet> tests(new CodeSnippet());

    unsigned globalGuardLatency =
        bus.machine()->controlUnit()->globalGuardLatency();

    for (auto rfGuard : registerGuards(bus)) {
        for (int64_t guardState : generateGuardStates(*rfGuard, seed) ) {
            std::unique_ptr<CodeSnippet> guardStateSet(
                getSetGuardCode(*rfGuard, guardState));

            // Assuming that guard value is loaded in the last instruction in
            // guardStateSet.
            unsigned guardEvaluationCycle = std::max(
                globalGuardLatency +
                    (unsigned)rfGuard->registerFile()->guardLatency(),
                0u);

            std::unique_ptr<CodeSnippet> moveWithGuard(
                getMovementForGuard(*rfGuard, guardEvaluationCycle, false,
                    seed));

            if (guardStateSet && moveWithGuard) {
                tests->append(guardStateSet.release());
                tests->append(moveWithGuard.release());
            }
        }
    }

    return tests.release();
}

std::vector<int64_t>
GuardTestGenerator::generateGuardStates(
    const TTAMachine::RegisterGuard& rfGuard,
    RandomNumberGenerator::SeedType seed) {

    RandomNumberGenerator rng(seed + 11);
    int regWidth = rfGuard.registerFile()->width();

    assert(regWidth > 0);
    if (regWidth == 1) {
        return { 1, 0 };
    } else {
        return {
            1, 0,
            rng() % regWidth, rng() % regWidth,
            rng() % regWidth, rng() % regWidth
        };
    }

    return {};
}

/**
 * Creates code snippet that loads the given value into the register specified
 * by the register guard.
 */
TTAProgram::CodeSnippet*
GuardTestGenerator::getSetGuardCode(
    const TTAMachine::RegisterGuard& rfGuard, int64_t value) {

    using namespace TTAProgram;

    std::unique_ptr<CodeSnippet> code(SnippetGenerator::getValue(value,
        *rfGuard.registerFile(), rfGuard.registerIndex()));

    return code.release();
}

/**
 * Creates code snippet that has some bus movement with the given guard.
 *
 */
TTAProgram::CodeSnippet*
GuardTestGenerator::getMovementForGuard(
    TTAMachine::RegisterGuard& rfGuard, unsigned guardEvalCycle,
    bool /*addNoiseMoves*/, RandomNumberGenerator::SeedType seed) {

    using namespace TTAProgram;

    RandomNumberGenerator rng(seed+92);
    std::unique_ptr<CodeSnippet> tests(new CodeSnippet());


    for (unsigned i = 1; i < guardEvalCycle; i++) {
        auto nop = SnippetGenerator::getNOP(
            *rfGuard.registerFile()->machine());
        if (!nop) return nullptr;
        tests->append(nop);
    }

    // Generate guarded movement on the bus at the guard evaluation //
    auto bus = rfGuard.parentBus();
    if (bus->immediateWidth() < 0) return nullptr;

    Terminal* anyTerm = SnippetGenerator::getAnyWritablePort(*bus);
    if (!anyTerm) return nullptr;

    auto terminalImm = new TerminalImmediate(SimValue(1, 1));
    auto guardedMove = SnippetGenerator::getInstruction(
        new Move(terminalImm, anyTerm, *bus, new MoveGuard(rfGuard)));
    if (!guardedMove) return nullptr;

    tests->add(guardedMove);

    return tests.release();
}
