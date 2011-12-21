/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file MachineTester.hh
 *
 * Declaration of MachineTester class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_MACHINE_TESTER_HH
#define TTA_MACHINE_TESTER_HH

#include <set>
#include <vector>
#include <map>

#include "Exception.hh"
#include "Socket.hh"

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
