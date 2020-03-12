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
#include "Guard.hh"

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

        int requiredBits = 
            MachineConnectivityCheck::requiredImmediateWidth(
                bus.signExtends(), immediate, 
                *destinationPort.parentUnit()->machine());
        if (bus.immediateWidth() < requiredBits) {
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

    // check connections to function units
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        for (int j = 0; j < fu.portCount(); j++ ) {
            Port& port = *fu.port(j);
            // connections from RF to FU's
            if (port.inputSocket() != NULL) {
                std::set<const TTAMachine::Bus*> sharedBuses;
                SetTools::intersection(
                    rfBuses, connectedSourceBuses(port), sharedBuses);
                if (sharedBuses.size() == 0) {
                    return false;
                }
            }
        }
    } 

    // check connections to control unit
    TTAMachine::ControlUnit& cu = *mach.controlUnit();
    for (int i = 0; i < cu.portCount(); i++ ) {
        Port& port = *cu.port(i);

        // connections from RF to CU
        if (port.inputSocket() != NULL) {
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

    TTAMachine::Machine& mach = *rf.machine();
    TTAMachine::Machine::FunctionUnitNavigator fuNav = 
        mach.functionUnitNavigator();
    TTAMachine::Machine::ImmediateUnitNavigator iuNav = 
        mach.immediateUnitNavigator();

    std::set<const TTAMachine::Bus*> rfBuses;

    for (int i = 0; i < rf.portCount(); i++) {
        const TTAMachine::Port& port = *rf.port(i);
        if (port.inputSocket() != NULL) {
            appendConnectedSourceBuses(port, rfBuses);
        }
    }

    // check connections from function units
    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        for (int j = 0; j < fu.portCount(); j++ ) {
            Port& port = *fu.port(j);
            // connections from FU to RF
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

    // check connections from immediate units 
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit& iu = *iuNav.item(i);
        bool iuConnected = false;
        for (int j = 0; j < iu.portCount(); j++ ) {
            Port& port = *iu.port(j);
            if (port.outputSocket() != NULL) {
                std::set<const TTAMachine::Bus*> sharedBuses;
                SetTools::intersection(
                    rfBuses, connectedDestinationBuses(port),sharedBuses);
                if (sharedBuses.size() != 0) {
                    iuConnected = true;
                    break;
                }
            }
        }
        if (iuConnected == false) {
            return false;
        }
    }

    // check connections from control unit
    TTAMachine::ControlUnit& cu = *mach.controlUnit();
    for (int i = 0; i < cu.portCount(); i++ ) {
        Port& port = *cu.port(i);

        // connections from CU to RF
        if (port.outputSocket() != NULL) {
            std::set<const TTAMachine::Bus*> sharedBuses;
            SetTools::intersection(
                rfBuses, connectedDestinationBuses(port), sharedBuses);
            if (sharedBuses.size() == 0) {
                return false;
            }
        }
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
 * Gets all register files needed for limited connectivity temp registers
 *
 * @param machine machine we are checking
 * @return vector of registerfiles whose last register is to be used for 
 */
std::vector<RegisterFile*>
MachineConnectivityCheck::tempRegisterFiles(
    const TTAMachine::Machine& machine) {
    std::vector<RegisterFile*> rfs;

    Machine::RegisterFileNavigator regNav =
        machine.registerFileNavigator();

    Machine::ImmediateUnitNavigator iuNav =
        machine.immediateUnitNavigator();

    Machine::FunctionUnitNavigator fuNav =
        machine.functionUnitNavigator();

    RegisterFile* connectedRF = NULL;
    bool allConnected = true;
    int widestRFWidth = 0;

    for (int i = 0; i < regNav.count(); i++) {
        if (regNav.item(i)->width() > widestRFWidth) {
            widestRFWidth = regNav.item(i)->width();
        }
    }

    for (int i = 0; i < regNav.count(); i++) {
        if (!rfConnected(*regNav.item(i))) {
            allConnected = false;
        } else {
            // we have at least on RF connected to everywhere so use it.
            // but only if it's wide enough (as wide as the widest RF)
            if (connectedRF == NULL && 
                regNav.item(i)->width() == widestRFWidth) {
                connectedRF = regNav.item(i);
            }
        }
    }

    // register copy also needed if unconnected imm from regs
    for (int i = 0; i < iuNav.count(); i++) {
        if (!fromRfConnected(*iuNav.item(i))) {
            allConnected = false;
        }
    }

    // check connection to and from RA, and imm->FU's
    // just checking load, store and jump might be enough. 
    // implement that later, lets now be safe and unoptimal
    ControlUnit& cu = *machine.controlUnit();
    SpecialRegisterPort& ra = *cu.returnAddressPort();

    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        for (int j = 0; j < fu.portCount(); j++ ) {
            Port& port = *fu.port(j);
            if (port.outputSocket() != NULL) {
                if (!isConnected(port, ra)) {
                    allConnected = false;
                } 
            }
            if (port.inputSocket() != NULL) {
                if (!isConnected(ra,port)||
                    !canWriteAllImmediates(port)) {
                    allConnected = false;
                } 
            }
        }
    }

    if (!allConnected) {
        if (connectedRF != NULL) {
            rfs.push_back(connectedRF);
            return rfs;
        } else {
            for (int i = 0; i < regNav.count(); i++) {
                RegisterFile* rf = regNav.item(i);
                if (rf->width() != 1) {
                    rfs.push_back(rf);
                }
            }
        }
    }
    
    return rfs;
}


/** 
 * Checks if there is a way to write immediate diretly from bus or from immu
 * to the given port
 * 
 * @param port Port where to check immediate write ability.
 */    
bool
MachineConnectivityCheck::canWriteAllImmediates(Port& destPort) {
    /** First check if there is a bus that can transfer the immediates */
    int portWidth = destPort.width();
    int widestConnectedImmediate = -1;
    int widestImmediate = -1;
    Socket& socket = *destPort.inputSocket();
    
    // check immediates from buses
    for (int i = 0; i < socket.segmentCount(); ++i) {
        const Bus& bus = *socket.segment(i)->parentBus();
        int immw = bus.immediateWidth();
        widestConnectedImmediate = 
            std::max(widestConnectedImmediate, immw);
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
	    widestConnectedImmediate = 
                std::max(widestConnectedImmediate, immw);
            if (immw >= portWidth) {
                return true;
            }
        }
	widestImmediate = std::max(widestImmediate, immw);
    }

    TTAMachine::Machine::BusNavigator busNav = 
        mach.busNavigator();
    
    for (int i = 0; i < busNav.count(); i++) {
        Bus& bus = *busNav.item(i);
        widestImmediate = std::max(widestImmediate, bus.immediateWidth());
    }    
    
    return widestImmediate == widestConnectedImmediate;
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
            MathTools::requiredBitsSigned(source.value().unsignedValue());
    } else if (!signExtension) {
        bits =
            MathTools::requiredBits(source.value().unsignedValue());
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

    TTAMachine::Unit* unit = NULL;
    ProgramOperation& po = moveNode.destinationOperation();
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
                unit = outputNode.move().source().port().parentUnit();
                break;
            }
        }
    }

    Operation& op = move.destination().hintOperation();
    TCEString opName = StringTools::stringToLower(op.name());

    if (unit != NULL) {
        FunctionUnit* fu = dynamic_cast<FunctionUnit*>(unit);
        assert(fu != NULL);
        return busConnectedToFU(bus, *fu, opName, opIndex);
    }

    // check if the move has a candidate FU set which limits the
    // choice of FU for the node
    std::set<TCEString> candidateFUs;
    std::set<TCEString> allowedFUs;

    addAnnotatedFUs(candidateFUs, move, TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
    addAnnotatedFUs(allowedFUs, move, TTAProgram::ProgramAnnotation::ANN_ALLOWED_UNIT_DST);

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

bool MachineConnectivityCheck::busConnectedToDestination(
    const TTAMachine::Bus& bus, const MoveNode& moveNode) {
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
const TTAMachine::Machine& mach, const MoveNode& node) {
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

                    allowedFUNames.insert(
                        mn.move().source().port().parentUnit()->name());
                }
            }
        }

        std::set<TCEString> candidateFUs;
        std::set<TCEString> allowedFUs;
	
        addAnnotatedFUs(candidateFUs, node.move(), 
	    TTAProgram::ProgramAnnotation::ANN_CONN_CANDIDATE_UNIT_DST);
        addAnnotatedFUs(allowedFUs, node.move(),
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
        TTAMachine::Port* port = rf.port(i);
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
MachineConnectivityCheck::addAnnotatedFUs(std::set<TCEString>& candidateFUs, 
	const TTAProgram::Move& m, 
	TTAProgram::ProgramAnnotation::Id id) {

     const int annotationCount =
                    m.annotationCount(id);
                for (int i = 0; i < annotationCount; ++i) {
                    std::string candidateFU =
                        m.annotation(i, id).stringValue();
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
    const TTAMachine::Machine& mach) {

    const Machine::RegisterFileNavigator regNav =
        mach.registerFileNavigator();

    std::set<std::pair<RegisterFile*,int> > allGuardRegs;
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
                    std::pair<RegisterFile*,int>(rg->registerFile(),
                                                 rg->registerIndex()));
            }
        }
    }


    // then check for the connections.
    for (int i = 0; i < regNav.count(); i++) {
        TTAMachine::RegisterFile* srf = regNav.item(i);
        for (int j = 0; j < regNav.count(); j++) {
            TTAMachine::RegisterFile* drf = regNav.item(i);
            for (std::set<std::pair<RegisterFile*,int> >::iterator k =
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
