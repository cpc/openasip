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
 * @file ImmediateTestGenerator.cc
 *
 * Implementation of ImmediateTestGenerator class.
 *
 * Created on: 9.3.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam.tut.fi)
 * @note rating: red
 */

#include "ImmediateTestGenerator.hh"

#include <assert.h>
#include <vector>

#include "Machine.hh"
#include "Move.hh"
#include "Bus.hh"
#include "ControlUnit.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
#include "Program.hh"
#include "Instruction.hh"
#include "Procedure.hh"
#include "MachineConnectivityCheck.hh"
#include "MathTools.hh"
#include "TemplateSlot.hh"
#include "LimmTestSnippetGenerator.hh"


ImmediateTestGenerator::ImmediateTestGenerator()
    : TestGeneratorBase(
        std::string("immediate-tests"),
        std::string(
            "Generates various short and long immediate values. ")) {
}

ImmediateTestGenerator::~ImmediateTestGenerator() {
}


std::vector<TestCase>
ImmediateTestGenerator::generateTestCasesImpl(
    RandomNumberGenerator::SeedType seed) {
    using namespace TTAMachine;
    using namespace TTAProgram;

    std::vector<TestCase> testcases;
    Program* prog = new Program(*machine().controlUnit()->addressSpace());

    // Generate SIMMs //
    Procedure* simmproc =
        new Procedure("testsimms", *machine().controlUnit()->addressSpace());

    const Machine::BusNavigator& busNav = machine().busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus& bus = *busNav.item(i);
        Terminal* dstTerminal = findAnyWritableTerminal(
            bus, bus.immediateWidth());
        if (dstTerminal == nullptr) {
            continue;
        }
        generateSIMMMoves(*simmproc, bus, *dstTerminal, seed);
    }
    if (simmproc->instructionCount() == 0) {
        delete simmproc;
    } else {
        prog->addProcedure(simmproc);
        testcases.push_back(TestCase(prog, "simm-tests"));
    }

    // Generate LIMMs //
    prog = new Program(*machine().controlUnit()->addressSpace());
    Procedure* limmproc =
        new Procedure("testlimms", *machine().controlUnit()->addressSpace());

    for (auto it : machine().instructionTemplateNavigator()) {
        if (!targetsAnyIU(*it)) {
            continue;
        }
        generateLIMMTestCode(*it, *limmproc, seed);
    }

    if (simmproc->instructionCount() == 0) {
        delete limmproc;
    } else {
        prog->addProcedure(limmproc);
        testcases.push_back(TestCase(prog, "limm-tests"));
    }

    return testcases;
}

/**
 * Seeks any unit port to write an immediate and returns Terminal of it.
 *
 * Otherwise returns NULL.
 */
TTAProgram::Terminal*
ImmediateTestGenerator::findAnyWritableTerminal(
    const TTAMachine::Bus& bus,
    unsigned transportBitWidth) {
    using namespace TTAMachine;
    using namespace TTAProgram;

    const Machine& mach = *bus.machine();
    const RegisterFile* candidateRF = nullptr;
    int candidateRFWidth = 0;

    // Seek writable port among RFs //
    const Machine::RegisterFileNavigator rfNav = mach.registerFileNavigator();
    for (int i = 0; i < rfNav.count(); i++) {
        const RegisterFile& rf = *rfNav.item(i);
        if (rf.maxWrites() < 1 || rf.size() < 1)
            continue;
        if (!MachineConnectivityCheck::busConnectedToRF(bus, rf))
            continue;

        if (rf.width() > candidateRFWidth) {
            candidateRF = &rf;
            candidateRFWidth = rf.width();
        }
        if (rf.width() >= static_cast<int>(transportBitWidth)) {
            break;
        }
    }

    if (candidateRF) {
        return new TerminalRegister(*candidateRF->firstReadPort(), 0);
    }

    // Seek writable port among non-triggering FU ports //
    // todo

    return nullptr;
}


TTAProgram::Terminal*
ImmediateTestGenerator::makeSimmTerminal(const TTAMachine::Bus& bus,
    int value) {

    return new TTAProgram::TerminalImmediate(
        SimValue(value, bus.immediateWidth()));
}


/**
 * Generates test immediate instructions for the given bus.
 *
 * The instructions are appended to the procedure.
 */
void
ImmediateTestGenerator::generateSIMMMoves(
    TTAProgram::Procedure& targetProcedure,
    TTAMachine::Bus& targetBus,
    const TTAProgram::Terminal& targetTerminal,
    RandomNumberGenerator::SeedType seed) {

    using namespace TTAProgram;
    RandomNumberGenerator dice(seed);

    // Prevents transporting values that are wider than anything between
    // immediate and target terminal.
    unsigned transportWidth = static_cast<unsigned>(std::min({
        targetBus.immediateWidth(),
        targetTerminal.port().width(),
        31 }));
    const int transportMask = static_cast<int>(~(~(0ul) << transportWidth));

    targetProcedure.add(makeSIMMInstruction(targetBus, targetTerminal, 1));
    targetProcedure.add(makeSIMMInstruction(targetBus, targetTerminal, 0));
    targetProcedure.add(makeSIMMInstruction(
        targetBus, targetTerminal, -1 % transportMask));
    targetProcedure.add(makeSIMMInstruction(
        targetBus, targetTerminal, 0));
    for (int i = 0; i < 100; i++) {
        targetProcedure.add(makeSIMMInstruction(
            targetBus, targetTerminal, dice() % transportMask));
    }
    targetProcedure.add(makeSIMMInstruction(targetBus, targetTerminal, 0));
}

/**
 * Creates instruction that transport short immediate to targeted terminal.
 */
TTAProgram::Instruction*
ImmediateTestGenerator::makeSIMMInstruction(
    TTAMachine::Bus& targetBus,
    const TTAProgram::Terminal& targetTerminal,
    int value) {

    using namespace TTAMachine;
    using namespace TTAProgram;

    Instruction* inst = new Instruction();
    inst->addMove(std::make_shared<Move>(
        makeSimmTerminal(targetBus, value), targetTerminal.copy(), targetBus));
    return inst;
}

/**
 * Returns true, if the IT writes to any IU.
 */
bool
ImmediateTestGenerator::targetsAnyIU(
    const TTAMachine::InstructionTemplate& it) {
    for (int i = 0; i < it.slotCount(); i++) {
        if (it.slot(i)->destination() != nullptr && it.slot(i)->width() > 0) {
            return true;
        }
    }
    return false;
}

/**
 * Generates Long immediate test code.
 *
 * The instruction template is used to load various values to IUs specified in
 * it. Generated code is appended to the Procedure object.
 *
 */
void
ImmediateTestGenerator::generateLIMMTestCode(
    const TTAMachine::InstructionTemplate& it,
    TTAProgram::Procedure& targetProcedure,
    RandomNumberGenerator::SeedType seed) {

    if (!targetsAnyIU(it)) {
        return;
    }

    LimmTestSnippetGenerator limmTestGen(it);
    if (!limmTestGen.valid()) {
        // Can not generate test code for some reason.
        return;
    }

    // Generate basic LIMMs  //
    for (int64_t val : { 0, 1, 0, -1, 0 } ) {
        for (int i = 0; i < limmTestGen.inputCount(); i++) {
            int64_t mask = -1;
            mask = ~(mask << limmTestGen.inputBitWidth(i));
            limmTestGen.assignInput(i, val & mask);
        }
        targetProcedure.append(limmTestGen.generateSnippet());
    }

    // Generate random LIMMs //
    RandomNumberGenerator rng(seed);
    for (int c = 0; c < 20; c++) {
        for (int i = 0; i < limmTestGen.inputCount(); i++) {
            int64_t mask = -1;
            mask = ~(mask << limmTestGen.inputBitWidth(i));
            limmTestGen.assignInput(i, rng() & mask);
        }
        targetProcedure.append(limmTestGen.generateSnippet());
    }
}


