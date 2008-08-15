/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file MachineConnectivityCheck.cc
 *
 * Implementation of MachineConnectivityCheck class.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include <set>

#include "MachineConnectivityCheck.hh"
#include "Application.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "SetTools.hh"
#include "RegisterFile.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"
#include "MathTools.hh"
#include "TerminalImmediate.hh"

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
    const TTAMachine::Port& destinationPort) {

    std::set<TTAMachine::Bus*> sourceBuses;
    MachineConnectivityCheck::appendConnectedDestinationBuses(
        sourcePort, sourceBuses);

    std::set<TTAMachine::Bus*> destinationBuses;
    MachineConnectivityCheck::appendConnectedSourceBuses(
        destinationPort, destinationBuses);

    std::set<TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(sourceBuses, destinationBuses, sharedBuses);

    return sharedBuses.size() > 0;
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
    const TTAMachine::BaseRegisterFile& destRF) {

    std::set<TTAMachine::Bus*> buses;
    MachineConnectivityCheck::appendConnectedSourceBuses(destRF, buses);

    for (std::set<TTAMachine::Bus*>::const_iterator i = buses.begin(); 
         i != buses.end(); ++i) {
        TTAMachine::Bus& bus = **i;

        int requiredBits = 
            MachineConnectivityCheck::requiredImmediateWidth(
                bus.signExtends(), immediate);
        if (bus.immediateWidth() >= requiredBits)
            return true;
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
    const TTAMachine::Port& destinationPort) {

    std::set<TTAMachine::Bus*> buses;
    MachineConnectivityCheck::appendConnectedSourceBuses(
        destinationPort, buses);

    for (std::set<TTAMachine::Bus*>::const_iterator i = buses.begin(); 
         i != buses.end(); ++i) {
        TTAMachine::Bus& bus = **i;

        int requiredBits = 
            MachineConnectivityCheck::requiredImmediateWidth(
                bus.signExtends(), immediate);
        if (bus.immediateWidth() >= requiredBits)
            return true;
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
    
    std::set<TTAMachine::Bus*> destBuses = connectedSourceBuses(destPort);
    std::set<TTAMachine::Bus*> srcBuses;

    for (int i = 0; i < sourceRF.portCount(); i++) {
        const TTAMachine::Port& port = *sourceRF.port(i);
        if (port.outputSocket() != NULL) {
            appendConnectedDestinationBuses(port, srcBuses);
        }
    }

    std::set<TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(
        srcBuses, destBuses, sharedBuses);
    return (sharedBuses.size() > 0);
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
    const TTAMachine::BaseRegisterFile& destRF) {
    
    std::set<TTAMachine::Bus*> srcBuses;
    appendConnectedDestinationBuses(sourceRF, srcBuses);

    std::set<TTAMachine::Bus*> dstBuses;
    appendConnectedSourceBuses(destRF, dstBuses);

    std::set<TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(srcBuses, dstBuses, sharedBuses);
    return (sharedBuses.size() > 0);
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
    
    std::set<TTAMachine::Bus*> srcBuses;
    appendConnectedDestinationBuses(sourceRF, srcBuses);

    std::set<TTAMachine::Bus*> dstBuses;
    appendConnectedSourceBuses(destFU, dstBuses);

    std::set<TTAMachine::Bus*> sharedBuses;
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

    std::set<TTAMachine::Bus*> sourceBuses = 
        connectedDestinationBuses(sourcePort);
    std::set<TTAMachine::Bus*> destBuses;

    for (int i = 0; i < destRF.portCount(); i++) {
        const TTAMachine::Port& port = *destRF.port(i);
        if (port.inputSocket() != NULL) {
            appendConnectedSourceBuses(port, destBuses);
        }
    }
    
    std::set<TTAMachine::Bus*> sharedBuses;
    SetTools::intersection(sourceBuses, destBuses, sharedBuses);
    return (sharedBuses.size() > 0 );
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
    

    std::set<TTAMachine::Bus*> rfBuses;

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
                std::set<TTAMachine::Bus*> sharedBuses;
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
            std::set<TTAMachine::Bus*> sharedBuses;
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

    std::set<TTAMachine::Bus*> rfBuses;

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
                std::set<TTAMachine::Bus*> sharedBuses;
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
                std::set<TTAMachine::Bus*> sharedBuses;
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
            std::set<TTAMachine::Bus*> sharedBuses;
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
std::set<TTAMachine::Bus*> 
MachineConnectivityCheck::connectedSourceBuses(
    const TTAMachine::Port& port) {

    std::set<TTAMachine::Bus*> buses;
    appendConnectedSourceBuses(port,buses);
    return buses;
}

/**
 * Returns all buses the given port can write to
 *
 * @param port The port to check.
 * @return The set of buses connected to the port.
 */
std::set<TTAMachine::Bus*> 
MachineConnectivityCheck::connectedDestinationBuses(
    const TTAMachine::Port& port) {

    std::set<TTAMachine::Bus*> buses;
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
    const TTAMachine::Port& port, std::set<TTAMachine::Bus*>& buses) {

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
    const TTAMachine::Port& port, std::set<TTAMachine::Bus*>& buses) {

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
    const TTAMachine::Unit& unit, std::set<TTAMachine::Bus*>& buses) {

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
    const TTAMachine::Unit& unit, std::set<TTAMachine::Bus*>& buses) {

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
    Socket& socket = *destPort.inputSocket();

    // check immediates from buses
    for (int i = 0; i < socket.segmentCount(); ++i) {
        const Bus& bus = *socket.segment(i)->parentBus();
        if (bus.immediateWidth() >= portWidth) {
            return true;
        }
    }

    // then check directly connected imm units
    const TTAMachine::Machine& mach = *destPort.parentUnit()->machine();
    TTAMachine::Machine::ImmediateUnitNavigator iuNav = 
        mach.immediateUnitNavigator();
    
    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit& iu = *iuNav.item(i);
        if (iu.width() >= portWidth && isConnected(iu, destPort)) {
            return true;
        }
    }
    
    return false;
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
    const TTAProgram::TerminalImmediate& source) {

    int bits = -1;
    if (signExtension) {
        bits =
            MathTools::requiredBitsSigned(source.value().unsignedValue());
    } else if (!signExtension) {
        bits =
            MathTools::requiredBits(source.value().unsignedValue());
    } 

    /* This heuristics always reserves at least 10 bits to the required width
       of an instruction address, since the actual final address
       is not known before generating the binary program image. The 
       address depends on compression, encoding, etc. and compression 
       might depend on the schedule itself, etc. It's an egg-and-chicken 
       problem. In addition it always adds an additional bit to the
       original address width "for safety".

       The minimum bit guess is totally arbitrary, it can be much
       more (especially if a whole function is moved around) or much
       less (much improved schedule from the sequential program).

       @todo We should get this from the largest address of the
       instruction memory address space or using smarter heuristics?*/
    if (source.isInstructionAddress() && bits < 32) {
        ++bits;
        if (bits < 10)
            bits = 10;
    }
    return bits;
}
