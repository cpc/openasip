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
