/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file MachineConnectivityCheck.cc
 *
 * Implementation of MachineConnectivityCheck class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @author Heikki Kultala 2008 (hkultala-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <set>

#include "MachineConnectivityCheck.hh"
#include "MachineInfo.hh"
#include "Application.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "SetTools.hh"
#include "RegisterFile.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "MathTools.hh"
#include "TerminalImmediate.hh"
#include "Move.hh"
#include "TerminalFUPort.hh"
#include "HWOperation.hh"
#include "TCEString.hh"
#include "Operation.hh"
#include "FUPort.hh"
#include "ProgramOperation.hh"
#include "AssocTools.hh"
#include "StringTools.hh"
#include "MoveNode.hh"
#include "MoveGuard.hh"
#include "TemplateSlot.hh"
#include "Guard.hh"
#include "OperationPool.hh"
#include "Operand.hh"

using namespace TTAMachine;
/**
 * Constructor for using this generic class directly.
 */
MachineConnectivityCheck::MachineConnectivityCheck() : 
    MachineCheck("Common helper functionality for connectivity checks.") {
}

/**
 * Destructor.
 */
MachineConnectivityCheck::~MachineConnectivityCheck() {
}

/**
 * Constructor for deriving.
 *
 * @param shortDesc The short description of the check.
 */
MachineConnectivityCheck::MachineConnectivityCheck(
    const std::string& shortDesc_) : MachineCheck(shortDesc_) {
}


bool
MachineConnectivityCheck::isConnected(
    PortSet sourcePorts,
    PortSet destinationPorts,
    const Guard* guard) {
    for (PortSet::iterator i =
             sourcePorts.begin();
         i != sourcePorts.end(); i++) {
        const TTAMachine::Port& sport = **i;
        for (PortSet::iterator j =
                 destinationPorts.begin();
             j != destinationPorts.end(); j++) {
            if (isConnected(sport, **j, guard)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Dummy implementation for the pure virtual method MachineCheck::check().
 *
 * The implementation is needed for generation of Python bindings, as
 * Boost.Python cannot create instances of abstract base classes.
 */

bool
MachineConnectivityCheck::check(const TTAMachine::Machine&,
				MachineCheckResults&) const {
    assert(0);
}

/**
 * Checks whether there is a connection between two ports.
 *
 * @param sourcePort The source (rf/fu) port.
 * @param destinationPort The destination (rf/fu) port.
 * @return True if there is at least one path between the given ports.
 */
bool
MachineConnectivityCheck::isConnected(
    const TTAMachine::Port& sourcePort,
    const TTAMachine::Port& destinationPort,
    const Guard* guard) {

    // TODO: replace this cache's second value(bool) with set of buses.
    PortPortBoolMap::const_iterator
        i = portPortCache_.find(PortPortPair(&sourcePort, &destinationPort));
    if (i != portPortCache_.end()) {
        if (i->second == false || guard == NULL) {
            return i->second;
        }
    }
    std::set<const TTAMachine::Bus*> sourceBuses;
    MachineConnectivityCheck::appendConnectedDestinationBuses(
        sourcePort, sourceBuses);

    std::set<const TTAMachine::Bus*> destinationBuses;
    MachineConnectivityCheck::appendConnectedSourceBuses(
        destinationPort, destinationBuses);

    std::set<const TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(sourceBuses, destinationBuses, sharedBuses);
    if (sharedBuses.size() > 0) {
        portPortCache_[PortPortPair(&sourcePort,&destinationPort)] = true;

        if (guard == NULL) {
            return true;
        }

        for (std::set<const TTAMachine::Bus*>::iterator i = sharedBuses.begin();
             i != sharedBuses.end(); i++) {
            if ((*i)->hasGuard(*guard)) {
                return true;
            }
        }
             
        return false; // bus found but lacks the guards
    } else {
        portPortCache_[PortPortPair(&sourcePort,&destinationPort)] = false;
        return false;
    }
}

/**
 * Checks whether an immediate with given width can be transported to the
 * destination register file.
 *
 * @param destRF The destination RF.
 * @param immediate The immediate to transport.
 * @return True if there is at least one path between the given ports.
 */
bool
MachineConnectivityCheck::canTransportImmediate(
    const TTAProgram::TerminalImmediate& immediate,
    const TTAMachine::BaseRegisterFile& destRF,
    const Guard* guard) {

    std::set<const TTAMachine::Bus*> buses;
    MachineConnectivityCheck::appendConnectedSourceBuses(destRF, buses);

    for (auto bus : buses) {
        int requiredBits = 
            MachineConnectivityCheck::requiredImmediateWidth(
                bus->signExtends(), immediate, *destRF.machine());
        if (bus->immediateWidth() < requiredBits) {
            continue;
        }
        
        if (guard == NULL) {
            return true;
        } else {
            if (bus->hasGuard(*guard)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Checks whether an immediate with given width can be transported to the
 * destination port.
 *
 * @param immediate The immediate to transport.
 * @param destinationPort The destination port.
 * @return True if there is at least one path between the given ports.
 */
bool
MachineConnectivityCheck::canTransportImmediate(
    const TTAProgram::TerminalImmediate& immediate,
    const TTAMachine::Port& destinationPort,
    const Guard* guard) {

    std::set<const TTAMachine::Bus*> buses;
    MachineConnectivityCheck::appendConnectedSourceBuses(
        destinationPort, buses);

    for (auto i = buses.begin(); i != buses.end(); ++i) {
        const TTAMachine::Bus& bus = **i;

        if (!canTransportImmediate(immediate, bus)) {
            continue;
        }
        if (guard == NULL) {
            return true;
        } else {
            if (bus.hasGuard(*guard)) {
                return true;
            }
        }
    }
    return false;
}

bool 
MachineConnectivityCheck::canTransportImmediate(
    const TTAProgram::TerminalImmediate& immediate,
    PortSet destinationPorts,
    const Guard* guard) {

    for (PortSet::iterator i =
             destinationPorts.begin();
         i != destinationPorts.end(); i++) {
        if (canTransportImmediate(immediate, **i, guard)) {
            return true;
        }
    }
    return false;
}

/**
 * Checks whether an immediate with given width can be transported on the
 * bus.
 *
 * @param immediate The immediate to transport.
 * @param bus The bus.
 * @return True if the bus can transport the immediate as inline.
 */
bool
MachineConnectivityCheck::canTransportImmediate(
    const TTAProgram::TerminalImmediate& immediate,
    const TTAMachine::Bus& bus) {

    int requiredBits =
	MachineConnectivityCheck::requiredImmediateWidth(
	    bus.signExtends(), immediate, *bus.machine());
    if (bus.immediateWidth() >= requiredBits) {
        return true;
    } else {
        return false;
    }
}

/**
 * Checks whether there is a connection from some outport port of a 
 * register file or immediate unit into the specified port.
 *
 * @param sourceRF Source RF/Immediate unit.
 * @param destPort The destination port.
 * @return True if there is at least one connection from any of the output
 * ports of the source RF/Imm unit into the destination port.
 */
bool
MachineConnectivityCheck::isConnected(
    const TTAMachine::BaseRegisterFile& sourceRF,
    const TTAMachine::Port& destPort) {

    RfPortBoolMap::const_iterator
        i = rfPortCache_.find(RfPortPair(&sourceRF, &destPort));
    if (i != rfPortCache_.end()) {
        return i->second;
    }    
    std::set<const TTAMachine::Bus*> destBuses = connectedSourceBuses(destPort);
    std::set<const TTAMachine::Bus*> srcBuses;

    for (int i = 0; i < sourceRF.portCount(); i++) {
        const TTAMachine::Port& port = *sourceRF.port(i);
        if (port.outputSocket() != NULL) {
            appendConnectedDestinationBuses(port, srcBuses);
        }
    }

    std::set<const TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(
        srcBuses, destBuses, sharedBuses);
    if (sharedBuses.size() > 0) {
        rfPortCache_[RfPortPair(&sourceRF,&destPort)] = true;
        return true;
    } else {
        rfPortCache_[RfPortPair(&sourceRF,&destPort)] = false;
        return false;
    }
}

/**
 * Checks whether there is a connection from some outport port of a 
 * register file or immediate unit into some input port of the given
 * target register file or immediate unit.
 *
 * @param sourceRF Source RF/Immediate unit.
 * @param destRF Source RF/Immediate unit.
 * @return True if there is at least one connection from any of the output
 * ports of the source RF/Imm unit into the destination RF.
 */
bool
MachineConnectivityCheck::isConnected(
    const TTAMachine::BaseRegisterFile& sourceRF,
    const TTAMachine::BaseRegisterFile& destRF,
    const TTAMachine::Guard* guard) {
    
    RfRfBoolMap::const_iterator
        i = rfRfCache_.find(RfRfPair(&sourceRF, &destRF));
    if (i != rfRfCache_.end()) {
        if (i->second == false || guard == NULL) {
            return i->second;
        }
    }
    std::set<const TTAMachine::Bus*> srcBuses;
    appendConnectedDestinationBuses(sourceRF, srcBuses);

    std::set<const TTAMachine::Bus*> dstBuses;
    appendConnectedSourceBuses(destRF, dstBuses);

    std::set<const TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(srcBuses, dstBuses, sharedBuses);
    if (sharedBuses.size() > 0) {
        rfRfCache_[RfRfPair(&sourceRF,&destRF)] = true;
        if (guard == NULL) {
            return true;
        }

        for (std::set<const TTAMachine::Bus*>::iterator i = sharedBuses.begin();
             i != sharedBuses.end(); i++) {
            if ((*i)->hasGuard(*guard)) {
                return true;
            }
        }
        return false; // bus found but lacks the guards
    } else {
        rfRfCache_[RfRfPair(&sourceRF,&destRF)] = false;
        return false;
    }
}

/**
 * Checks whether there is a connection from some outport port of a 
 * register file or immediate unit into some input port of the given
 * target function unit.
 *
 * @param sourceRF Source RF/Immediate unit.
 * @param destFU Destination FU.
 * @return True if there is at least one connection from any of the output
 * ports of the source RF/Imm unit into the destination FU.
 */
bool
MachineConnectivityCheck::isConnected(
    const TTAMachine::BaseRegisterFile& sourceRF,
    const TTAMachine::FunctionUnit& destFU) {
    
    std::set<const TTAMachine::Bus*> srcBuses;
    appendConnectedDestinationBuses(sourceRF, srcBuses);

    std::set<const TTAMachine::Bus*> dstBuses;
    appendConnectedSourceBuses(destFU, dstBuses);

    std::set<const TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(srcBuses, dstBuses, sharedBuses);
    return (sharedBuses.size() > 0);
}


/**
 * Checks whether there is a connection from specified port into some
 * input port of a register file.
 *
 * @param sourcePort The source port.
 * @param destRF Destination register file.
 * @return True if there exists at least one connection from source to 
 * any input port of destination register file.
 */
bool 
MachineConnectivityCheck::isConnected(
    const TTAMachine::Port& sourcePort,
    const TTAMachine::RegisterFile& destRF) {

    PortRfBoolMap::const_iterator
        i = portRfCache_.find(PortRfPair(&sourcePort, &destRF));
    if (i != portRfCache_.end()) {
        return i->second;
    }

    std::set<const TTAMachine::Bus*> sourceBuses =
        connectedDestinationBuses(sourcePort);
    std::set<const TTAMachine::Bus*> destBuses;

    for (int i = 0; i < destRF.portCount(); i++) {
        const TTAMachine::Port& port = *destRF.port(i);
        if (port.inputSocket() != NULL) {
            appendConnectedSourceBuses(port, destBuses);
        }
    }
    
    std::set<const TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(sourceBuses, destBuses, sharedBuses);

    if (sharedBuses.size() > 0) {
        portRfCache_[PortRfPair(&sourcePort,&destRF)] = true;
        return true;
    } else {
        portRfCache_[PortRfPair(&sourcePort,&destRF)] = false;
        return false;
    }
}

/**
 * Checks whether the two RFs are connected to the exact same buses.
 */
bool
MachineConnectivityCheck::isEquallyConnected(
    const TTAMachine::BaseRegisterFile& RF1,
    const TTAMachine::BaseRegisterFile& RF2) {

    std::set<const TTAMachine::Bus*> dstBuses1, dstBuses2;
    appendConnectedDestinationBuses(RF1, dstBuses1);
    appendConnectedDestinationBuses(RF2, dstBuses2);

    std::set<const TTAMachine::Bus*> srcBuses1, srcBuses2;
    appendConnectedSourceBuses(RF1, srcBuses1);
    appendConnectedSourceBuses(RF2, srcBuses2);

    return dstBuses1 == dstBuses2 && srcBuses1 == srcBuses2;
}

bool
MachineConnectivityCheck::isPortApplicableToWidths(
    const TTAMachine::Port& port, std::set<int> widths) {
    auto fup = dynamic_cast<const FUPort*>(&port);
    if (fup == nullptr) {
        return false;
    }
    auto fu = dynamic_cast<const FunctionUnit*>(port.parentUnit());
    if (fu == nullptr) {
        return false;
    }

    OperationPool opPool;

    for (int i = 0; i < fu->operationCount(); i++) {
        auto hwop = fu->operation(i);
        if (!hwop->isBound(*fup))
            continue;

        int opIndex = hwop->io(*fup);
        int oprWidth = operandWidth(*hwop, opIndex);
        if (AssocTools::containsKey(widths, oprWidth)) {
            return true;
        }
    }
    return false;
}

/**
 * Checks whether there is a connection from the given Register file
 * or Immediate unit to all FU's and control unit of the machine.
 *
 * It both this and toRfConnected() returns true, 
 * then this FU can be used as the only register file
 * to be used for reduced connectivity-register copying.
 *
 * @param brf Register file or immediate unit being checked.
 * @return true if this RF can write to all ports af all FU's.
 */
bool 
MachineConnectivityCheck::fromRfConnected(
    const TTAMachine::BaseRegisterFile& brf) {

    int width = brf.width();
    bool isVectorRegs = width > 32;
    TTAMachine::Machine& mach = *brf.machine();
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        mach.functionUnitNavigator();
    

    std::set<const TTAMachine::Bus*> rfBuses;

    for (int i = 0; i < brf.portCount(); i++) {
        const TTAMachine::Port& port = *brf.port(i);
        if (port.outputSocket() != NULL) {
            appendConnectedDestinationBuses(port, rfBuses);
        }
    }

    std::set<int> widths;
    widths.insert(brf.width());

    auto widthsExt = widths;
    if (width == 32) {
        widthsExt.insert(1);
        widthsExt.insert(8);
        widthsExt.insert(16);
    }
    if (width == 1) {
        widthsExt.insert(32);
    }

    // check connections to function units
    for (auto fu: fuNav) {
        for (int j = 0; j < fu->portCount(); j++ ) {
            Port& port = *fu->port(j);
            // connections from RF to FU's
            if (port.inputSocket() != NULL &&
                isPortApplicableToWidths(port, widthsExt)) {
                std::set<const TTAMachine::Bus*> sharedBuses;
                SetTools::intersection(
                    rfBuses, connectedSourceBuses(port), sharedBuses);
                if (sharedBuses.size() == 0) {
                    return false;
                }
            }
        }
    } 

    // no need to transfer data from vector regs to control unit.
    if (isVectorRegs) {
        return true;
    }

    // check connections to control unit
    TTAMachine::ControlUnit& cu = *mach.controlUnit();
    for (int i = 0; i < cu.portCount(); i++ ) {
        Port& port = *cu.port(i);

        // connections from RF to CU
        if (port.inputSocket() != NULL &&
            isPortApplicableToWidths(port, widths)) {
            std::set<const TTAMachine::Bus*> sharedBuses;
            SetTools::intersection(
                rfBuses, connectedSourceBuses(port), sharedBuses);
            if (sharedBuses.size() == 0) {
                return false;
            }
        }
    }
    return true;
}

/** 
 * Checks if an RF is connected to all other units
 *
 * It this returns true, 
 * then this FU can be used as the only register file
 * to be used for reduced connectivity-register copying.
 *
 * @param rf Register file to be checked.
 * @return Returns true if the given RF is connected to all ports in all FU's
 * and at least to some output port in all immediate units.
 */
bool 
MachineConnectivityCheck::rfConnected(
    const TTAMachine::RegisterFile& rf) {

    return fromRfConnected(rf) && toRfConnected(rf);
}

std::pair<int, int>
MachineConnectivityCheck::immBits(
    const TTAMachine::Machine& mach) {
    std::pair<int, int> rv;
    std::set<const TTAMachine::Bus*> buses;
    for (auto b: mach.busNavigator()) {
        buses.insert(b);
    }
    shortImmBits(buses, rv);
    return rv;
}

void
MachineConnectivityCheck::shortImmBits(
    std::set<const TTAMachine::Bus*>& buses, std::pair<int, int>& immBits) {

    for (auto b: buses) {
        int w = b->immediateWidth();
        if (b->signExtends()) {
            immBits.first = std::max(immBits.first, w);
        } else {
            immBits.second = std::max(immBits.second,w);
        }
    }
}

std::pair<int, int>
MachineConnectivityCheck::immBits(
    const TTAMachine::RegisterFile& rf) {
    auto mach = rf.machine();
    std::pair<int, int> rv(0,0);
    std::set<const TTAMachine::Bus*> rfBuses;

    if (mach == nullptr) {
        return rv;
    }

    for (int i = 0; i < rf.portCount(); i++) {
        auto port = rf.port(i);
        if (port->inputSocket() != NULL) {
            appendConnectedSourceBuses(*port, rfBuses);
        }
    }
    shortImmBits(rfBuses, rv);

    // then check LIMM connections.
    for (auto iu: mach->immediateUnitNavigator()) {
        int w = iu->width();
        for (int j = 0; j < iu->portCount(); j++ ) {
            Port& port = *iu->port(j);
            if (port.outputSocket() == nullptr)
                continue;

            std::set<const TTAMachine::Bus*> sharedBuses;
            SetTools::intersection(
                rfBuses, connectedDestinationBuses(port),sharedBuses);
            // TODO: check bus widths.
            if (sharedBuses.size() != 0) {
                if (iu->signExtends()) {
                    rv.first = std::max(rv.first, w);
                } else {
                    rv.second = std::max(rv.second, w);
                }
            }
        }
    }
    return rv;
}

/**
 * Checks whether there is a connection to the given Register file
 * from all FU's and control unit of the machine.
 *
 * It both this and fromRfConnected() returns true, 
 * then this FU can be used as the only register file
 * to be used for reduced connectivity-register copying.
 *
 * @param brf Register file or immediate unit being checked.
 * @return true if this RF can write to all ports af all FU's.
 */
bool
MachineConnectivityCheck::toRfConnected(
    const TTAMachine::RegisterFile& rf) {

    int width = rf.width();
    bool isVectorRF = width > 32;
    TTAMachine::Machine& mach = *rf.machine();
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        mach.functionUnitNavigator();
    TTAMachine::Machine::ImmediateUnitNavigator iuNav = 
        mach.immediateUnitNavigator();

    std::set<const TTAMachine::Bus*> rfBuses;

    std::set<int> widths;
    widths.insert(rf.width());
    if (width == 32) {
        widths.insert(1);
        widths.insert(16); // Needed for half-floats?

    }
    if (width == 1) {
        widths.insert(32);
    }

    for (int i = 0; i < rf.portCount(); i++) {
        const TTAMachine::Port& port = *rf.port(i);
        if (port.inputSocket() != NULL) {
            appendConnectedSourceBuses(port, rfBuses);
        }
    }

    // check connections from function units
    for (auto fu: fuNav) {
        for (int j = 0; j < fu->portCount(); j++ ) {
            Port& port = *fu->port(j);
            // connections from FU to RF
            if (port.outputSocket() != NULL &&
                isPortApplicableToWidths(port, widths) &&
                port.width() >= width) {
                std::set<const TTAMachine::Bus*> sharedBuses;
                SetTools::intersection(
                    rfBuses, connectedDestinationBuses(port), sharedBuses);
                if (sharedBuses.size() == 0) {
                    return false;
                }
            }
        }
    } 

    // no need to check for imms or cu connections for vector RFs
    if (isVectorRF) {
        return true;
    }

    // check connections from control unit
    TTAMachine::ControlUnit& cu = *mach.controlUnit();
    for (int i = 0; i < cu.portCount(); i++ ) {
        Port& port = *cu.port(i);

        // connections from CU to RF
        if (port.width() == width) {
            if (port.outputSocket() != NULL) {
                std::set<const TTAMachine::Bus*> sharedBuses;
                SetTools::intersection(
                    rfBuses, connectedDestinationBuses(port), sharedBuses);
                if (sharedBuses.size() == 0) {
                    return false;
                }
            }
        }
    }

    // if all immediates can be written to the RF?
    auto rfImmBits = immBits(rf);
    if (rfImmBits.first >= width || rfImmBits.second >= width) {
        return true;
    }

    // if there are wider imms in the adf, not ok
    auto allImmBits = immBits(*rf.machine());
    if (rfImmBits.first < allImmBits.first ||
        (rfImmBits.second < allImmBits.second &&
         (rfImmBits.first-1) < allImmBits.second)) {
        return false;
    }
    return true;
}

/**
 * Returns all buses the given port can read from.
 *
 * @param port The port to check.
 * @return The set of buses connected to the port.
 */
std::set<const TTAMachine::Bus*>
MachineConnectivityCheck::connectedSourceBuses(
    const TTAMachine::Port& port) {

    std::set<const TTAMachine::Bus*> buses;
    appendConnectedSourceBuses(port,buses);
    return buses;
}

/**
 * Returns all buses the given port can write to
 *
 * @param port The port to check.
 * @return The set of buses connected to the port.
 */
std::set<const TTAMachine::Bus*>
MachineConnectivityCheck::connectedDestinationBuses(
    const TTAMachine::Port& port) {

    std::set<const TTAMachine::Bus*> buses;
    appendConnectedDestinationBuses(port,buses);
    return buses;
}

/**
 * Appends source buses connected to a given (input) port.
 *
 * @param port The port to check
 * @param buses Set where the connected buses are appended.
 */
void
MachineConnectivityCheck::appendConnectedSourceBuses(
    const TTAMachine::Port& port, std::set<const TTAMachine::Bus*>& buses) {

    const TTAMachine::Socket* inputS = port.inputSocket();

    if (inputS != NULL) {
        for (int i = 0; i < inputS->segmentCount(); ++i) {
            buses.insert(inputS->segment(i)->parentBus());
        }
    }
}

/**
 * Appends destination buses connected to a given (output) port.
 *
 * @param port The port to check
 * @param buses Set where the connected buses are appended.
 */
void
MachineConnectivityCheck::appendConnectedDestinationBuses(
    const TTAMachine::Port& port, std::set<const TTAMachine::Bus*>& buses) {

    const TTAMachine::Socket* outputS = port.outputSocket();

    if (outputS != NULL) {
        for (int i = 0; i < outputS->segmentCount(); ++i) {
            buses.insert(outputS->segment(i)->parentBus());
        }
    }
}

/**
 * Appends source buses connected to a given unit.
 *
 * Source buses are the buses the unit can read data from.
 *
 * @param unit The unit to check
 * @param buses Set where the connected buses are appended.
 */
void
MachineConnectivityCheck::appendConnectedSourceBuses(
    const TTAMachine::Unit& unit, std::set<const TTAMachine::Bus*>& buses) {

    for (int p = 0; p < unit.portCount(); ++p) {
        const TTAMachine::Port& port = *unit.port(p);
        appendConnectedSourceBuses(port, buses);
    }
}

/**
 * Appends destination buses connected to a given unit.
 *
 * Destination buses are the buses the unit can write data to.
 *
 * @param unit The unit to check
 * @param buses Set where the connected buses are appended.
 */
void
MachineConnectivityCheck::appendConnectedDestinationBuses(
    const TTAMachine::Unit& unit, std::set<const TTAMachine::Bus*>& buses) {

    for (int p = 0; p < unit.portCount(); ++p) {
        appendConnectedDestinationBuses(*unit.port(p), buses);
    }
}

/**
 * Checks if given RF is connected to differently connected RFs,
 * to know if we need to reserve temp registers for transfers between
 * the two RFs.
 *
 * TODO: This isn't always needed: there is no need to reserve a register
 *       from an RF if it is only connected to RFs with a subset of the
 *       connectivity it offers.
 */
bool
MachineConnectivityCheck::isConnectedToDifferentlyConnectedRFs(
    const TTAMachine::RegisterFile& rf) {
    auto regNav = rf.machine()->registerFileNavigator();
    for (auto rf2: regNav) {
        if ((rf2 != &rf) &&
            (isConnected(rf,*rf2) || isConnected(*rf2,rf)) &&
            !isEquallyConnected(rf, *rf2) &&
            ((rf.width() <= 32 && rf2->width() <= 32) ||
             (rf.width() == rf2->width()))) {
            return true;
        }
    }
    return false;
}

int MachineConnectivityCheck::operandWidth(
    const TTAMachine::HWOperation& hwop, int index) {
    OperationPool opPool;
    Operation& op = opPool.operation(hwop.name().c_str());
    if (&op == &NullOperation::instance()) {
        TCEString msg = "ADF has unknown operation: "; msg << hwop.name();
        throw Exception(__FILE__, __LINE__, __func__, msg);
    }
    Operand& operand = op.operand(index);
    return operand.width();
}


std::set<const RegisterFile*>
MachineConnectivityCheck::needRegCopiesDueReadPortConflicts(
    const TTAMachine::Machine& machine) {

    std::map<int, int> noRegInputCount;
    auto fuNav = machine.functionUnitNavigator();
    auto regNav = machine.registerFileNavigator();

    std::set<const RegisterFile*> rv;

    for (auto fu : fuNav) {
        for (int j = 0; j < fu->operationCount(); j++) {
            auto hwop = fu->operation(j);
            std::map<int, int> myNoRegInputCount;
            for (int k = 1; k <= hwop->operandCount(); k++) {
                auto p = hwop->port(k);
                if (p->inputSocket() != NULL &&
                    (p->noRegister() || p->isTriggering())) {
                    int w = operandWidth(*hwop, k);
                    myNoRegInputCount[w]++;
                    if (w == 1) {
                        myNoRegInputCount[32]++;
                    } else if (w == 32) {
                        myNoRegInputCount[1]++;
                    }
                }
            }

            for(auto mw : myNoRegInputCount) {
                int w = mw.first;
                noRegInputCount[w] = std::max(noRegInputCount[w], mw.second);
            }
        }
    }
    for (auto rf: regNav) {
        // one read needed for trigger so <= instead of <
        if (rf->maxReads() < noRegInputCount[rf->width()]) {
            rv.insert(rf);
        }
    }
    return rv;
}

bool
MachineConnectivityCheck::needsRegisterCopiesDueImmediateOperands(
    const TTAMachine::Machine& mach) {

    std::set<int> scalarWidths = {1,32};
    for (auto fu: mach.functionUnitNavigator()) {
        for (int j = 0; j < fu->portCount(); j++ ) {
            Port& port = *fu->port(j);
            // connections from RF to FU's
            if (port.inputSocket() != NULL &&
                isPortApplicableToWidths(port, scalarWidths)) {
                if (!canWriteAllImmediates(port)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool MachineConnectivityCheck::raConnected(const TTAMachine::Machine& machine) {

    static bool spammed = false;
    // check connection from RA to jump for fn return.
    ControlUnit& cu = *machine.controlUnit();
    SpecialRegisterPort& ra = *cu.returnAddressPort();
    if (cu.hasOperation("jump")) {
        auto jumpOp = cu.operation("jump");
        auto port = jumpOp->port(1);
        if (!isConnected(ra, *port)) {
            if (Application::verboseLevel() > 0 && !spammed) {
                std::cout << "Reserving registers for temp reg use because "
                          << "a connection is missing between the RA port "
                          << "and the address port of jump operation." << std::endl;
                spammed = true;
            }
            return false;
        }
    }

    // check that RA can be loaded and stored
    TCEString ldOp = machine.isLittleEndian() ? "LD32" : "LDW";
    TCEString stOp = machine.isLittleEndian() ? "ST32" : "STW";
    bool hasLoad = false;
    bool hasStore = false;
    bool raConnectedToLoad = false;
    bool raConnectedToStore = false;
    for (auto fu: machine.functionUnitNavigator()) {
        if (!fu->hasAddressSpace() ||
            !fu->addressSpace()->hasNumericalId(0)) {
            continue;
        }
        if (fu->hasOperation(ldOp)) {
            hasLoad = true;
            auto ldhwop = fu->operation(ldOp);
            auto port = ldhwop->port(2);
            if (isConnected(*port, ra)) {
                raConnectedToLoad = true;
            }
        }

        if (fu->hasOperation(stOp)) {
            hasStore = true;
            auto sthwop = fu->operation(stOp);
            auto port = sthwop->port(2);
            if (isConnected(ra, *port)) {
                raConnectedToStore = true;
            }
        }
    }

    if ((raConnectedToLoad || !hasLoad) &&
        (raConnectedToStore || !hasStore)) {
        return true;
    } else {
        if (Application::verboseLevel() > 0 && !spammed) {
            std::cout << "Reserving registers for temp reg use because "
                      << "a connection is missing between the RA port "
                      << "and the LSU." << std::endl;
            spammed = true;
        }
        return false;
    }
}

/**
 * Gets all register files needed for limited connectivity temp registers
 *
 * @param machine machine we are checking
 * @return vector of registerfiles whose last register is to be used for 
 */
std::set<const RegisterFile*, MachinePart::Comparator>
MachineConnectivityCheck::tempRegisterFiles(
    const TTAMachine::Machine& machine) {
    static bool spammed = false;

    std::set<const RegisterFile*, TTAMachine::MachinePart::Comparator> rfs;

    auto regNav = machine.registerFileNavigator();
    auto iuNav = machine.immediateUnitNavigator();
    auto fuNav = machine.functionUnitNavigator();

    auto reducedConnRfs = needRegCopiesDueReadPortConflicts(machine);
    std::set<int> portConflictWidths;
    for (auto rf: reducedConnRfs) {
        int w = rf->width();
        portConflictWidths.insert(w);
    }

    bool portConflicts = !reducedConnRfs.empty();
    bool allConnected = true;
    if (portConflicts) {
        if (Application::verboseLevel() > 0 && !spammed) {
            std::cout << "Reserving registers for temp reg use because " <<
                "of possible RF port conflicts; There are operations " <<
                "on registerless FUs with more operations than there are " <<
                "read ports on some RFs." << std::endl;
            spammed = true;
        }
        allConnected = false;
    }
    int widestRFWidth = 0;

    std::set<int> lackingConnectivityWidths;
    std::set<const TTAMachine::RegisterFile*> allConnectedRFs;
    std::map<int, const RegisterFile*> priorityConnectedRFs;
    std::map<int, int> regCounts;
    std::map<int, int> regFileCounts;

    // may need temp reg copies because immediate operands cannto be
    // transferred to all operations directly.
    if (allConnected && needsRegisterCopiesDueImmediateOperands(machine)) {
        lackingConnectivityWidths.insert(32);
        allConnected = false;
        if (Application::verboseLevel() > 0 && !spammed) {
            std::cout << "Reserving registers for temp reg use because " <<
                "all immediate operands are not possible to transfer " <<
                "directly. This reduces the number of registers available " <<
                "for storing usable values." << std::endl;
            spammed = true;
        }
    }

    for (auto rf: regNav) {
        int w = rf->width();
        if (w > widestRFWidth) {
            widestRFWidth = w;
        }
        regCounts[w] += rf->size();
        regFileCounts[w]++;
    }

    for (auto rf: regNav) {
        int width = rf->width();
        if (!rfConnected(*rf)) {
            reducedConnRfs.insert(rf);
            allConnected = false;
            lackingConnectivityWidths.insert(width);
            if (Application::verboseLevel() > 0 && !spammed) {
                std::cout << "Reserving registers for temp reg use because RF: "
                          << rf->name() << " has reduced connectivity to FUs or "
                          << "immediates." << std::endl;
                spammed = true;
            }
        } else {
            allConnectedRFs.insert(rf);
            // we have at least on RF connected to everywhere so use it.
            // but only if it's wide enough (as wide as the widest RF)
            auto connectedRF = priorityConnectedRFs[width];

            // ra/imm/narrowed connectivity for 32bit
            if ((width <= 32 || isConnectedToDifferentlyConnectedRFs(*rf)) &&
                 (connectedRF == nullptr ||
                rf->maxReads() * rf->maxWrites() >
                  connectedRF->maxReads() * connectedRF->maxWrites())) {
                priorityConnectedRFs[width] = rf;
            }
        }
    }

    if (!raConnected(machine)) {
        allConnected = false;
        lackingConnectivityWidths.insert(32);
    }

    if (allConnected) {
        return rfs;
    }

    bool needNextBigger = false;
    if (AssocTools::containsKey(lackingConnectivityWidths, 1)) {
        if (priorityConnectedRFs[1] != nullptr &&
            regCounts[1] > 2 &&
            regFileCounts[1] > 1) {
            rfs.insert(priorityConnectedRFs[1]);
        } else {
            needNextBigger = true;
        }
    }
    // 32-bit.
    if (needNextBigger ||
        AssocTools::containsKey(lackingConnectivityWidths, 32) ||
        portConflicts) {
        if (!portConflicts && priorityConnectedRFs[32] != nullptr) {
            rfs.insert(priorityConnectedRFs[32]);
        } else {
            for (auto rf: regNav) {
                if (rf->width() == 32 &&
                    (isConnectedToDifferentlyConnectedRFs(*rf) ||
                     portConflicts)) {
                    rfs.insert(rf);
                }
            }
        }
    }

    // vector.
    for (int w = 64; w <= widestRFWidth; w*=2) {
        if (AssocTools::containsKey(lackingConnectivityWidths, w) ||
            portConflicts) {
            if (!portConflicts && priorityConnectedRFs[w] != nullptr) {
                rfs.insert(priorityConnectedRFs[w]);
            } else {
                for (auto rf: regNav) {
                    if (rf->width() == w &&
                        (isConnectedToDifferentlyConnectedRFs(*rf) ||
                         portConflicts)) {
                        rfs.insert(rf);
                    }
                }
            }
        }
    }
    
    return rfs;
}


/** 
 * Checks if there is a way to write immediate directly from bus or from immu
 * to the given port
 * 
 * @param port Port where to check immediate write ability.
 */    
bool
MachineConnectivityCheck::canWriteAllImmediates(Port& destPort) {
    /** First check if there is a bus that can transfer the immediates */
    int portWidth = destPort.width();
    int sextImm = 0;
    int zextImm = 0;

    Socket& socket = *destPort.inputSocket();
    
    // check immediates from buses
    for (int i = 0; i < socket.segmentCount(); ++i) {
        auto bus = socket.segment(i)->parentBus();
        int immw = bus->immediateWidth();
        if (bus->signExtends()) {
            sextImm = std::max(immw, sextImm);
        } else {
            zextImm = std::max(immw, zextImm);
        }
        if (immw >= portWidth) {
            return true;
        }
    }

    // then check directly connected imm units
    const TTAMachine::Machine& mach = *destPort.parentUnit()->machine();
    TTAMachine::Machine::ImmediateUnitNavigator iuNav = 
        mach.immediateUnitNavigator();
    
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit& iu = *iuNav.item(i);
        int immw = iu.width();
        if (isConnected(iu, destPort)) {
            if (iu.signExtends()) {
                sextImm = std::max(immw, sextImm);
            } else {
                zextImm = std::max(immw, zextImm);
            }
        }
    }

    auto widestImms = immBits(mach);
    // wide zext imm not needed if has one bit wider sext imm.
    if (widestImms.first > sextImm ||
        (widestImms.second > zextImm && (sextImm-1) < widestImms.second)) {
        return false;
    }
    return true;
}

/**
 * Returns the bit width the immediate requires.
 *
 * @param signExtension Whether sign extension is used.
 * @param source The immediate terminal.
 * @return Bitwidth required for the immediate.
 */
int
MachineConnectivityCheck::requiredImmediateWidth(
    bool signExtension,
    const TTAProgram::TerminalImmediate& source,
    const TTAMachine::Machine& mach) {

    if (source.isCodeSymbolReference()) {
        const AddressSpace& instrAS = *mach.controlUnit()->addressSpace();
        if (source.toString() == "_end") {
            AddressSpace* dataAS;
            try {
                dataAS = MachineInfo::defaultDataAddressSpace(mach);
            } catch (Exception&) {
                assert(false && "No default data address space");
            }

            return signExtension ?
                MathTools::requiredBitsSigned(dataAS->end()):
                MathTools::requiredBits(dataAS->end());
        } else {
            return signExtension ?
                MathTools::requiredBitsSigned (instrAS.end()):
                MathTools::requiredBits(instrAS.end());
        }
    }
    if (source.isInstructionAddress() || source.isBasicBlockReference()) {
        const AddressSpace& as = *mach.controlUnit()->addressSpace();
        return signExtension ? 
            MathTools::requiredBitsSigned(as.end()): 
            MathTools::requiredBits(as.end());
    }

    int bits = -1;
    if (signExtension) {
        bits =
            MathTools::requiredBitsSigned(source.value().sLongWordValue());
    } else if (!signExtension) {
        bits =
            MathTools::requiredBits(source.value().uLongWordValue());
    } 

    return bits;
}

bool MachineConnectivityCheck::busConnectedToPort(
    const TTAMachine::Bus& bus, const TTAMachine::Port& port) {
    const TTAMachine::Socket* inputS = port.inputSocket();
    if (inputS != NULL) {
        for (int i = 0; i < inputS->segmentCount(); ++i) {
            if (inputS->segment(i)->parentBus() == &bus) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Returns true if bus is connected to the RF's any writing port.
 */
bool MachineConnectivityCheck::busConnectedToRF(
    const TTAMachine::Bus& bus, const TTAMachine::Unit& destRF) {
    for (int i = 0; i < destRF.portCount(); i++) {
        const TTAMachine::Port& port = *destRF.port(i);
        if (MachineConnectivityCheck::busConnectedToPort(bus, port)) {
            return true;
        }
    }
    return false;
}


bool
MachineConnectivityCheck::busConnectedToFU(
    const TTAMachine::Bus& bus, const FunctionUnit& fu, 
    const TCEString& opName, int opIndex) {

    if (fu.hasOperationLowercase(opName)) {
        const TTAMachine::HWOperation& hwop = 
            *fu.operationLowercase(opName);
        
        TTAMachine::FUPort* port = hwop.port(opIndex);
        if (port == NULL){
            throw IllegalMachine(
                __FILE__, __LINE__, __func__, 
                "Target is missing operand bindings for: "
                + opName);
        }
        return MachineConnectivityCheck::busConnectedToPort(bus, *port);
    }
    return false;
}


bool 
MachineConnectivityCheck::busConnectedToAnyFU(
    const TTAMachine::Bus& bus, const MoveNode& moveNode) {

    const TTAProgram::Move& move = moveNode.move();
    TTAProgram::TerminalFUPort* tfp = 
        static_cast<TTAProgram::TerminalFUPort*>(&move.destination());
    int opIndex = tfp->operationIndex();

    ProgramOperation& po = moveNode.destinationOperation();
    auto fu = po.scheduledFU();
/*
    for (int i = 0; i < po.inputMoveCount(); i++ ) {
        MoveNode& inputNode = po.inputMove(i);
        if (inputNode.isAssigned()) {
            unit = inputNode.move().destination().port().parentUnit();
            break;
        }
    }
    if (unit == NULL) { // goto over this would have been better
        for (int i = 0; i < po.outputMoveCount(); i++ ) {
            MoveNode& outputNode = po.outputMove(i);
            if (outputNode.isAssigned()) {
                if (outputNode.isSourceOperation() &&
                    &outputNode.sourceOperation() == &po) {
                    unit = outputNode.move().source().port().parentUnit();
                    break;
                } else {
                    assert (!outputNode.move().isUnconditional());
                    assert (&outputNode.guardOperation() == &po);
                    const TTAMachine::Guard& guard =
                        outputNode.move().guard().guard();
                    const TTAMachine::PortGuard* pg =
                        dynamic_cast<const TTAMachine::PortGuard*>(&guard);
                    assert(pg); // todo: throw?
                    unit = pg->port()->parentUnit();
                    break;
                }
            }
        }
    }
*/

    Operation& op = move.destination().hintOperation();
    TCEString opName = StringTools::stringToLower(op.name());

    if (fu != NULL) {
//        const FunctionUnit* fu = dynamic_cast<const FunctionUnit*>(unit);
        assert(fu != NULL);
        return busConnectedToFU(bus, *fu, opName, opIndex);
    }

    // check if the move has a candidate FU set which limits the
    // choice of FU for the node
    std::set<TCEString> candidateFUs;
    std::set<TCEString> allowedFUs;

    addAnnotatedFUs(
        candidateFUs, move, 
        TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
    addAnnotatedFUs(
        allowedFUs, move, 
        TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST);

    TTAMachine::ControlUnit* gcu = bus.machine()->controlUnit();
    if (candidateFUs.empty() || AssocTools::containsKey(
            candidateFUs,gcu->name())) {
        if (busConnectedToFU(bus, *gcu, opName, opIndex)) {
            return true;
        }
    }


    const TTAMachine::Machine::FunctionUnitNavigator& fuNav = 
        bus.machine()->functionUnitNavigator();
    for (int i = 0; i < fuNav.count(); i++) {
        TTAMachine::FunctionUnit& fu = *fuNav.item(i);
        if (!candidateFUs.empty() && !AssocTools::containsKey(
                candidateFUs,fu.name())) {
            continue;
        }
        if (!allowedFUs.empty() && !AssocTools::containsKey(
                allowedFUs,fu.name())) {
            continue;
        }
        if (busConnectedToFU(bus, fu, opName, opIndex)) {
            return true;
        }
    }
    return false;
}

bool 
MachineConnectivityCheck::busConnectedToDestination(
    const TTAMachine::Bus& bus, 
    const MoveNode& moveNode) {
    const TTAProgram::Move& move = moveNode.move();
    if (move.destination().isGPR()) {
        TTAMachine::Unit& destRF = 
            *move.destination().port().parentUnit();
        return MachineConnectivityCheck::busConnectedToRF(bus, destRF);
        
    }
    if (move.destination().isFUPort()) {
        if (move.destination().isRA()) {
            const TTAMachine::ControlUnit* gcu = bus.machine()->controlUnit();
            const TTAMachine::SpecialRegisterPort* ra = 
                gcu->returnAddressPort();
            // TODO:: machineconcctivitycheck, bus connected to port?
            if (MachineConnectivityCheck::busConnectedToPort(bus, *ra)) {
                return true;
            }
        } else {
            return MachineConnectivityCheck::busConnectedToAnyFU(
                bus, moveNode);
        }
    }
    return false;
}

int
MachineConnectivityCheck::totalConnectionCount(
    const TTAMachine::Machine& mach) {
    int totalFound = 0;
    for (int bi = 0; bi < mach.busNavigator().count(); ++bi) {
        const TTAMachine::Segment& s = 
            *mach.busNavigator().item(bi)->segment(0);
        totalFound += s.connectionCount();
    }
    return totalFound;
}


MachineConnectivityCheck::PortSet 
MachineConnectivityCheck::findPossibleDestinationPorts(
    const TTAMachine::Machine& mach, 
    const MoveNode& node) {
    PortSet res;
    if (node.isScheduled()) {
        res.insert(&node.move().destination().port());
        return res;
    }

    if (node.isDestinationOperation()) {
        TTAMachine::Machine::FunctionUnitNavigator nav = 
            mach.functionUnitNavigator();
        ProgramOperation& po = node.destinationOperation();
        std::set<TCEString> allowedFUNames;

        if (po.isAnyNodeAssigned()) {
            for (int i = 0; i < po.inputMoveCount(); i++) {
                MoveNode& mn = po.inputMove(i);
                if (mn.isScheduled()) {
                    allowedFUNames.insert(
                        mn.move().destination().port().parentUnit()->name());
                }
            }

            for (int i = 0; i < po.outputMoveCount(); i++) {
                MoveNode& mn = po.outputMove(i);
                if (mn.isScheduled()) {
                    if (mn.isSourceOperation() && &mn.sourceOperation() == &po) {
                        allowedFUNames.insert(
                            mn.move().source().port().parentUnit()->name());
                    } else {
                        if (mn.isGuardOperation() &&
                            &mn.guardOperation() == &po) {
                            const TTAMachine::Guard& guard =
                                mn.move().guard().guard();
                            const TTAMachine::PortGuard* pg =
                                dynamic_cast<const TTAMachine::PortGuard*>(&guard);
                            assert(pg); // todo: throw?
                            const TTAMachine::Unit* u = pg->port()->parentUnit();
                            allowedFUNames.insert(u->name());
                        }
                    }
                }
            }
        }

        std::set<TCEString> candidateFUs;
        std::set<TCEString> allowedFUs;
	
        addAnnotatedFUs(
            candidateFUs, node.move(), 
            TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
        addAnnotatedFUs(
            allowedFUs, node.move(),
            TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST);

        if (!candidateFUs.empty()) {
            if (allowedFUNames.empty()) {
                allowedFUNames = candidateFUs;
            } else {
                std::set<TCEString> tmp;
                SetTools::intersection(allowedFUNames, candidateFUs,tmp);
                allowedFUNames = tmp;
            }

        }
    
        if (!allowedFUs.empty()) {
            if (allowedFUNames.empty()) {
                allowedFUNames = allowedFUs;
            } else {
                std::set<TCEString> tmp;
                SetTools::intersection(allowedFUNames, allowedFUs,tmp);
                allowedFUNames = tmp;
            }
        }
        
        for (int i = 0; i <= nav.count(); i++) {
            TTAMachine::FunctionUnit* fu;
            if (i == nav.count()) { // GCU is not on fu navigator
                fu = mach.controlUnit();
            } else {
                fu = nav.item(i);
            }
            
            if (!allowedFUNames.empty() && !AssocTools::containsKey(
                    allowedFUNames, fu->name())) {
                continue;
            }
            if (fu->hasOperation(po.operation().name())) {
                TTAMachine::HWOperation* hwop = 
                    fu->operation(po.operation().name());
                res.insert(
                    hwop->port(node.move().destination().operationIndex()));
            }
        }
        return res;
    }

    if (node.move().destination().isRA()) {
        res.insert(mach.controlUnit()->returnAddressPort());
        return res;
    }

    // destination is register
    if (!node.move().destination().isGPR()) {
        std::cerr << "node should have dest as reg: " << 
            node.toString() << std::endl;
    }
    assert(node.move().destination().isGPR());
    return findWritePorts(
        *node.move().destination().port().parentUnit());
}

MachineConnectivityCheck::PortSet
MachineConnectivityCheck::findWritePorts(const TTAMachine::Unit& rf) {
    PortSet res;
    for (int i = 0; i < rf.portCount(); i++) {
        const TTAMachine::Port* port = rf.port(i);
        if (port->isInput()) {
            res.insert(port);
        }
    }
    return res;
}

MachineConnectivityCheck::PortSet
MachineConnectivityCheck::findReadPorts(const TTAMachine::Unit& rf) {
    PortSet res;
    for (int i = 0; i < rf.portCount(); i++) {
        TTAMachine::Port* port = rf.port(i);
        if (port->isOutput()) {
            res.insert(port);
        }
    }
    return res;
}

/**
 * Returns true if there is connection between the port and the bus.
 */
 bool
 MachineConnectivityCheck::isConnected(
        const TTAMachine::Bus& bus,
        const TTAMachine::Port& port) {
     std::vector<const Socket*> sockets{
         port.inputSocket(), port.outputSocket()};

     for (const Socket* socket : sockets) {
         if (socket != nullptr && socket->isConnectedTo(bus)) {
             return true;
         }
     }
     return false;
 }

 /**
  * Returns true if there is connection between the port and the bus.
  */
 bool
 MachineConnectivityCheck::isConnected(
     const TTAMachine::Port& port,
     const TTAMachine::Bus& bus) {
     return isConnected(bus, port);
 }

/**
 * Returns busses that connects the given ports.
 */
MachineConnectivityCheck::BusSet
MachineConnectivityCheck::findRoutes(
    TTAMachine::Port& port1,
    TTAMachine::Port& port2) {

    MachineConnectivityCheck::BusSet result;
    const Machine& mach = *port1.parentUnit()->machine();
    for (Bus* bus : mach.busNavigator()) {
        if (isConnected(port1, *bus) && isConnected(port2, *bus)) {
            result.insert(bus);
        }
    }
    return result;
}

MachineConnectivityCheck::PortSet
MachineConnectivityCheck::findPossibleSourcePorts(
    const TTAMachine::Machine& mach, const MoveNode& node) {
    PortSet res;
    if (node.isScheduled() && !node.isSourceConstant()) {
        res.insert(&node.move().source().port());
        return res;
    }

    if (node.isSourceOperation()) {
        TTAMachine::Machine::FunctionUnitNavigator nav = 
            mach.functionUnitNavigator();
        ProgramOperation& po = node.sourceOperation();

        std::set<TCEString> candidateFUs;
        std::set<TCEString> allowedFUs;
	
        addAnnotatedFUs(candidateFUs, node.move(), 
	    TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_SRC);
        addAnnotatedFUs(allowedFUs, node.move(),
 	    TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_SRC);

        for (int i = 0; i < nav.count(); i++) {
            TTAMachine::FunctionUnit* fu = nav.item(i);
            if (!allowedFUs.empty() && !AssocTools::containsKey(
                    allowedFUs, fu->name())) {
                continue;
            }
            if (!candidateFUs.empty() && !AssocTools::containsKey(
                    candidateFUs, fu->name())) {
                continue;
            }
            
            if (fu->hasOperation(po.operation().name())) {
                TTAMachine::HWOperation* hwop = 
                    fu->operation(po.operation().name());
                res.insert(
                    hwop->port(node.move().source().operationIndex()));
            }
        }
        return res;
    }

    if (node.isSourceVariable()) {
        // source is register
        return findReadPorts(
            *node.move().source().port().parentUnit());
    }

    // consider only long immediates here.
    if (node.isSourceConstant()) {
        TTAProgram::TerminalImmediate& imm = 
            static_cast<TTAProgram::TerminalImmediate&>(node.move().source());
        TTAMachine::Machine::ImmediateUnitNavigator nav = 
            mach.immediateUnitNavigator();
        for (int i = 0; i < nav.count(); i++) {
            TTAMachine::ImmediateUnit* iu = nav.item(i);
            if (iu->width() >= 
                MachineConnectivityCheck::requiredImmediateWidth(
                    iu->extensionMode() == Machine::SIGN, imm, mach)) {
                for (int i = 0; i < iu->portCount(); i++) {
                    TTAMachine::Port* port = iu->port(i);
                    assert(port->isOutput());
                    res.insert(port);
                }
            }
        }
        return res;
    }

    if (node.move().source().isRA()) {
        res.insert(mach.controlUnit()->returnAddressPort());
    }

    return res;
}

/**
 * 1 = can write
 * 0 = cannot write
 * -1 = can write through limm
 */
int MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
    const MoveNode& src, PortSet& destinationPorts, bool ignoreGuard) {

    int trueVal = 1;
    if (destinationPorts.empty()) {
        return false;
    }

    if (src.isSourceConstant()) {
        TTAProgram::TerminalImmediate* imm = 
            static_cast<TTAProgram::TerminalImmediate*>(
                &src.move().source());
        if (MachineConnectivityCheck::canTransportImmediate(
                *imm, destinationPorts)) {
            return true; // can transfer via short imm.
        } else {
            trueVal = -1; // mayby through LIMM?
        }
    }

    PortSet sourcePorts = findPossibleSourcePorts(
            *(*destinationPorts.begin())->parentUnit()->machine(), src);

    // TODO: Why cannot move.guard return pointer which is NULL if unconditional?
    const TTAProgram::Move& move = src.move();
    if (MachineConnectivityCheck::isConnected(
            sourcePorts, destinationPorts,
            (ignoreGuard || move.isUnconditional()) ?
            NULL : &move.guard().guard())) {
        return trueVal;
    }

    return false;
}

bool MachineConnectivityCheck::canBypass(
    const MoveNode& src, const MoveNode& user, 
    const TTAMachine::Machine& targetMachine) {

    MachineConnectivityCheck::PortSet destinationPorts = 
        MachineConnectivityCheck::findPossibleDestinationPorts(
            targetMachine, user);

    int trueVal = 1;
    if (destinationPorts.empty()) {
        return false;
    }

    if (src.isSourceConstant()) {
        TTAProgram::TerminalImmediate* imm = 
            static_cast<TTAProgram::TerminalImmediate*>(
                &src.move().source());
        if (MachineConnectivityCheck::canTransportImmediate(
                *imm, destinationPorts)) {
            return true; // can transfer via short imm.
        } else {
            trueVal = -1; // mayby through LIMM?
        }
    }

    PortSet sourcePorts = findPossibleSourcePorts(targetMachine, src);

    // TODO: Why cannot move.guard return pointer which is NULL if
    // unconditional?
    const TTAProgram::Move& userMove = user.move();
    if (MachineConnectivityCheck::isConnected(
            sourcePorts, destinationPorts, 
            userMove.isUnconditional() ? NULL : &userMove.guard().guard())) {
        return trueVal;
    }

    return false;
}


bool
MachineConnectivityCheck::canAnyPortWriteToDestination(
    PortSet& sourcePorts, const MoveNode& dest) {

    if (sourcePorts.empty()) {
        return false;
    }

    PortSet destPorts = findPossibleDestinationPorts(
            *(*sourcePorts.begin())->parentUnit()->machine(), dest);
    return MachineConnectivityCheck::isConnected(sourcePorts, destPorts);
}

bool 
MachineConnectivityCheck::canTransportMove(
    const MoveNode& moveNode,
    const TTAMachine::Machine& machine,
    bool ignoreGuard) {
    PortSet destinationPorts =
        MachineConnectivityCheck::findPossibleDestinationPorts(
            machine,moveNode);

    return MachineConnectivityCheck::canSourceWriteToAnyDestinationPort(
        moveNode, destinationPorts, ignoreGuard);
}

void
MachineConnectivityCheck::addAnnotatedFUs(
    std::set<TCEString>& candidateFUs, 
	const TTAProgram::Move& m, 
	TTAProgram::ProgramAnnotation::Id id) {
    
    const int annotationCount = m.annotationCount(id);
    for (int i = 0; i < annotationCount; ++i) {
        std::string candidateFU = m.annotation(i, id).stringValue();
        candidateFUs.insert(candidateFU);
    }
}

/* These are static */
MachineConnectivityCheck::PortPortBoolMap MachineConnectivityCheck::portPortCache_;
MachineConnectivityCheck::RfRfBoolMap MachineConnectivityCheck::rfRfCache_;
MachineConnectivityCheck::RfPortBoolMap MachineConnectivityCheck::rfPortCache_;
MachineConnectivityCheck::PortRfBoolMap MachineConnectivityCheck::portRfCache_;


bool
MachineConnectivityCheck::hasConditionalMoves(
    const TTAMachine::Machine& mach, const std::set<int>& rfWidths) {

    const Machine::RegisterFileNavigator regNav =
        mach.registerFileNavigator();

    std::set<std::pair<const RegisterFile*,int> > allGuardRegs;
    const Machine::BusNavigator& busNav = mach.busNavigator();

    // first just collect all guard registers.
    for (int bi = 0; bi < busNav.count(); ++bi) {
        Bus* bus = busNav.item(bi);
        for (int gi = 0; gi < bus->guardCount(); gi++) {
            const Guard* guard = bus->guard(gi);
            const TTAMachine::RegisterGuard* rg =
                dynamic_cast<const RegisterGuard*>(guard);
            if (rg != NULL) {
                allGuardRegs.insert(
                    std::pair<const RegisterFile*,int>(rg->registerFile(),
                                                       rg->registerIndex()));
            }
        }
    }

    if (allGuardRegs.empty()) {
        return false;
    }

    // then check for the connections.
    for (int i = 0; i < regNav.count(); i++) {
        const TTAMachine::RegisterFile* srf = regNav.item(i);
        for (int j = 0; j < regNav.count(); j++) {
            const TTAMachine::RegisterFile* drf = regNav.item(i);
            int width = drf->width();

            // if we do not care about RFs of this size
            if (!rfWidths.empty() &&
                !AssocTools::containsKey(rfWidths, width)) {
                continue;
            }

            for (std::set<std::pair<const RegisterFile*,int> >::iterator k =
                     allGuardRegs.begin(); k != allGuardRegs.end(); k++) {
                if (!isConnectedWithBothGuards(*srf, *drf, *k)) {
                    return false;
                }
            }
        }
    }
    return true;
}
/*
bool
MachineConnectivityCheck::hasConditionalOperations(
    const TTAMachine::Machine& mach) {

}
*/

/**
 * Checks whether there is a connection from some outport port of a 
 * register file or immediate unit into some input port of the given
 * target register file or immediate unit.
 *
 * @param sourceRF Source RF/Immediate unit.
 * @param destRF Source RF/Immediate unit.
 * @return True if there is at least one connection from any of the output
 * ports of the source RF/Imm unit into the destination RF.
 */
bool
MachineConnectivityCheck::isConnectedWithBothGuards(
    const TTAMachine::BaseRegisterFile& sourceRF,
    const TTAMachine::BaseRegisterFile& destRF,
    std::pair<const RegisterFile*,int> guardReg) {
    
    RfRfBoolMap::const_iterator
        i = rfRfCache_.find(RfRfPair(&sourceRF, &destRF));
    if (i != rfRfCache_.end()) {
        if (i->second == false) {
            return false;
        }
    }
    std::set<const TTAMachine::Bus*> srcBuses;
    appendConnectedDestinationBuses(sourceRF, srcBuses);

    std::set<const TTAMachine::Bus*> dstBuses;
    appendConnectedSourceBuses(destRF, dstBuses);

    std::set<const TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(srcBuses, dstBuses, sharedBuses);

    bool trueOK = false;
    bool falseOK = false;
    if (sharedBuses.size() > 0) {
        rfRfCache_[RfRfPair(&sourceRF,&destRF)] = true;
        for (auto bus: sharedBuses) {
            std::pair<bool, bool> guardsOK = hasBothGuards(bus, guardReg);
            trueOK |= guardsOK.first;
            falseOK |= guardsOK.second;
            if (trueOK && falseOK) {
                return true;
            }
        }
    }
    return false;
}

std::pair<bool,bool> MachineConnectivityCheck::hasBothGuards(
    const TTAMachine::Bus* bus, std::pair<const RegisterFile*,int> guardReg) {
    bool trueOK = false;
    bool falseOK = false;

    for (int gi = 0; gi < bus->guardCount(); gi++) {
        const TTAMachine::Guard* guard = bus->guard(gi);
        const TTAMachine::RegisterGuard* rg =
            dynamic_cast<const TTAMachine::RegisterGuard*>(guard);
        if (rg != NULL) {
            if (rg->registerFile() == guardReg.first &&
                rg->registerIndex() == guardReg.second) {
                if (rg->isInverted()) {
                    falseOK = true;
                } else {
                    trueOK = true;
                }
                if (falseOK && trueOK) {
                    return std::pair<bool, bool>(true, true);
                }
            }
        }
    }
    return std::pair<bool, bool>(trueOK, falseOK);
}

int MachineConnectivityCheck::maxLIMMCount(
    const TTAMachine::Machine& targetMachine) {
    auto iuNav = targetMachine.immediateUnitNavigator();
    int limmCount = 0;
    for (auto iu : iuNav) {
        limmCount += iu->maxReads();
    }
    return limmCount;
}

int MachineConnectivityCheck::maxSIMMCount(
    const TTAMachine::Machine& targetMachine) {

    auto busNav = targetMachine.busNavigator();
    int simmCount = 0;
    for (auto bus : busNav) {
        if (bus->immediateWidth() > 0) {
            simmCount++;
        }
    }
    return simmCount;
}

/**
 * Find FU which has copy op that can be used to schedule this move.
 *
 * Prioritizes FUs which can schedule the move directly with one
 * copy op. If none found, then fives FU which has copy op and
 * can recursively be used with later copy.
 * If none found at all, returns empty set.
 */
MachineConnectivityCheck::FUSet MachineConnectivityCheck::copyOpFUs(
    const TTAMachine::Machine& mach,
    const MoveNode& mn) {

    // with these, single copy op is enough
    FUSet suitableFUs;
    // with these, have to use multiple copy ops or copy+regcopy
    FUSet partiallySuitableFUs;

    std::string opName = "COPY";
    const TTAProgram::Move& move = mn.move();
    auto destinationPorts = findPossibleDestinationPorts(mach, mn);

    for (auto fu: mach.functionUnitNavigator()) {
        PortSet copyOutPorts;
        PortSet copyTriggerPorts;
        if (fu->hasOperation(opName)) {
            TTAMachine::HWOperation* hwop = fu->operation(opName);
            copyOutPorts.insert(hwop->port(2));
            copyTriggerPorts.insert(hwop->port(1));
            if (MachineConnectivityCheck::isConnected(
                    copyOutPorts, destinationPorts,
                    (move.isUnconditional()) ?
                    nullptr : &move.guard().guard())) {
                partiallySuitableFUs.insert(fu);
                if (canSourceWriteToAnyDestinationPort(
                        mn, copyTriggerPorts)) {
                    suitableFUs.insert(fu);
                }
            }
        }
    }
    return suitableFUs.empty() ? partiallySuitableFUs : suitableFUs;
}

bool MachineConnectivityCheck::canBypassOpToDst(
    const TTAMachine::Machine& mach,
    const TCEString& opName,
    int outIndex,
    const MoveNode& mn) {

    PortSet sourcePorts;

    const TTAMachine::Machine::FunctionUnitNavigator& fuNav =
        mach.functionUnitNavigator();
    for (auto fu: fuNav) {
        if (fu->hasOperation(opName)) {
            TTAMachine::HWOperation* hwop = fu->operation(opName);
            sourcePorts.insert(hwop->port(outIndex));
        }
    }

    MachineConnectivityCheck::PortSet destinationPorts =
        MachineConnectivityCheck::findPossibleDestinationPorts(mach, mn);

    const TTAProgram::Move& move = mn.move();
    return MachineConnectivityCheck::isConnected(
        sourcePorts, destinationPorts,
        (move.isUnconditional()) ?
        nullptr : &move.guard().guard());
}
