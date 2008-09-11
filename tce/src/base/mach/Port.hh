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
 * @file Port.hh
 *
 * Declaration of Port class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#ifndef TTA_PORT_HH
#define TTA_PORT_HH

#include <string>

#include "MachinePart.hh"
#include "Exception.hh"
#include "ObjectState.hh"

namespace TTAMachine {

class Socket;
class Unit;
class RegisterFile;
class FunctionUnit;

/**
 * Represents a port between function units and sockets.
 */
class Port : public SubComponent {
public:
    Port(const std::string& name, Unit& parentUnit)
        throw (ComponentAlreadyExists, InvalidName);
    Port(const ObjectState* state, Unit& parentUnit)
        throw (ObjectStateLoadingException);
    virtual ~Port();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    virtual int width() const = 0;
    virtual void attachSocket(Socket& socket)
        throw (IllegalRegistration, ComponentAlreadyExists,
               IllegalConnectivity);
    virtual void detachSocket(Socket& socket)
        throw (InstanceNotFound);
    virtual void detachAllSockets();
    Unit* parentUnit() const;
    virtual Socket* inputSocket() const;
    virtual Socket* outputSocket() const;
    virtual Socket* unconnectedSocket(int index) const
        throw (OutOfRange);
    virtual int socketCount() const;
    virtual bool isConnectedTo(const Socket& socket) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

    virtual bool isOutput() const;
    virtual bool isInput() const;

    /// ObjectState name for Port.
    static const std::string OSNAME_PORT;
    /// ObjectState attribute key for the name of the port.
    static const std::string OSKEY_NAME;
    /// ObjectState attribute key for the name of the connected socket.
    static const std::string OSKEY_FIRST_SOCKET;
    /// ObjectState attribute key for the name of the other connected socket.
    static const std::string OSKEY_SECOND_SOCKET;

    struct PairComparator {
        bool operator()(
            const std::pair<const Port*, const Port*>& pp1, 
            const std::pair<const Port*, const Port*>& pp2)
            const;
    };

protected:
    Port(const std::string& name, FunctionUnit& parentUnit)
        throw (ComponentAlreadyExists, InvalidName);

private:
    /// Copying forbidden.
    Port(const Port&);
    /// Assingment forbidden.
    Port& operator=(const Port&);

    void loadStateWithoutReferences(const ObjectState* state)
        throw (ObjectStateLoadingException);
    void attachSocket(const std::string& socketName)
        throw (InstanceNotFound, IllegalRegistration,
               ComponentAlreadyExists, IllegalConnectivity);

    /// Name of the port.
    std::string name_;
    /// Parent unit.
    Unit* parentUnit_;
    /// Connection to the first socket.
    Socket* socket1_;
    /// Connection to the second socket.
    Socket* socket2_;
};
}

#include "Port.icc"

#endif
