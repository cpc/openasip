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
 * @file MachineTester.hh
 *
 * Declaration of MachineTester class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_MACHINE_TESTER_HH
#define TTA_MACHINE_TESTER_HH

#include <set>
#include <vector>
#include <map>

#include "Exception.hh"
#include "Socket.hh"
#include "Unit.hh"

/**
 * MachineTester is a class that implements all the API needed for
 * diagnostics of machine model.
 */
class MachineTester {
public:
    MachineTester(const TTAMachine::Machine& machine);
    virtual ~MachineTester();

    virtual bool canConnect(
        const TTAMachine::Socket& socket, 
        const TTAMachine::Segment& segment);
    virtual bool canConnect(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port);
    virtual bool canBridge(
        const TTAMachine::Bus& source,
        const TTAMachine::Bus& destination);
    virtual bool canSetDirection(
        const TTAMachine::Socket& socket, 
        TTAMachine::Socket::Direction direction);
    static bool isValidComponentName(const std::string& name);

    bool connectionExists() const;
    bool illegalRegistration() const;
    bool loop() const;
    TTAMachine::Bus* branchedBus() const;
    bool wrongSocketDirection() const;
    bool maxConnections() const;
    bool unknownSocketDirection() const;
    bool noConnections() const;
    TTAMachine::Port* sameDirSocketConnection(
        TTAMachine::Socket::Direction direction) const;
    TTAMachine::Port* forbiddenSocketDirection(
        TTAMachine::Socket::Direction direction) const;
    bool registerFilePortAlreadyConnected() const;

private:
    // copying prohibited
    MachineTester(const MachineTester& tester);
    // assingment prohibited
    MachineTester& operator=(const MachineTester& tester);

    void clearState();
    static bool areInSameChain(
        const TTAMachine::Bus& bus1,
        const TTAMachine::Bus& bus2);
    bool legalPortConnections(
        const TTAMachine::Socket& socket,
        TTAMachine::Socket::Direction direction);

    /// The machine on which the tests are carried out.
    const TTAMachine::Machine* machine_;
    /// Indicates if the reason for the last test failure is that the tested
    /// connection already exists.
    bool connectionExists_;
    /// Indicates if the reason for failure is that a port of register file
    /// is already connected to a particular socket.
    bool rfPortAlreadyConnected_;
    /// Indicates if the reason for last test failure was illegal
    /// registration.
    bool illegalRegistration_;
    /// Indicates if the reason for failure when trying to bridge two busses
    /// is that a loop in the bus chain would be created.
    bool loop_;
    /// If the reason for failure when trying to bridge two busses is that a
    /// branch in the bus chain would be created, this is the branched bus.
    const TTAMachine::Bus* branchedBus_;
    /// Indicates if the reason for failure when trying to connect a socket
    /// to a port was that the direction of the socket is incompatible with
    /// the direction of other sockets connected to the port.
    bool wrongSocketDirection_;
    /// Indicates if the reason for failure when trying to connect a socket
    /// to a port was that this connection would exceed the maximum number
    /// of connections allowed for a port.
    bool maxConnections_;
    /// Indicates if tried to set the socket direction to Socket::UNKNOWN.
    bool unknownDir_;
    /// Indicates if the reason for failure when tried to set socket
    /// direction was that the socket has no segment connections.
    bool noConnections_;
    /// If the reason for failure when tried to set socket
    /// direction to input was that the socket is connected to a port that
    /// already has an input socket connected, this is the port.
    TTAMachine::Port* sameDirInputSocketConn_;
    /// If the reason for failure when tried to set socket
    /// direction to output was that the socket is connected to a port that
    /// already has an output socket connected, this is the port.
    TTAMachine::Port* sameDirOutputSocketConn_;
    /// If the reason for failure when tried to set socket direction to input
    /// was the socket is connected to a unit that cannot be connected to an
    /// input socket, this is the port of the unit.
    TTAMachine::Port* forbiddenInputSocketDir_;
};

#endif
