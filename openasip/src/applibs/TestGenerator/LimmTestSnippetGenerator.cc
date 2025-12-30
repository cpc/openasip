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
 * @file LimmTestSnippetGenerator.cc
 *
 * Implementation of LimmTestSnippetGenerator class.
 *
 * Created on: 2.8.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "LimmTestSnippetGenerator.hh"

#include <set>
#include <utility>
#include <memory>

#include "Machine.hh"
#include "InstructionTemplate.hh"
#include "TemplateSlot.hh"

#include "CodeSnippet.hh"
#include "Instruction.hh"
#include "Move.hh"
#include "Immediate.hh"
#include "TerminalImmediate.hh"
#include "TerminalRegister.hh"

#include "MachineConnectivityCheck.hh"

#include "CodeSnippet.hh"
#include "Procedure.hh"

#include "MathTools.hh"


LimmTestSnippetGenerator::LimmTestSnippetGenerator(
    const TTAMachine::InstructionTemplate& it) : it_(it) {

    using TTAMachine::ImmediateUnit;

    std::set<const ImmediateUnit*> ius;

    // Gather unique IUs targeted by the IT.
    for (int i = 0; i < it.slotCount(); i++) {
        if (it.slot(i)->width() > 0) {
            assert(it.slot(i)->destination());
            ius.insert(it.slot(i)->destination());
        }
    }

    // Create test input points and update supported widths.
    for (auto iu : ius) {
        testInputs_.emplace_back(targetIU(it.supportedWidth(*iu), *iu));
    }

    // Values loaded into IUs are made visible for bus trace verification. That
    // is, the loaded values are transported via some buses.
    // Init the structs with a lambda function:
    //    (iu register index : int) -> Instruction*,
    // where the returned instruction does the transport from a single IU.
    for (targetIU& tiu : testInputs_) {
        tiu.sinkFn = getSinkRoute(tiu.iu, tiu.bitWidth);
        if (!tiu.sinkFn) return;
    }

    valid_ = true;
}

LimmTestSnippetGenerator::~LimmTestSnippetGenerator() {
}

int
LimmTestSnippetGenerator::inputCount() const {
    assert(valid_ && "Snippet template generator is not valid to use.");
    return testInputs_.size();
}

int
LimmTestSnippetGenerator::inputBitWidth(int i) const {
    assert(valid_ && "Snippet template generator is not valid to use.");
    return testInputs_.at(i).bitWidth;
}

/**
 * Sets value to be loaded into some IU.
 *
 * The value must be requiredBits(val) <= inputBitWidth(i).
 *
 * At construction time, by default, input values are zeroes.
 */
void
LimmTestSnippetGenerator::assignInput(int i, uint64_t val) {
    assert(valid_ && "Snippet template generator is not valid to use.");
    assert(testInputs_.at(i).bitWidth > 0);
    assert(MathTools::requiredBits(val) <= testInputs_.at(i).bitWidth);
    testInputs_.at(i).testInput = val;
}

/**
 * Generates the snippet.
 */
TTAProgram::CodeSnippet*
LimmTestSnippetGenerator::generateSnippet() const {
    assert(valid_ && "Snippet template generator is not valid to use.");

    TTAProgram::CodeSnippet* code = new TTAProgram::CodeSnippet();
    assert(!testInputs_.empty());
    code->add(makeLimmLoadInstruction(testInputs_, it_));
    for (const auto& i : testInputs_) {
        code->add(i.sinkFn(0));
    }

    return code; //placeholder
}

/**
 * Used for creating lambda function that creates an instruction to do value
 * transportation from an IU to anywhere.
 */
std::function<TTAProgram::Instruction*(int)> makeSinkFn(
    const TTAMachine::RFPort& src,
    TTAMachine::Bus& bus,
    const TTAMachine::Port& dst,
    const TTAMachine::InstructionTemplate& it) {

    using namespace TTAProgram;

    std::shared_ptr<Instruction> instrTempl(new Instruction(it));
    auto srcTerm = new TerminalRegister(src, 0);
    auto dstTerm = new TerminalRegister(dst, 0);
    instrTempl->addMove(std::make_shared<Move>(srcTerm, dstTerm, bus));

    auto result = [=](int) -> TTAProgram::Instruction* {
        return instrTempl->copy();
    };

    return result;
}


/**
 * Creates function that generates an instruction to transport value from IU to
 * anywhere.
 *
 * May return empty function.
 */
std::function<TTAProgram::Instruction*(int)>
LimmTestSnippetGenerator::getSinkRoute(
    const TTAMachine::ImmediateUnit& iu, int transportWidth) {

    using namespace TTAMachine;
    using MCC = MachineConnectivityCheck;

    std::function<TTAProgram::Instruction*(int)> result; // Empty function.

    const Machine& mach = *iu.machine();

    std::vector<const TTAMachine::Port*> sinkPorts;
    for (auto rf : mach.registerFileNavigator()) {
        if (rf->maxWrites() < 1 || rf->size() < 1)
            continue;
        const TTAMachine::Port* rfPort = rf->firstReadPort();
        if (rfPort->width() < transportWidth) continue;
        sinkPorts.push_back(rfPort);
    }

    std::vector<std::pair<Bus*, InstructionTemplate*>> busRoutes;
    for (auto bus : mach.busNavigator()) {
        InstructionTemplate* itForMove = nullptr;
        for (auto it : mach.instructionTemplateNavigator()) {
            if (!it->usesSlot(bus->name())
                && it->numberOfDestinations() == 0) {
                itForMove = it;
                break;
            }
        }

        busRoutes.push_back(std::make_pair(bus, itForMove));
    }

    for(int srcIdx = 0; srcIdx < iu.portCount(); srcIdx++) {
        auto srcPort = iu.port(srcIdx);
        if (srcPort->width() < transportWidth) continue;

        for (auto sinkPort : sinkPorts) {
            for (auto bus : busRoutes) {
                if (MCC::isConnected(*srcPort, *bus.first)
                    && MCC::isConnected(*bus.first, *sinkPort)) {
                    return makeSinkFn(
                        *srcPort, *bus.first, *sinkPort, *bus.second);
                }
            }
        }
    }

    return result;
}

/**
 * Creates instruction that loads values into IUs using the instruction
 * template.
 */
TTAProgram::Instruction*
LimmTestSnippetGenerator::makeLimmLoadInstruction(
    const std::vector<targetIU>& inputValues,
    const TTAMachine::InstructionTemplate& it) {

    using namespace TTAProgram;

    Instruction* instr = new Instruction(it);

    for (auto input : inputValues) {
        auto termImm = new TerminalImmediate(
            SimValue(input.testInput, input.bitWidth));
        auto iuTerm = new TerminalRegister(*input.iu.port(0), 0);
        instr->addImmediate(std::make_shared<Immediate>(termImm, iuTerm));
    }

    return instr;
}


