/*
    Copyright (c) 2002-2020 Tampere University.

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
 * @file ExplorationTools.hh
 *
 * Declaration of ExplorationTools class that includes some helper functions
 * used in automated design space exploration.
 *
 * @author Alex Hirvonen (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#include "ExplorationTools.hh"

#include "ADFSerializer.hh"
#include "AddressSpace.hh"
#include "Application.hh"
#include "ExecutionPipeline.hh"
#include "FUPort.hh"
#include "FileSystem.hh"
#include "HWOperation.hh"
#include "Operand.hh"
#include "Segment.hh"

/**
 * Connects given port to the first Bus in the machine.
 *
 * @param mach Machine for that port.
 * @param port FUPort or RFPort
 * @param direction Socket direction.
 */
void
ExplorationTools::connectPortToBus(
    Port* port, Bus* bus, Socket::Direction direction) {
    Machine* mach = port->parentUnit()->machine();

    // create socket with unique name
    static unsigned int idx = 0;
    std::string socketPrefix = "S_";
    const Machine::SocketNavigator socketNav = mach->socketNavigator();

    while (socketNav.hasItem(socketPrefix + Conversion::toString(idx))) {
        idx++;
    }
    Socket* socket = new Socket(socketPrefix + Conversion::toString(idx));
    mach->addSocket(*socket);

    // attach to bus
    bus->segment(0)->attachSocket(*socket);
    socket->setDirection(direction);

    port->attachSocket(*socket);
}

/**
 * Adds operation to function unit.
 *
 * @param Operation from OSAL.
 * @param Function unit to add to.
 * @returns created Function unit pointer.
 */
FunctionUnit*
ExplorationTools::createOperationFU(Machine* mach, const Operation& osalOp) {
    TCEString opName = osalOp.name();

    // create function unit with unique name
    unsigned int idx = 1;
    std::string fuPrefix = opName + "_";
    const Machine::FunctionUnitNavigator fuNav =
        mach->functionUnitNavigator();

    while (fuNav.hasItem(fuPrefix + Conversion::toString(idx))) {
        idx++;
    }
    FunctionUnit* fu = new FunctionUnit(fuPrefix + Conversion::toString(idx));
    mach->addFunctionUnit(*fu);

    // set default address space for LSUs
    TTAMachine::AddressSpace* addrSpace =
        mach->addressSpaceNavigator().item("data");
    if (osalOp.usesMemory()) {
        fu->setAddressSpace(addrSpace);
    }

    HWOperation* hwOp = new HWOperation(opName, *fu);

    // create ports for each operand
    for (int j = 1; j <= osalOp.operandCount(); j++) {
        Operand operand = osalOp.operand(j);

        int opWidth = operand.elementWidth() * operand.elementCount();

        FUPort* port = NULL;
        if (operand.isInput()) {
            port = new FUPort(
                "in" + Conversion::toString(j), opWidth, *fu, false, false);

            if (j == 1) {
                port->setName(port->name() + "t");
                port->setTriggering(true);

                // 64bit support triggering problem workaround
                // Change addressing port from 64bit to 32bit
                if (opName == "LD32" || opName == "ST32" || opName == "LD8" ||
                    opName == "LD16" || opName == "LDU8" ||
                    opName == "LDU16") {
                    port->setWidth(32);
                }  // workaround end
            }
            hwOp->pipeline()->addPortRead(
                operand.index(), opCycleStart(opName, true), 1);

        } else {
            port = new FUPort(
                "out" + Conversion::toString(j - osalOp.numberOfInputs()),
                opWidth, *fu, false, false);

            hwOp->pipeline()->addPortWrite(
                operand.index(), opCycleStart(opName, false), 1);
        }
        hwOp->bindPort(j, *port);

        // connect port to FIRST bus via socket
        Socket::Direction dir =
            (operand.isInput()) ? Socket::INPUT : Socket::OUTPUT;
        connectPortToBus(port, mach->busNavigator().item(0), dir);
    }
    return fu;
}

/**
 * Returns start cycle of read/write for certain operation.
 *
 * Used to set default latencies for operations.
 *
 * @param Name of the operation.
 * @param Read/Write?: Read cycle = true, Write = false.
 * @return Start cycle index.
 */
int
ExplorationTools::opCycleStart(TCEString opName, bool isReadCycle) {
    int readCycle = 0;
    int writeCycle = 1;

    if (opName.startsWith("MUL") || opName.startsWith("DIV") ||
        opName.startsWith("MAC")) {
        writeCycle = 3;
    } else if (opName.startsWith("LD") || opName.startsWith("ALD")) {
        writeCycle = 3;
    } else if ((opName == "SHL1ADD") || (opName == "SHL2ADD")) {
        writeCycle = 1;
    } else if (
        (opName == "SHL") || (opName == "SHL64") || (opName == "SHR") ||
        (opName == "SHR64") || (opName == "SHRU") || (opName == "SHRU64")) {
        writeCycle = 2;
    } else if (
        opName.startsWith("SHL1") || opName.startsWith("SHL2") ||
        opName.startsWith("SHL4") || opName.startsWith("SHL8") ||
        opName.startsWith("SHL16") || opName.startsWith("SHL32") ||
        opName.startsWith("SHR1") || opName.startsWith("SHR2") ||
        opName.startsWith("SHR4") || opName.startsWith("SHR8") ||
        opName.startsWith("SHR16") || opName.startsWith("SHR32") ||
        opName.startsWith("SHRU1") || opName.startsWith("SHRU2") ||
        opName.startsWith("SHRU4") || opName.startsWith("SHRU8") ||
        opName.startsWith("SHRU16") || opName.startsWith("SHRU32")) {
        writeCycle = 1;
    }
    if (isReadCycle) {
        return readCycle;
    } else {
        return writeCycle;
    }
}

/**
 * Returns list of RF names in width DECREASING order
 *
 * @param mach TTA architecture
 * @return vector of RF ids sorted by DECREASING width order.
 */
std::vector<TCEString>
ExplorationTools::sortRFsByWidth(Machine* mach) {
    std::vector<TCEString> sortedRFids{};
    Machine::RegisterFileNavigator rfNavi = mach->registerFileNavigator();

    for (int i = 0; i < rfNavi.count(); i++) {
        RegisterFile* rf = rfNavi.item(i);

        if (sortedRFids.empty()) {
            sortedRFids.push_back(rf->name());
            continue;
        }

        for (unsigned int j = 0; j < sortedRFids.size(); j++) {
            int w = rfNavi.item(sortedRFids.at(j))->width();

            if (w <= rf->width()) {
                sortedRFids.insert(sortedRFids.begin() + j, rf->name());
                break;
            } else {
                if (j == (sortedRFids.size() - 1)) {
                    sortedRFids.push_back(rf->name());
                    break;
                }
            }
        }
    }
    return sortedRFids;
}
