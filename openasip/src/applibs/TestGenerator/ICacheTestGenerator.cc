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
 * @file ICacheTestGenerator.cc
 *
 * Implementation of ICacheTestGenerator class.
 *
 * Created on: 21.9.2015
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "ICacheTestGenerator.hh"

#include <memory>
#include <algorithm>
#include <map>
#include <limits>

#include "Machine.hh"
#include "MachineConnectivityCheck.hh"
#include "MachineInfo.hh"
#include "Move.hh"
#include "Bus.hh"
#include "ControlUnit.hh"
#include "Terminal.hh"
#include "TerminalRegister.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
#include "Instruction.hh"
#include "Procedure.hh"
#include "Program.hh"
#include "FUPort.hh"
#include "Immediate.hh"

#include "Application.hh"
#include "MathTools.hh"

#include "MachineImplementation.hh"
#include "CacheImplementation.hh"

//#define DEBUG_ICACHETESTGEN

static const std::vector<TestCase> emptyTestSet = std::vector<TestCase>();


ICacheTestGenerator::ICacheTestGenerator()
    : TestGeneratorBase(
        "icache-test-generator",
        "Creates a test program that stresses instruction cache(s) if.",
        true) {

}

ICacheTestGenerator::~ICacheTestGenerator() {
}

std::vector<TestCase>
ICacheTestGenerator::generateTestCasesImpl(
    RandomNumberGenerator::SeedType seed) {

    using namespace TTAMachine;
    using namespace TTAProgram;

    if (!initializedWithIdf() || !initializedWithAdf()) {
        if (Application::spamVerbose()) {
            std::cerr << title() << ": Missing ADF and/or IDF. Skipping."
                << std::endl;
        }
        return emptyTestSet;
    }
    if (!implementation().hasL1InstructionCache()) {
        if (Application::spamVerbose()) {
            std::cerr << title()
                << ": IDF lacks instruction cache. Skipping."
                << std::endl;
        }
        return emptyTestSet;
    }

    seed_ = seed;
    std::unique_ptr<Program> prog(new Program(
        *machine().controlUnit()->addressSpace()));

    generateTestProgram(*prog);
    if (prog->procedureCount() == 0) {
        return emptyTestSet;
    }

    //todo simulate reference data for cache statistics?

    return std::vector<TestCase>{TestCase(prog.release(), "icache-test")};
}

/**
 * Generates the test program for instruction cache.
 */
void
ICacheTestGenerator::generateTestProgram(TTAProgram::Program& prog) {
    using namespace TTAMachine;
    using namespace TTAProgram;

    /*
     * Generates jumps to random locations in subspace of instruction address
     * space that should cover L1 instruction cache. That is, the cache would
     * get fully filled and have some its blocks evicted.
     *
     * The subspace is divided into jump block - that consists of jump
     * instruction and delay slots (NOPs) - and they are chained together.
     */

    if (!machine().controlUnit()->hasOperation("jump")) {
        if (Application::spamVerbose()) {
            std::cerr << title()
                << ": TTA does not have jump capability. Skipping."
                << std::endl;
        }
        return;
    }

    const IDF::CacheImplementation& l1 = implementation().l1InstructionCache();
    int cacheSize = l1.blockSize() * l1.setSize() * l1.cacheSize();
    const InstructionAddress startAddr =
        machine().controlUnit()->addressSpace()->start();
    // Set limit to point where instruction cache should get filled and have
    // its blocks be evicted but not past machines usable address space.
    InstructionAddress upperLimitAddr =
        std::min(startAddr + static_cast<LongWord>(cacheSize*3),
            machine().controlUnit()->addressSpace()->end());
    const unsigned delaySlots =
        static_cast<unsigned>(machine().controlUnit()->delaySlots());
    FUPort& jumpPort =
        *dynamic_cast<FUPort*>(machine().controlUnit()->triggerPort());

    LIMMSource limmSource = selectLIMMSource(jumpPort);
    bool useLimm = false;
    Bus* jumpAddressFromSimmBus = selectSIMMSource(jumpPort);

    // If SIMM cannot use (lacking range for upper addresses) check if LIMM
    // can be used instead for jump addresses.
    unsigned maxJumpAddress = 0;
    if (jumpAddressFromSimmBus == nullptr ||
        (jumpAddressFromSimmBus->immediateWidth() <
            MathTools::requiredBits(upperLimitAddr))) {

        if (std::get<0>(limmSource) >= static_cast<unsigned>(
            MathTools::requiredBits(upperLimitAddr))) {
            useLimm = true;
            if (std::get<0>(limmSource) > 31) {
                maxJumpAddress = std::numeric_limits<unsigned>::max();
            } else {
                maxJumpAddress = (1 << std::get<0>(limmSource)) - 1;
            }
        } else {
            if (Application::spamVerbose()) {
                std::cerr << title()
                    << ": Cannot generate random jumps. Skipping."
                    << std::endl;
            }
            return;
        }
    } else {
        unsigned simmWidth = jumpAddressFromSimmBus->immediateWidth();
#ifdef DEBUG_ICACHETESTGEN
        std::cerr << "simmWidth = " << simmWidth << std::endl;
#endif
        if (simmWidth > 31) {
            maxJumpAddress = std::numeric_limits<unsigned>::max();
        } else {
            maxJumpAddress = (1 << simmWidth) - 1;
        }
    }

#ifdef DEBUG_ICACHETESTGEN
    if (useLimm && std::get<0>(limmSource) > 0) {
        std::cerr << "bits: " << std::get<0>(limmSource) << std::endl;
        std::cerr << " bus: " << std::get<1>(limmSource)->name() << std::endl;
        std::cerr << "  IU: " << std::get<2>(limmSource)->name() << std::endl;
        std::cerr << "  IT: " << std::get<3>(limmSource)->name() << std::endl;
    }
#endif

    maxJumpAddress = std::min(maxJumpAddress, upperLimitAddr);
    std::unique_ptr<Procedure> jumpProc(new Procedure(
        "randomjumper", *machine().controlUnit()->addressSpace()));

#ifdef DEBUG_ICACHETESTGEN
    std::cerr << "maxJumpAddress = " << maxJumpAddress << std::endl;
#endif

    std::vector<InstructionAddress> jumpChain;
    unsigned jumpBlockSize = (useLimm)?(2+delaySlots):(1+delaySlots);
    generateJumpTargets(jumpChain, startAddr, maxJumpAddress, jumpBlockSize);

#ifdef DEBUG_ICACHETESTGEN
    std::cerr << "Jumps:";
#endif
    std::map<InstructionAddress, InstructionAddress> jumpFromToTable;
    InstructionAddress currentAddr = startAddr;
    for (InstructionAddress next : jumpChain) {
        jumpFromToTable.insert({currentAddr, next});
#ifdef DEBUG_ICACHETESTGEN
        std::cerr << "[" << currentAddr << " -> " << next << "] ";
#endif
        currentAddr = next;
    }
#ifdef DEBUG_ICACHETESTGEN
    std::cerr << std::endl;
#endif

    for (auto jumpFromTo : jumpFromToTable) {
        if (useLimm) {
            generateJumpWithLIMM(*jumpProc, *std::get<1>(limmSource),
                jumpPort, jumpFromTo.second, limmSource);
        } else {
            generateJumpWithSIMM(*jumpProc, *jumpAddressFromSimmBus,
                jumpPort, jumpFromTo.second);
        }
    }
    if (jumpProc->instructionCount() > 0) {
        prog.addProcedure(jumpProc.release());
    }
}

/**
 * Selects suitable SIMM source that can be used directly for target port
 * (forPort).
 *
 * Selects greedily a SIMM source that holds a largest value.
 *
 * @param forPort The target port.
 * @returns Suitable SIMM source bus connected to given port. Nullptr if could
 *          not found the suitable source.
 */
TTAMachine::Bus*
ICacheTestGenerator::selectSIMMSource(TTAMachine::FUPort& forPort) {
    using namespace TTAMachine;
    using namespace TTAProgram;
    using MCC = MachineConnectivityCheck;

    Bus* selectedBus = nullptr;
    unsigned maxUnsignedSimm = 0;
    for (Bus* busCandidate : machine().busNavigator()) {
        if (MCC::isConnected(forPort, *busCandidate)) {
            if (busCandidate->immediateWidth() <= 0) {
                continue;
            }
            unsigned simmWidth = busCandidate->immediateWidth();
            if (simmWidth > maxUnsignedSimm) {
                maxUnsignedSimm = busCandidate->immediateWidth();
                selectedBus = busCandidate;
            }
        }
    }
    return selectedBus;
}

/**
 * Selects suitable LIMM source that can be used directly for target port
 * (forPort).
 *
 * Selects greedily a LIMM source that holds a largest value.
 *
 * @param forPort The target port.
 * @returns Returns tuple of (unsigned, Bus*, IU*, InstructionTemplate*).
 *          unsigned is for largest value to be carried to the target port.
 *          Bus pointer is the suitable way from a IU to target port. The IU
 *          pointer is for the suitable LIMM source. The InstructionTemplate
 *          pointer is the selected template to carry the LIMM.
 */
ICacheTestGenerator::LIMMSource
ICacheTestGenerator::selectLIMMSource(TTAMachine::FUPort& forPort) {
    using namespace TTAMachine;
    using namespace TTAProgram;
    using MCC = MachineConnectivityCheck;

    // todo? should take account the port widths too.

    LIMMSource result{0, nullptr, nullptr, nullptr};
    if (machine().immediateUnitNavigator().count() == 0) {
        return result;
    }

    std::vector<ImmediateUnit*> suitableIUs;
    std::map<ImmediateUnit*, MCC::BusSet> suitableRoutes;
    for (ImmediateUnit* iu : machine().immediateUnitNavigator()) {
        if (iu->portCount() == 0) continue;
        RFPort* iuPort = iu->port(0);
        MCC::BusSet tmp = MCC::findRoutes(*iuPort, forPort);
        suitableRoutes[iu].insert(tmp.begin(), tmp.end());
        suitableIUs.push_back(iu);
    }

    for (ImmediateUnit* iu : suitableIUs) {
        unsigned widestFoundBitWidthViaBus = 0;
        Bus* selectedRoute = nullptr;

        for (Bus* route : suitableRoutes.at(iu)) {
            unsigned routeWidth = static_cast<unsigned>(route->width());
            if (routeWidth > widestFoundBitWidthViaBus) {
                widestFoundBitWidthViaBus = route->width();
                selectedRoute = route;
            }
        }

        unsigned int widestFoundBitWidthViaIT = 0;
        InstructionTemplate* selectedIT = nullptr;
        for (InstructionTemplate* it :
            machine().instructionTemplateNavigator()) {

            unsigned widestWidthViaIT = it->supportedWidth(*iu);
            // Seeking max unsigned value to be carried -> drop one bit if
            // sign extended.
            if (widestWidthViaIT > 0 && iu->signExtends()) {
                widestWidthViaIT -= 1;
            }
            // Note: now convert bit width -> largest value holded.
            if (widestWidthViaIT > widestFoundBitWidthViaIT) {
                widestFoundBitWidthViaIT = widestWidthViaIT;
                selectedIT = it;
            }
        }

        if (selectedRoute != nullptr && selectedIT != nullptr) {
            unsigned widestBitWidthCarried = std::min(
                widestFoundBitWidthViaBus, widestFoundBitWidthViaIT);
            if (widestBitWidthCarried > std::get<0>(result)) {
                assert(selectedRoute != nullptr);
                assert(iu != nullptr);
                assert(selectedIT != nullptr);
                result = std::make_tuple(
                    widestBitWidthCarried,
                    selectedRoute, iu, selectedIT);
            }
        }
    }

    return result;
}

/**
 * Generates random jump targets ahead of jumpTargets and at maximum distance
 * of maxJumpHeight.
 *
 * The result list consists of addresses to jump in order:
 * addr0 -> addr1 -> addr2 -> ... -> addrn.
 * Last item in jumpTargets is the largest address; others are randomized.
 */
void
ICacheTestGenerator::generateJumpTargets(
    std::vector<InstructionAddress>& jumpTargets,
    InstructionAddress startJumpLoc,
    unsigned maxJumpHeight,
    unsigned jumpBlockSize) {

    jumpTargets.clear();
    if (maxJumpHeight < jumpBlockSize) {
        return;
    }

    // jump instruction + delay slots
    unsigned numTargets = maxJumpHeight/jumpBlockSize;

    jumpTargets.reserve(numTargets);
    for (unsigned i = jumpBlockSize; i <= (maxJumpHeight - jumpBlockSize); ) {
        jumpTargets.push_back(startJumpLoc + i);
        i += jumpBlockSize;
    }
    if (jumpTargets.empty()) {
        return;
    }

    RandomNumberGenerator rng(seed_);
    std::random_shuffle(jumpTargets.begin(), --jumpTargets.end(),
        [&rng] (unsigned n) -> unsigned {
        return rng() % n;
    });
    seed_ = rng();
}

/**
 * Generates jump instruction (with delay slots) to given location and adds
 * it to procedure.
 */
void
ICacheTestGenerator::generateJumpWithSIMM(
    TTAProgram::Procedure& targetProc,
    TTAMachine::Bus& bus,
    const TTAMachine::FUPort& jumpPort,
    InstructionAddress jumpTarget) {

    using namespace TTAMachine;
    using namespace TTAProgram;

    //todo? select suitable instruction template.

    assert(jumpPort.isOpcodeSetting());
    const TTAMachine::HWOperation& jumpOper =
        *jumpPort.parentUnit()->operation("jump");

    Instruction* jumpInst = new Instruction();
    jumpInst->addMove(std::make_shared<Move>(
        new TerminalImmediate(SimValue(jumpTarget, bus.width())),
        new TerminalFUPort(jumpPort, jumpOper),
        bus));
    targetProc.add(jumpInst);
    // Add NOP instruction into delay slots.
    // todo: May be add some dummy moves instead of NOPs for bus and cache
    // activity?
    for (int i = 0; i < machine().controlUnit()->delaySlots(); i++) {
        targetProc.add(new Instruction());
    }
}

void
ICacheTestGenerator::generateJumpWithLIMM(
    TTAProgram::Procedure& targetProc,
    TTAMachine::Bus& bus,
    const TTAMachine::FUPort& jumpPort,
    InstructionAddress jumpTarget,
    ICacheTestGenerator::LIMMSource& limmSource) {

    using namespace TTAMachine;
    using namespace TTAProgram;


    unsigned limmWidth = std::get<0>(limmSource);
    ImmediateUnit* iu = std::get<2>(limmSource);
    InstructionTemplate* it = std::get<3>(limmSource);

    Instruction* limmInst = new Instruction(*it);
    Immediate* jumpAddr = new Immediate(
        new TerminalImmediate(SimValue(jumpTarget, limmWidth)),
        new TerminalRegister(*iu->port(0), 0));
    limmInst->addImmediate(std::shared_ptr<Immediate>(jumpAddr));

    assert(jumpPort.isOpcodeSetting());
    const TTAMachine::HWOperation& jumpOper =
        *jumpPort.parentUnit()->operation("jump");

    Instruction* jumpInst = new Instruction();
    jumpInst->addMove(std::make_shared<Move>(
        new TerminalRegister(*iu->port(0), 0),
        new TerminalFUPort(jumpPort, jumpOper),
        bus));

    targetProc.add(limmInst);
    targetProc.add(jumpInst);

    // Add NOP instruction into delay slots.
    // todo: May be add some dummy moves instead of NOPs for bus and cache
    // activity?
    for (int i = 0; i < machine().controlUnit()->delaySlots(); i++) {
        targetProc.add(new Instruction());
    }
}


void
ICacheTestGenerator::generateDummyInstructions(
    TTAProgram::Procedure& /*targetProc*/,
    unsigned int /*numOfInstructions*/) {

    // use limm templates
    // write simms
}

