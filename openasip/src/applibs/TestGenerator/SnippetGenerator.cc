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
 * @file SnippetGenerator.cc
 *
 * Implementation of SnippetGenerator class.
 *
 * Created on: 5.8.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "SnippetGenerator.hh"

#include <memory>

#include "Machine.hh"
#include "RegisterFile.hh"
#include "Socket.hh"
#include "RFPort.hh"
#include "InstructionTemplate.hh"
#include "UniversalMachine.hh"

#include "CodeSnippet.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "TerminalFUPort.hh"
#include "TerminalRegister.hh"
#include "TerminalImmediate.hh"

#include "MachineInfo.hh"
#include "MachineConnectivityCheck.hh"

#include "MathTools.hh"
#include "SimValue.hh"

using namespace TTAMachine;
using namespace TTAProgram;

SnippetGenerator::SnippetGenerator() {
}

SnippetGenerator::~SnippetGenerator() {
}

/**
 * Stores exactly the given value into the register.
 *
 * - No bits of the values are lost.
 * - No machine resources are reserved/used after last instruction*1
 *
 * *1 beside the targeted register.
 */
TTAProgram::CodeSnippet*
SnippetGenerator::getValue(
    int64_t value, const TTAMachine::RegisterFile& rf, int regIdx) {

    // @note: Method implementation is quick and crude.

    using MCC = MachineConnectivityCheck;
    using namespace TTAProgram;

    auto mach = getRealMachine(rf);
    if (!mach) {
        return nullptr;
    }

    int valueBitWidthUnsigned = MathTools::requiredBits(value);
    int valueBitWidthSigned = MathTools::requiredBitsSigned(value);

    std::unique_ptr<CodeSnippet> code;

    // Attempt with SIMM //
    for (auto& bus : mach->busNavigator()) {
        // In non-matching bus-reg width negative values would lose its sign.
        if (bus->width() != rf.width()
            && (bus->width() < valueBitWidthUnsigned
                || rf.width() < valueBitWidthUnsigned)) continue;
        int requiredBitWidth = bus->signExtends()?
            valueBitWidthSigned : valueBitWidthUnsigned;
        if (bus->immediateWidth() < requiredBitWidth) continue;
        if (!MCC::busConnectedToRF(*bus, rf)) continue;

        // Ok, use SIMM
        auto simmInstr = getInstruction(
            getUncheckedSimmMove(value, *bus, rf, regIdx));
        if (simmInstr)
            return getCodeSnippet(simmInstr);
    }

    // Attempt with LIMM //
    // todo

    // Attempt with fabrication (by adding, shifting etc.) //
    // todo

    // Attempt with load from constant pool //
    // todo

    return nullptr; // All attempts failed
}


/**
 * Returns new instruction with the move and suitable instruction template.
 *
 * Acquires ownership of the move regardless of the return result.
 *
 * Returns nullptr if the the instruction can not be created for some reason
 * like:
 * - No instruction template to use.
 */
TTAProgram::Instruction*
SnippetGenerator::getInstruction(TTAProgram::Move* m1) {
    auto mach = getRealMachine(*m1);
    if (!mach) return nullptr;

    using namespace TTAProgram;

    for (auto it : mach->instructionTemplateNavigator()) {
        // TODO: search for template that has the most slots available for
        //       moves for flexibility to add other moves later.
        if (!it->usesSlot(m1->bus().name())
            // Let's be conservative and not use IT writing to some IUs
            // causing overwrites.
            && it->numberOfDestinations() == 0) {

            std::unique_ptr<Instruction> instr(new Instruction(*it));
            instr->addMove(std::shared_ptr<Move>(m1));
            return instr.release();
        }
    }
    delete m1;
    return nullptr;
}

/**
 * Returns sequence of instruction having only no-operation moves or
 * equivalent.
 */
TTAProgram::CodeSnippet*
SnippetGenerator::getNOP(const TTAMachine::Machine& mach) {
    for (auto it : mach.instructionTemplateNavigator()) {
        if (it->numberOfDestinations() == 0) {
            return getCodeSnippet(new Instruction(*it));
        }
    }
    return nullptr;
}

/**
 * Returns any port terminal connected to the bus to write into.
 *
 * Returned port terminal:
 * - does not trigger operations,
 * - has register index of zero in case of RF port.
 */
TTAProgram::Terminal*
SnippetGenerator::getAnyWritablePort(
    const TTAMachine::Bus& bus) {

    using MCC = MachineConnectivityCheck;

    auto mach = getRealMachine(bus);
    for (auto rf : mach->registerFileNavigator()) {
        auto regTerm = getAnyWritablePort(bus, *rf, 0);
        if (regTerm) return regTerm;
    }

    for (auto fu : mach->functionUnitNavigator()) {
        for (int i = 0; i < fu->portCount(); i ++) {
            auto fuPort = fu->port(i);
            if (!fuPort->isTriggering()
                && MCC::busConnectedToPort(bus, *fuPort)) {
                return new TerminalFUPort(*fuPort);
            }
        }
    }

    return nullptr;
}

/**
 * Return first occurring writable port that is connected to the bus.
 */
TTAProgram::TerminalRegister*
SnippetGenerator::getAnyWritablePort(
    const TTAMachine::Bus& bus,
    const TTAMachine::RegisterFile& rf,
    int registerIndex) {

    using MCC = MachineConnectivityCheck;

    auto mach = getRealMachine(rf);
    if (!mach) return nullptr;
    for (auto rfPort : MCC::findWritePorts(rf)) {
        if (MCC::isConnected(bus, *rfPort)) {
            return new TerminalRegister(*rfPort, registerIndex);
        }
    }

    return nullptr;
}

/**
 * Return real machine or nullptr.
 */
const TTAMachine::Machine*
SnippetGenerator::getRealMachine(
    const TTAMachine::RegisterFile& rf) {

    if (rf.machine() && !rf.machine()->isUniversalMachine()) {
        return rf.machine();
    }
    return nullptr;
}

/**
 * Return real machine or nullptr.
 */
const TTAMachine::Machine*
SnippetGenerator::getRealMachine(const TTAProgram::Move& move) {

    auto* mach = move.bus().machine();
    if (mach && !mach->isUniversalMachine()) {
        return mach;
    }
    return nullptr;
}

/**
 * Return real machine or nullptr.
 */
const TTAMachine::Machine*
SnippetGenerator::getRealMachine(
    const TTAMachine::Bus& bus) {
    auto* mach = bus.machine();
    if (mach && !mach->isUniversalMachine()) {
        return mach;
    }
    return nullptr;
}

/**
 * Creates code snippet with the given instruction.
 */
TTAProgram::CodeSnippet*
SnippetGenerator::getCodeSnippet(
    TTAProgram::Instruction* instr) {

    if (!instr) return nullptr;

    auto snippet = new TTAProgram::CodeSnippet();
    snippet->add(instr);
    return snippet;
}

/**
 * Creates move to transport immediate to register.
 *
 * This method does not check its arguments for validity.
 */
TTAProgram::Move*
SnippetGenerator::getUncheckedSimmMove(
    int64_t value,
    TTAMachine::Bus& bus,
    const TTAMachine::RegisterFile& rf,
    int index) {

    using namespace TTAProgram;

    auto rfPortTerm = getAnyWritablePort(bus, rf, index);
    if (!rfPortTerm) return nullptr;

    return new Move(
        new TerminalImmediate(SimValue(value, rf.width())),
        rfPortTerm,
        bus);
}


