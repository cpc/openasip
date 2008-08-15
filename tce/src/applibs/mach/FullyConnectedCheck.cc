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
 * @file FullyConnectedCheck.hh
 *
 * Implementation of FullyConnectedCheck class.
 *
 * @author Jari Mäntyneva 2007 (jari.mantyneva@tut.fi)
 * @note rating: red
 */

#include "FullyConnectedCheck.hh"
#include "MachineTester.hh"
#include "FunctionUnit.hh"
#include "FUPort.hh"
#include "Socket.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"

using namespace TTAMachine;

/**
 * Constructor for using this generic class directly.
 */
FullyConnectedCheck::FullyConnectedCheck() :
    MachineCheck("Fully connected machine check.") {
}

/**
 * Constructor for deriving.
 *
 * @param shortDesc The short description of the check.
 */
FullyConnectedCheck::FullyConnectedCheck(const std::string& shortDesc):
    MachineCheck(shortDesc) {
}

/**
 * The destructor.
 */
FullyConnectedCheck::~FullyConnectedCheck() {
}

/**
 * Returns true if the machine can be fixed to pass the check.
 *
 * @param mach The machine to be analyzed.
 * @return True if the machine can be fixed to pass the check.
 */
bool
FullyConnectedCheck::canFix(const TTAMachine::Machine& mach) const {

    const TTAMachine::Machine::BridgeNavigator brNav =
        mach.bridgeNavigator();
    
    const TTAMachine::Machine::BusNavigator busNav =
        mach.busNavigator();
    
    if (busNav.count() == 0 || brNav.count() > 0) {
        return false;
    }
    return true;
}

/**
 * Checks is the machine fully connected and adds errors to results.
 *
 * @param mach Machine to be checked.
 * @param results Check results.
 * @return True if the machine is fully connected.
 */
bool
FullyConnectedCheck::check(
    const TTAMachine::Machine& mach, MachineCheckResults& results) const {

    bool result = true;

    const Machine::SocketNavigator
        socketNav = mach.socketNavigator();

    for (int so = 0; so < socketNav.count(); so++) {
        // for each socket in the machine
        if (!socketAttachedToAllBusses(*socketNav.item(so))) {
            results.addError(
                *this, "All sockets not connected to all busses.");
            result = false;
        }
    }
    // Connect all unconnected FU ports to newly created sockets.
    const Machine::FunctionUnitNavigator fuNav = mach.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        for (int p = 0; p < fu.operationPortCount(); p++) {
            FUPort& port = *fu.operationPort(p);
            if (port.socketCount() == 0) {
                results.addError(
                    *this, "FU: '" + fu.name() + "' port '" +
                    port.name() + "' not connected to any socket.");
                result = false;
            }
        }
    }

    // Connect all unconnected IU ports to new sockets.
    const Machine::ImmediateUnitNavigator iuNav =
        mach.immediateUnitNavigator();

    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit& iu = *iuNav.item(i);
        for (int p = 0; p < iu.portCount(); p++) {
            RFPort& port = *iu.port(p);
            if (port.socketCount() == 0) {
                results.addError(
                    *this, "IU: '" + iu.name() + "' port '" +
                    port.name() + "' not connected to any socket.");
                result = false;
            }
        }
    }

    // Connect all unconnected RF ports to new sockets.
    const Machine::RegisterFileNavigator rfNav = mach.registerFileNavigator();

    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile& rf = *rfNav.item(i);
        for (int p = 0; p < rf.portCount(); p++) {
            RFPort& port = *rf.port(p);
            if (port.socketCount() == 0) {
                results.addError(
                    *this, "RF: '" + rf.name() + "' port '" +
                    port.name() + "' not connected to any socket.");
                result = false;
            }
        }
    }

    ControlUnit* gcu = mach.controlUnit();
    if (gcu != NULL) {
        for (int p = 0; p < gcu->operationPortCount(); p++) {
            FUPort& port = *gcu->operationPort(p);
            if (port.socketCount() == 0) {
                results.addError(
                    *this, "GCU port '" +
                    port.name() + "' not connected to any socket.");
                return false;
            }
        }
        for (int p = 0; p < gcu->specialRegisterPortCount(); p++) {
            SpecialRegisterPort& port = *gcu->specialRegisterPort(p);
            if (port.socketCount() == 0) {
                results.addError(
                    *this, "GCU port '" +
                    port.name() + "' not connected to any socket.");
            }
        }
    }
    return result;
}

/**
 * Fixes the machine to pass the chek if possible.
 *
 * @param mach Machine to be fixed.
 * @return Returns an empty string.
 * @exception InvalidData Thows if the machine cannot be fixed.
 */
std::string
FullyConnectedCheck::fix(TTAMachine::Machine& mach) const
    throw (InvalidData) {

    if (!canFix(mach)) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "Machines contains no busses or contains bridges.");
    }

    const Machine::SocketNavigator
        socketNav = mach.socketNavigator();

    for (int so = 0; so < socketNav.count(); so++) {
        // for each socket in the machine
        attachSocketToAllBusses(*socketNav.item(so));
    }
    // Connect all unconnected FU ports to newly created sockets.
    const Machine::FunctionUnitNavigator fuNav = mach.functionUnitNavigator();

    for (int i = 0; i < fuNav.count(); i++) {
        FunctionUnit& fu = *fuNav.item(i);
        for (int p = 0; p < fu.operationPortCount(); p++) {
            FUPort& port = *fu.operationPort(p);
            if (port.socketCount() == 0) {
                connectFUPort(port);
            }
        }
    }

    // Connect all unconnected IU ports to new sockets.
    const Machine::ImmediateUnitNavigator iuNav =
        mach.immediateUnitNavigator();

    for (int i = 0; i < iuNav.count(); i++) {
        ImmediateUnit& iu = *iuNav.item(i);
        for (int p = 0; p < iu.portCount(); p++) {
            RFPort& port = *iu.port(p);
            if (port.socketCount() == 0) {
                connectIUPort(port);
            }
        }
    }

    // Connect all unconnected RF ports to new sockets.
    const Machine::RegisterFileNavigator rfNav = mach.registerFileNavigator();

    for (int i = 0; i < rfNav.count(); i++) {
        RegisterFile& rf = *rfNav.item(i);
        for (int p = 0; p < rf.portCount(); p++) {
            RFPort& port = *rf.port(p);
            if (port.socketCount() == 0) {
                connectRFPort(port);
            }
        }
    }

    ControlUnit* gcu = mach.controlUnit();
    if (gcu != NULL) {
        for (int p = 0; p < gcu->operationPortCount(); p++) {
            FUPort& port = *gcu->operationPort(p);
            if (port.socketCount() == 0) {
                connectFUPort(port);
            }
        }
        for (int p = 0; p < gcu->specialRegisterPortCount(); p++) {
            SpecialRegisterPort& port = *gcu->specialRegisterPort(p);
            if (port.socketCount() == 0) {
                connectSpecialRegisterPort(port);
            }
        }
    }    

    return "";
}

/**
 * Connects an immediate unit port to all possible busses.
 *
 * @param port Immediate unit port to connect.
 */
void
FullyConnectedCheck::connectIUPort(RFPort& port) const {

    MachineTester tester(*port.parentUnit()->machine());

    Socket* socket = createSocket(*port.parentUnit(), Socket::OUTPUT);

    if (tester.canConnect(*socket, port)) {
        port.attachSocket(*socket);
    } else {
        delete socket;
    }
}

/**
 * Connects a special register port to all possible busses.
 *
 * @param port Special register port to connect.
 */
void
FullyConnectedCheck::connectSpecialRegisterPort(
    SpecialRegisterPort& port) const{

    MachineTester tester(*port.parentUnit()->machine());

    Socket* inSocket = createSocket(*port.parentUnit(), Socket::INPUT);
    Socket* outSocket = createSocket(*port.parentUnit(), Socket::OUTPUT);

    if (tester.canConnect(*inSocket, port)) {
        port.attachSocket(*inSocket);
    } else {
        delete inSocket;
    }

    if (tester.canConnect(*outSocket, port)) {
        port.attachSocket(*outSocket);
    } else {
        delete outSocket;
    }
}


/**
 * Connects a register file port to all possible busses.
 *
 * @param port Immediate unit port to connect.
 */
void
FullyConnectedCheck::connectRFPort(RFPort& port) const {

    MachineTester tester(*port.parentUnit()->machine());
    const BaseRegisterFile& rf = *port.parentUnit();

    // Count input and output ports.
    int inputPorts = 0;
    int outputPorts = 0;
    for (int i = 0; i < rf.portCount(); i++) {
        const RFPort* p = rf.port(i);
        if (p->inputSocket() != NULL) {
            inputPorts++;
        }
        if (p->outputSocket() != NULL) {
            outputPorts++;
        }
    }

    Socket* socket = NULL;

    if (inputPorts > outputPorts) {
        socket = createSocket(*port.parentUnit(), Socket::OUTPUT);
    } else {
        socket = createSocket(*port.parentUnit(), Socket::INPUT);
    }

    if (tester.canConnect(*socket, port)) {
        port.attachSocket(*socket);
    } else {
        delete socket;
    }
}

/**
 * Creates a new socket to be attached to an unit.
 *
 * @param unit Unit whose name is used to generate a name for the socket.
 * @param direction Direction of the socket.
 * @return A new socket, attached to all possible busses, but not to a port.
 */
Socket*
FullyConnectedCheck::createSocket(
    const Unit& unit, Socket::Direction direction) const {

    TTAMachine::Machine& machine = *unit.machine();

    const TTAMachine::Machine::SocketNavigator socketNav =
        machine.socketNavigator();

    int s = 1;
    std::string socketName = unit.name();
    if (direction == Socket::INPUT) {
        socketName = socketName + "_i";
    } else {
        socketName = socketName + "_o";
    }

    while (socketNav.hasItem(
               socketName + Conversion::toString(s))) {

        s++;
    }

    Socket* socket =
        new Socket(socketName + Conversion::toString(s));

    socket->setMachine(machine);
    attachSocketToAllBusses(*socket);

    // direction can be set only if the socket is connected to a segment
    if (socket->segmentCount() > 0) {
        socket->setDirection(direction);
    }

    return socket;
}


/**
 * Creates socket(s) and connects a FU port to all possible busses.
 *
 * @param port FU port to connect to busses.
 */
void
FullyConnectedCheck::connectFUPort(FUPort& port) const {

    const FunctionUnit& fu = *port.parentUnit();
    Machine& machine = *fu.machine();
    MachineTester tester(machine);

    const TTAMachine::Machine::BusNavigator busNav = machine.busNavigator();
    const Machine::SocketNavigator socketNav =
        machine.socketNavigator();

    // Operand ports.
    for (int i = 0; i < fu.operationCount(); i++) {
        const HWOperation& operation = *fu.operation(i);
        for (int c = 0; c < operation.latency(); c++) {
            if (port.inputSocket() == NULL &&
                operation.pipeline()->isPortRead(port, c)) {

                Socket* socket = createSocket(fu, Socket::INPUT);

                if (tester.canConnect(*socket, port)) {
                    port.attachSocket(*socket);
                } else {
                    delete socket;
                }
                break;
            }
        }
    }

    // Result ports.
    for (int i = 0; i < fu.operationCount(); i++) {
        const HWOperation& operation = *fu.operation(i);
        for (int c = 0; c < operation.latency(); c++) {
            if (port.outputSocket() == NULL &&
                operation.pipeline()->isPortWritten(port, c)) {

                Socket* socket = createSocket(fu, Socket::OUTPUT);
                if (tester.canConnect(*socket, port)) {
                    port.attachSocket(*socket);
                } else {
                    delete socket;
                }
                break;
            }
        }
    }

}

/**
 * Attaches given socket to all possible busses.
 *
 * @param socket Socket to connect.
 */
void
FullyConnectedCheck::attachSocketToAllBusses(Socket& socket) const {

    TTAMachine::Machine& machine = *socket.machine();
    MachineTester tester(machine);

    const TTAMachine::Machine::BusNavigator busNav = machine.busNavigator();

    for (int b = 0; b < busNav.count(); b++) {
        // for each bus in the machine
        TTAMachine::Bus& bus = *busNav.item(b);
        for (int seg = 0; seg < bus.segmentCount(); seg++) {
            TTAMachine::Segment& segment = *bus.segment(seg);
            // for each segment in the bus
            if ((!socket.isConnectedTo(segment)) &&
                tester.canConnect(socket, segment)) {
                
                socket.attachBus(segment);
            }
        }
    }
}

/**
 * Checks if the given socket is attached to all possible busses.
 *
 * @param socket Socket to check.
 * @return True if the socket is attached to all possible busses.
 */
bool
FullyConnectedCheck::socketAttachedToAllBusses(const Socket& socket) const {

    TTAMachine::Machine& machine = *socket.machine();
    MachineTester tester(machine);

    const TTAMachine::Machine::BusNavigator busNav = machine.busNavigator();

    for (int b = 0; b < busNav.count(); b++) {
        // for each bus in the machine
        TTAMachine::Bus& bus = *busNav.item(b);
        for (int seg = 0; seg < bus.segmentCount(); seg++) {
            TTAMachine::Segment& segment = *bus.segment(seg);
            // for each segment in the bus
            if (!socket.isConnectedTo(segment)) {
                if (tester.canConnect(socket, segment)) {
                    return false;
                }
            }
        }
    }
    return true;
}
