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
 * @file Socket.hh
 *
 * Declaration of Socket class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 22 Jun 2004 by ao, ml, vpj, ll
 */

#ifndef TTA_SOCKET_HH
#define TTA_SOCKET_HH

#include <string>
#include <vector>
#include <set>

#include "MachinePart.hh"

namespace TTAMachine {

class Segment;
class Connection;
class Port;
class Bus;

/**
 * Represents a socket in the TTA processor.
 */
class Socket : public Component {
public:
    /**
     * Direction of data movements in socket.
     */
    enum Direction {
        INPUT,  ///< Data goes from bus to port.
        OUTPUT, ///< Data goes from port to bus.
        UNKNOWN ///< Unknown direction.
    };

    Socket(const std::string& name)
        throw (InvalidName);
    Socket(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~Socket();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    void setDirection(Direction direction)
        throw (IllegalConnectivity);
    Direction direction() const;
    void attachBus(Segment& bus)
        throw (IllegalRegistration, IllegalConnectivity);
    void detachBus(Segment& bus)
        throw (InstanceNotFound);
    void detachBus(Bus& bus);
    int portCount() const;
    Port* port(int index) const
        throw (OutOfRange);

    void detachAllPorts();
    const Connection& connection(const Segment& bus) const;
    bool isConnectedTo(const Bus& bus) const;
    bool isConnectedTo(const Segment& bus) const;

    int segmentCount() const;
    Segment* segment(int index) const
        throw (OutOfRange);

    bool hasDataPortWidth() const;

    std::string const& dataPortWidth() const;

    void setDataPortWidth(std::string const& width);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    /// ObjectState name for socket.
    static const std::string OSNAME_SOCKET;
    /// ObjectState attribute key for socket direction.
    static const std::string OSKEY_DIRECTION;
    /// ObjectState attribute value for input direction.
    static const std::string OSVALUE_INPUT;
    /// ObjectState attribute value for output direction.
    static const std::string OSVALUE_OUTPUT;
    /// ObjectState attribute value for unknown direction.
    static const std::string OSVALUE_UNKNOWN;

private:
    /// Table of Connection pointers.
    typedef std::vector<const Connection*> ConnectionTable;
    /// Table of port connections.
    typedef std::vector<Port*> PortTable;

    /// Copying forbidden.
    Socket(const Socket&);
    /// Assingment forbidden.
    Socket& operator=(const Socket&);

    void removeConnection(const Connection* connection);
    void attachPort(Port& port);
    void detachPort(const Port& port);
    void detachAllBuses();

    /// Direction of the socket.
    Direction direction_;
    /// Dataport width
    std::string dataPortWidth_;
    /// Contains all connections to busses.
    ConnectionTable busses_;
    /// Contains all connections to ports.
    PortTable ports_;

    // port must be able to insert and remove ports from PortTable
    friend class Port;
};
}

#include "Socket.icc"

#endif
