/*
    Copyright (c) 2002-2021 Tampere University.

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
/**
 * @file BlocksTranslator.cc
 *
 * Implementation of the translation (Blocks to TTA) functions.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#include "BlocksTranslator.hh"

#include <map>
#include <memory>
#include <string>

#include "BlocksModel.hh"
#include "ExecutionPipeline.hh"
#include "Segment.hh"

using namespace TTAMachine;
using namespace std;

/**
 * Build the TTA model (adf file) from the Blocks model.
 *
 * @param BlocksModel The Blocks model created from the Blocks
 * 'architecture.xml'.
 * @param outputName The name of the output adf.
 */
void
BlocksTranslator::BuildTTAModel(
    BlocksModel& blocksModel, const string& outputName) {
    Machine ttaMach;
    ttaMach.setLittleEndian(true);
    // Create data and instruction adress spaces
    const int bitWidth = 8;
    const unsigned int minAddress = 0;
    // TODO(mm): Allow for different instr mem size than default
    const unsigned int maxAddressInstr = 2046;
    // TODO(mm): Allow for different data mem size than default
    const unsigned int maxAddressData = 32768;
    AddressSpace asData(
        "data", bitWidth, minAddress, maxAddressData, ttaMach);
    AddressSpace asInstr(
        "instructions", bitWidth, minAddress, maxAddressInstr, ttaMach);

    // Create a list for each functional unit type
    list<BlocksALUPair*> aluList;
    list<BlocksLSUPair*> lsuList;
    list<BlocksRF*> rfList;
    list<BlocksIMM*> iuList;
    list<BlocksMULPair*> mulList;

    // Default required unit
    InstructionTemplate limm("limm", ttaMach);
    BlocksGCU gcu(
        ttaMach, "abu",
        asInstr);  // Has to be created here so in IU slot can be added

    // Check which outputs of the FU are used, each output requires a seperate
    // TTA FU.
    for (auto& fu : blocksModel.mFunctionalUnitList) {
        for (auto& source : fu.src) {
            // Split the string into FU and port
            string sourcePort = source.substr(source.rfind(".") + 1);
            string sourceFu = source.substr(0, source.rfind("."));
            // Set the usesOutx to true
            for (auto& srcFu : blocksModel.mFunctionalUnitList) {
                if (srcFu.name == sourceFu) {
                    if (sourcePort == "0")
                        srcFu.usesOut0 = true;
                    else
                        srcFu.usesOut1 = true;
                }
            }
        }
    }

    // Create for each Blocks unit a TTA unit (except default GCU and IDs)
    for (auto& fu : blocksModel.mFunctionalUnitList) {
        switch (fu.type) {
            case FU_TYPE::ID:
                break;  // Instruction decoders are not taken into account.
            case FU_TYPE::LSU:
                lsuList.push_back(new BlocksLSUPair(
                    ttaMach, fu.name, fu.src, asData, fu.usesOut0,
                    fu.usesOut1));
                break;
            case FU_TYPE::ALU:
                aluList.push_back(new BlocksALUPair(
                    ttaMach, fu.name, fu.src, fu.usesOut0, fu.usesOut1));
                break;
            case FU_TYPE::RF:
                rfList.push_back(new BlocksRF(ttaMach, fu.name, fu.src));
                break;
            case FU_TYPE::IU:
                iuList.push_back(new BlocksIMM(ttaMach, fu.name, fu.src));
                if (iuList.size() == 1)
                    limm.addSlot("ra_out_to_ra_in", 32, *(iuList.back()->iu));
                break;
            case FU_TYPE::MUL:
                mulList.push_back(new BlocksMULPair(
                    ttaMach, fu.name, fu.src, fu.usesOut0, fu.usesOut1));
                break;
            case FU_TYPE::ABU:
                gcu.sources = fu.src;
                break;
            default:
                try {
                    throw "Illegal function unit type in Blocks.xml file, aborting translation. \n";
                } catch (const char* error) {
                    fprintf(stderr, "%s", error);
                    Deinitialize(
                        ttaMach, aluList, lsuList, rfList, iuList, mulList);
                    return;
                }
        }
    }

    ConnectInputs(ttaMach, gcu, aluList, lsuList, rfList, mulList);
    ttaMach.writeToADF(outputName);
    Deinitialize(ttaMach, aluList, lsuList, rfList, iuList, mulList);
}

/**
 * Create a connection between two different sockets.
 * Note: Input := Bus to port, output := Port to bus.
 *
 * @param mach The TTA machine where the connection needs to be made.
 * @param inputSocket A pointer to the TTA input socket that needs to be
 * connected.
 * @param outputSocket A pointer to the TTA output socket that needs to be
 * connected.
 * @param gcu A reference to the TTA GCU (ABU) model.
 */
Bus*
BlocksTranslator::CreateConnection(
    Machine& mach, Socket* inputSocket, Socket* outputSocket,
    BlocksGCU& gcu) {
    // TODO(mm): add exception catching where given "input" is not an input
    // port
    const int busWidth = 32;
    const int immWidth = 0;
    const string& to = inputSocket->name();
    const string& from = outputSocket->name();
    Machine::Extension busExt = Machine::Extension::ZERO;
    Machine::BusNavigator busNav = mach.busNavigator();
    const string newBusNum = to_string(busNav.count());
    Bus* ttaBus;
    // Verify if the socket is already attached to a bus (excluding
    // 'ra_out_to_ra_in')
    if (inputSocket->segmentCount() == 0) {
        ttaBus = new Bus("bus_" + newBusNum, busWidth, immWidth, busExt);
        mach.addBus(*ttaBus);
        new Segment("seg1", *ttaBus);
        if (inputSocket->name() == "ra_in" || inputSocket->name() == "pc") {
            gcu.pcIn->attachBus(*ttaBus);
            gcu.raIn->attachBus(*ttaBus);
        } else {
            inputSocket->attachBus(*ttaBus);
        }
    } else if (
        inputSocket->segmentCount() == 1 &&
        inputSocket->segment(0)->parentBus()->name() == "ra_out_to_ra_in") {
        ttaBus = new Bus("bus_" + newBusNum, busWidth, immWidth, busExt);
        mach.addBus(*ttaBus);
        new Segment("seg1", *ttaBus);
        if (inputSocket->name() == "ra_in" || inputSocket->name() == "pc") {
            gcu.pcIn->attachBus(*ttaBus);
            gcu.raIn->attachBus(*ttaBus);
        } else {
            inputSocket->attachBus(*ttaBus);
        }
    } else if (
        inputSocket->segmentCount() == 2 &&
        inputSocket->segment(0)->parentBus()->name() == "ra_out_to_ra_in") {
        Segment* segment1 = inputSocket->segment(1);
        ttaBus = segment1->parentBus();
    }
    // Not 'ra_out_to_ra_in' and segmentCount == 1
    else {
        Segment* segment1 = inputSocket->segment(0);
        ttaBus = segment1->parentBus();
    }
    // Attach busses to sockets
    outputSocket->attachBus(*ttaBus);
    outputSocket->setDirection(Socket::Direction::OUTPUT);
    return ttaBus;
}

/**
 * Create all the connectivity in the TTA model.
 *
 * @param mach The TTA machine where the connections need to be made.
 * @param gcu A reference to the TTA GCU (ABU) model.
 * @param aluList A list with all ALU pairs currently in the TTA model.
 * @param lsuList A list with all LSU pairs currently in the TTA model.
 * @param rfList A list with all RFs currently in the TTA model.
 * @param mulList A list with all MUL pairs currently in the TTA model.
 */
void
BlocksTranslator::ConnectInputs(
    TTAMachine::Machine& mach, BlocksGCU& gcu,
    std::list<BlocksALUPair*> aluList, std::list<BlocksLSUPair*> lsuList,
    std::list<BlocksRF*> rfList, std::list<BlocksMULPair*> mulList) {
    Machine::SocketNavigator nav = mach.socketNavigator();
    // Create ALU connections (all connections that are an input to the ALUs)
    for (auto& blocksAlu : aluList) {
        for (auto& source : blocksAlu->sources) {
            Socket* outputSocket = FindOutputSocket(nav, source);
            CreateConnection(
                mach, blocksAlu->in1sock.get(), outputSocket, gcu);
            CreateConnection(
                mach, blocksAlu->in2sock.get(), outputSocket, gcu);
        }
    }

    // Create LSU connections (all connections that are an input to the LSUs)
    for (auto& lsu : lsuList) {
        for (auto& source : lsu->sources) {
            Socket* outputSocket = FindOutputSocket(nav, source);
            CreateConnection(mach, lsu->in1sock.get(), outputSocket, gcu);
            CreateConnection(mach, lsu->in2sock.get(), outputSocket, gcu);
        }
    }

    // Create MUL connections (all connections that are an input to the MULs)
    for (auto& mul : mulList) {
        for (auto& source : mul->sources) {
            Socket* outputSocket = FindOutputSocket(nav, source);
            CreateConnection(mach, mul->in1sock.get(), outputSocket, gcu);
            CreateConnection(mach, mul->in2sock.get(), outputSocket, gcu);
        }
    }

    // Create RF connections (all connections that are an input to the RFs)
    for (auto& rf : rfList) {
        for (auto& source : rf->sources) {
            Socket* outputSocket = FindOutputSocket(nav, source);
            CreateConnection(mach, rf->in1sock, outputSocket, gcu);
        }
    }

    // IMM has no input socket so is skipped.
    // Create GCU connections
    for (auto& source : gcu.sources) {
        Socket* outputSocket = FindOutputSocket(nav, source);
        CreateConnection(mach, gcu.pcIn, outputSocket, gcu);
        CreateConnection(mach, gcu.valIn, outputSocket, gcu);
    }
}

/**
 * Find the output socket corresponding to a FU name.
 *
 * @param nav An instance of a socketnavigator.
 * @param source The source of which the handle to the output socket needs to
 * be returned.
 */
Socket*
BlocksTranslator::FindOutputSocket(
    Machine::SocketNavigator nav, string source) {
    // String manipulation to get the right format
    source.replace(source.rfind("."), 1, "_out");
    assert(
        nav.hasItem(source) &&
        "Cannot create connection, output socket not found!");
    return nav.item(source);
}

/**
 * Clean up the memory, deletes all FUs and busses.
 *
 * @param mach The TTA machine where the connections need to be made.
 * @param aluList A list with all ALU pairs currently in the TTA model.
 * @param lsuList A list with all LSU pairs currently in the TTA model.
 * @param rfList A list with all RFs currently in the TTA model.
 * @param mulList A list with all MUL pairs currently in the TTA model.
 */
void
BlocksTranslator::Deinitialize(
    TTAMachine::Machine& mach, std::list<BlocksALUPair*> aluList,
    std::list<BlocksLSUPair*> lsuList, std::list<BlocksRF*> rfList,
    std::list<BlocksIMM*> iuList, std::list<BlocksMULPair*> mulList) {
    while (!aluList.empty()) {
        delete aluList.front();
        aluList.pop_front();
    }
    while (!lsuList.empty()) {
        delete lsuList.front();
        lsuList.pop_front();
    }
    while (!rfList.empty()) {
        delete rfList.front();
        rfList.pop_front();
    }
    while (!iuList.empty()) {
        delete iuList.front();
        iuList.pop_front();
    }
    while (!mulList.empty()) {
        delete mulList.front();
        mulList.pop_front();
    }
    // Delete busses
    Machine::BusNavigator busNav = mach.busNavigator();
    while (busNav.count() != 0) {
        Bus* toDelete = busNav.item(busNav.count() - 1);
        delete toDelete->segment(0);
        delete toDelete;
    }
}

/**
 * main()
 */
int
main(int argc, char* argv[]) {
    // Define a structured way to represent the .xml architecture.
    // Verify input arguments and open xml file.
    try {
        if (argc != 3)
            throw "Invalid number of arguments, expected 2 arguments. \n";
    } catch (const char* error) {
        fprintf(stderr, "%s", error);
        fprintf(
            stdout,
            "Expected path to .xml file with Blocks architecture and name of "
            "output architecture (<name>.adf). \n");
        return 1;
    }

    const string filepath = string(argv[1]);
    const string outputFileName = string(argv[2]);

    BlocksModel blocksModel(filepath);
    BlocksTranslator::BuildTTAModel(blocksModel, outputFileName);
}
