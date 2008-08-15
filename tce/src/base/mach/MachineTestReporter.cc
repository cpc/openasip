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
 * @file MachineTestReporter.cc
 *
 * Implementation of MachineTestReporter class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#include "MachineTestReporter.hh"
#include "MachineTestReportTextGenerator.hh"
#include "MachineTester.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Port.hh"
#include "Application.hh"

using std::string;
using boost::format;
using namespace TTAMachine;

/**
 * Generates a general error message of the error that occurred when tried
 * to attach a socket to a segment.
 *
 * @param socket The socket that was tried to be connected.
 * @param segment The segment to which the socket was tried to be connected.
 * @param tester MachineTester that was used to check if the connection can
 *               be made.
 * @return An error message.
 */
string
MachineTestReporter::socketSegmentConnectionError(
    const Socket& socket,
    const Segment& segment,
    const MachineTester& tester) {

    MachineTestReportTextGenerator textGenerator;

//     Bus* sameChainConnection = tester.sameChainConnection();
//     if (sameChainConnection != NULL) {
//         format text = textGenerator.text(MachineTestReportTextGenerator::
//             TXT_SOCKET_SEGMENT_CONN_SAME_CHAIN);
//         text % socket.name() % segment.name() % segment.parentBus()->name() %
//             sameChainConnection->name();
//         return text.str();
//     }

    if (tester.connectionExists()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_SEGMENT_CONN_EXISTS);
        text % socket.name() % segment.name() % segment.parentBus()->name();
        return text.str();
    }

    if (tester.illegalRegistration()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
            TXT_SOCKET_SEGMENT_CONN_ILLEGAL_REG);
        text % socket.name() % segment.parentBus()->name();
        return text.str();
    }

//     Bus* straddlingBus = tester.straddlingBus();
//     if (straddlingBus != NULL) {
//         format text = textGenerator.text(
//             MachineTestReportTextGenerator::
//             TXT_SOCKET_SEGMENT_CONN_STRADDLING_BUS);
//         text % socket.name() % segment.name() % segment.parentBus()->name() %
//             straddlingBus->name();
//         return text.str();
//     }

//     Socket* crossingSocket = tester.crossingSocket(socket);
//     if (crossingSocket != NULL) {
//         format text = textGenerator.text(MachineTestReportTextGenerator::
//                                          TXT_SOCKET_SEGMENT_CONN_CROSSING);
//         text % socket.name() % segment.name() % segment.parentBus()->name() %
//             crossingSocket->name();
//         return text.str();
//     }

//     Unit* straddlingUnit = tester.straddlingUnit();
//     if (straddlingUnit != NULL) {
//         format text = textGenerator.text(
//             MachineTestReportTextGenerator::
//             TXT_SOCKET_SEGMENT_CONN_STRADDLING_UNIT);
//         text % socket.name() % segment.name() % segment.parentBus()->name() %
//             straddlingUnit->name();
//         return text.str();
//     }

    Port* doubleInputConn = tester.sameDirSocketConnection(Socket::INPUT);
    Port* doubleOutputConn = tester.sameDirSocketConnection(Socket::OUTPUT);
    Port* forbiddenInputConn =
        tester.forbiddenSocketDirection(Socket::INPUT);
    Port* forbiddenOutputConn =
        tester.forbiddenSocketDirection(Socket::OUTPUT);
    if (doubleInputConn != NULL || forbiddenInputConn != NULL) {
        assert(doubleOutputConn != NULL || forbiddenOutputConn != NULL);
        format text = textGenerator.text(
            MachineTestReportTextGenerator::
            TXT_SOCKET_SEGMENT_CONN_BOTH_DIRS_ARE_ILLEGAL);
        Port* errInputPort = doubleInputConn != NULL ? doubleInputConn :
            forbiddenInputConn;
        Port* errOutputPort = doubleOutputConn != NULL ? doubleOutputConn :
            forbiddenOutputConn;
        text % socket.name() % errInputPort->name() %
            errInputPort->parentUnit()->name() % errOutputPort->name() %
            errOutputPort->parentUnit()->name();
        return text.str();
    }

    return "";
}


/**
 * Generates a general error message of the error that occurred when tried
 * to attach a socket to a port.
 *
 * @param socket The socket that was tried to be connected.
 * @param port The port to which the socket was tried to be connected.
 * @param tester MachineTester that was used to check if the connection can
 *               be made.
 * @return An error message.
 */
string
MachineTestReporter::socketPortConnectionError(
    const Socket& socket,
    const Port& port,
    const MachineTester& tester) {

    MachineTestReportTextGenerator textGenerator;

    if (tester.illegalRegistration()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_PORT_CONN_ILLEGAL_REG);
        text % socket.name() % port.parentUnit()->name();
        return text.str();
    }

    if (tester.connectionExists()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_PORT_CONN_EXISTS);
        text % socket.name() % port.name() % port.parentUnit()->name();
        return text.str();
    }

    if (tester.wrongSocketDirection()) {
        format text = textGenerator.text(
            MachineTestReportTextGenerator::
            TXT_SOCKET_PORT_CONN_WRONG_SOCKET_DIR);
        text % socket.name() % port.name() % port.parentUnit()->name();
        return text.str();
    }

    if (tester.maxConnections()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_PORT_CONN_MAX_CONN);
        text % socket.name() % port.name() % port.parentUnit()->name();
        return text.str();
    }

//     Socket* straddlingUnitConn = tester.straddlingUnitConnection();
//     if (straddlingUnitConn != NULL) {
//         format text = textGenerator.text(
//             MachineTestReportTextGenerator::
//             TXT_SOCKET_PORT_CONN_STRADDLING_UNIT);
//         text % socket.name() % port.name() % port.parentUnit()->name() %
//             straddlingUnitConn->name();
//         return text.str();
//     }

    if (tester.registerFilePortAlreadyConnected()) {
        format text = textGenerator.text(
            MachineTestReportTextGenerator::
            TXT_SOCKET_PORT_CONN_RF_PORT_CONN_EXISTS);
        text % socket.name() % port.name() % port.parentUnit()->name();
        return text.str();
    }

    return "";
}


/**
 * Generates a general error message of the error that occurred when tried
 * to bridge the given buses.
 *
 * @param sourceBus The source bus.
 * @param destinationBus The destination bus.
 * @param tester MachineTester that was used to check if the buses can be
 *               joined by a bridge.
 * @return An error message.
 */
string
MachineTestReporter::bridgingError(
    const Bus& sourceBus,
    const Bus& destinationBus,
    const MachineTester& tester) {

    MachineTestReportTextGenerator textGenerator;
    if (tester.illegalRegistration()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_BRIDGE_ILLEGAL_REG);
        text % sourceBus.name() % destinationBus.name();
        return text.str();
    }
    if (tester.loop()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_BRIDGE_LOOP);
        text % sourceBus.name() % destinationBus.name();
        return text.str();
    }
    if (tester.connectionExists()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_BRIDGE_EXISTS);
        text % sourceBus.name() % destinationBus.name();
        return text.str();
    }
//     Bus* straddlingBus = tester.straddlingBus();
//     if (straddlingBus != NULL) {
//         format text = textGenerator.text(MachineTestReportTextGenerator::
//                                          TXT_BRIDGE_STRADDLING_BUS);
//         text % sourceBus.name() % destinationBus.name() %
//             straddlingBus->name();
//         return text.str();
//     }
//     Socket* crossing = tester.crossingSocket(0);
//     if (crossing != NULL) {
//         Socket* crossing2 = tester.crossingSocket(1);
//         format text = textGenerator.text(MachineTestReportTextGenerator::
//                                          TXT_BRIDGE_CROSSING_SOCKET);
//         text % sourceBus.name() % destinationBus.name() % crossing->name() %
//             crossing2->name();
//         return text.str();
//     }
    Bus* branchedBus = tester.branchedBus();
    if (branchedBus != NULL) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_BRIDGE_BRANCHED_BUS);
        text % sourceBus.name() % destinationBus.name() %
            branchedBus->name();
        return text.str();
    }
//     Socket* bindingSocket = tester.bindingSocket();
//     if (bindingSocket != NULL) {
//         format text = textGenerator.text(MachineTestReportTextGenerator::
//                                          TXT_BRIDGE_BINDING_SOCKET);
//         text % sourceBus.name() % destinationBus.name() %
//             bindingSocket->name();
//         return text.str();
//     }
//     Unit* straddlingUnit = tester.straddlingUnit();
//     if (straddlingUnit != NULL) {
//         format text = textGenerator.text(MachineTestReportTextGenerator::
//                                          TXT_BRIDGE_STRADDLING_UNIT);
//         text % sourceBus.name() % destinationBus.name() %
//             straddlingUnit->name();
//         return text.str();
//     }

    return "";
}


/**
 * Generates a general error message of the reason why the direction of the
 * given socket can not be set.
 *
 * @param socket The socket.
 * @param direction The direction that was tried to set.
 * @param tester The MachineTester used to check if the direction can be set.
 * @return An error message.
 */
string
MachineTestReporter::socketDirectionSettingError(
    const Socket& socket,
    Socket::Direction direction,
    const MachineTester& tester) {

    MachineTestReportTextGenerator textGenerator;
    if (tester.unknownSocketDirection()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_DIR_UNKNOWN);
        text % socket.name();
        return text.str();
    }
    if (tester.noConnections()) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_NO_CONN);
        text % socket.name();
        return text.str();
    }
    Port* sameDirConnection = tester.sameDirSocketConnection(direction);
    if (sameDirConnection != NULL) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_SAME_DIR_CONN);
        if (direction == Socket::OUTPUT) {
            text % socket.name() % "output" % sameDirConnection->name() %
                sameDirConnection->parentUnit()->name();
        } else if (direction == Socket::INPUT) {
            text % socket.name() % "input" % sameDirConnection->name() %
                sameDirConnection->parentUnit()->name();
        } else {
            assert(false);
        }
        return text.str();
    }
    Port* forbiddenSocketDir = tester.forbiddenSocketDirection(direction);
    if (forbiddenSocketDir != NULL) {
        format text = textGenerator.text(MachineTestReportTextGenerator::
                                         TXT_SOCKET_FORBIDDEN_DIR);
        if (direction == Socket::OUTPUT) {
            text % socket.name() % "output" % forbiddenSocketDir->name() %
                forbiddenSocketDir->parentUnit()->name();
        } else if (direction == Socket::INPUT) {
            text % socket.name() % "input" % forbiddenSocketDir->name() %
                forbiddenSocketDir->parentUnit()->name();
        } else {
            assert(false);
        }
        return text.str();
    }
    return "";
}
