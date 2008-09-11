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
 * @file Port.cc
 *
 * Implementation of Port class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 17 Jun 2004 by jn, pj, jm, ll
 * @note rating: red
 */

#include "Port.hh"
#include "Unit.hh"
#include "Socket.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"
#include "MachineTester.hh"
#include "MOMTextGenerator.hh"
#include "MachineTestReporter.hh"
#include "Application.hh"

using std::string;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string Port::OSNAME_PORT = "port";
const string Port::OSKEY_NAME = "name";
const string Port::OSKEY_FIRST_SOCKET = "1_socket";
const string Port::OSKEY_SECOND_SOCKET = "2_socket";


/**
 * Constructor.
 *
 * @param name Name of the port.
 * @param parentUnit Unit to which the port is connected.
 * @exception ComponentAlreadyExists If the parent unit already has another
 *                                   port by the same name.
 * @exception InvalidName If the given name is not a valid name for a
 *                        component.
 */
Port::Port(const std::string& name, Unit& parentUnit)
    throw (ComponentAlreadyExists, InvalidName) :
    SubComponent(), name_(name), parentUnit_(NULL), socket1_(NULL),
    socket2_(NULL) {

    if (!MachineTester::isValidComponentName(name)) {
        const string procName = "Port::Port";
        throw InvalidName(__FILE__, __LINE__, procName);
    }

    parentUnit.addPort(*this);
    parentUnit_ = &parentUnit;
}


/**
 * Constructor.
 *
 * Loads its state from the given ObjectState instance but does not create
 * connections to sockets.
 *
 * @param state The ObjectState instance.
 * @param parentUnit The unit which contains the port.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
Port::Port(const ObjectState* state, Unit& parentUnit)
    throw (ObjectStateLoadingException) :
    SubComponent(), name_(""), parentUnit_(NULL), socket1_(NULL),
    socket2_(NULL) {

    loadStateWithoutReferences(state);
    try {
        parentUnit.addPort(*this);
    } catch (const ComponentAlreadyExists&) {
        const string procName = "Port::Port";
        MOMTextGenerator textGenerator;
        format errorMsg = textGenerator.text(
            MOMTextGenerator::TXT_PORT_EXISTS_BY_NAME);
        errorMsg % name() % parentUnit.name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    }
    parentUnit_ = &parentUnit;
}


/**
 * Destructor.
 */
Port::~Port() {

    Unit* parent = parentUnit_;

    parentUnit_ = NULL;
    assert(parent != NULL);
    parent->removePort(*this);

    if (socket1_ != NULL) {
        detachSocket(*socket1_);
    }

    if (socket2_ != NULL) {
        detachSocket(*socket2_);
    }
}


/**
 * Returns the name of the port.
 *
 * @return Name of the port.
 */
std::string
Port::name() const {
    return name_;
}


/**
 * Sets the name of the port.
 *
 * @param name The new name.
 * @exception ComponentAlreadyExists If a port with the given name already
 *                                   exists in the same unit.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
Port::setName(const std::string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (name == this->name()) {
        return;
    }

    const string procName = "Port::setName";

    if (!MachineTester::isValidComponentName(name)) {
        throw InvalidName(__FILE__, __LINE__, procName);
    }

    Unit* parent = this->parentUnit();
    for (int i = 0; i < parent->portCount(); i++) {
        Port* port = parent->port(i);
        if (port->name() == name) {
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        }
    }

    name_ = name;
}


/**
 * Attaches a socket to this port.
 *
 * NOTE! If the parent unit is a register file, attaching a socket may affect
 * to the maximum simultaneous reads of the register file.
 *
 * @param socket Socket to be attached.
 * @exception IllegalRegistration If the port and socket are not in the same
 *                                machine.
 * @exception ComponentAlreadyExists If the given socket is already attached.
 * @exception IllegalConnectivity If the socket cannot be attached because
 *                                of connectivity reasons.
 */
void
Port::attachSocket(Socket& socket)
    throw (IllegalRegistration, ComponentAlreadyExists,
           IllegalConnectivity) {

    parentUnit()->ensureRegistration(socket);
    string procName = "Port::attachSocket";

    MachineTester& tester = parentUnit()->machine()->machineTester();
    if (!tester.canConnect(socket, *this)) {
        if (tester.connectionExists()) {
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            string errorMsg = MachineTestReporter::
                socketPortConnectionError(socket, *this, tester);
            throw IllegalConnectivity(
                __FILE__, __LINE__, procName, errorMsg);
        }
    }

    if (socket1_ == NULL) {
        socket1_ = &socket;
    } else if (socket2_ == NULL) {
        socket2_ = &socket;
    } else {
        assert(false);
    }

    // bookeeping of Socket internal state - private Socket operation
    // reserved solely to Port class!
    socket.attachPort(*this);

    // sanity check
    if (socket2_ != NULL) {
        assert(socket1_->direction() == Socket::UNKNOWN ||
               socket1_->direction() != socket2_->direction());
    }
}


/**
 * Detaches a socket from this port.
 *
 * NOTE! If the parent unit is a register file, detaching a socket may affect
 * to the maximum simultaneous reads of the register file.
 *
 * @param socket Socket to be detached.
 * @exception InstanceNotFound If the given socket is not connected to the
 *                             port.
 */
void
Port::detachSocket(Socket& socket)
    throw (InstanceNotFound) {

    if (socket1_ == &socket) {
        socket1_ = NULL;
    } else if (socket2_ == &socket) {
        socket2_ = NULL;
    } else {
        string procName = "Port::detachSocket";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    // bookeeping of Socket internal state - private Socket operation
    // reserved solely to Port class!
    socket.detachPort(*this);
}


/**
 * Returns the input socket of the port.
 *
 * If the port does not have input socket (socket whose direction is
 * Socket::INPUT), returns NULL.
 *
 * @return Input socket of the port.
 */
Socket*
Port::inputSocket() const {
    if (socket1_ != NULL && socket1_->direction() == Socket::INPUT) {
        return socket1_;
    } else if (socket2_ != NULL && socket2_->direction() == Socket::INPUT) {
        return socket2_;
    } else {
        return NULL;
    }
}


/**
 * Returns the output socket of the port.
 *
 * If the port does not have output socket (socket whose direction is
 * Socket::OUTPUT), returns NULL.
 *
 * @return Output socket of the port.
 */
Socket*
Port::outputSocket() const {
    if (socket1_ != NULL && socket1_->direction() == Socket::OUTPUT) {
        return socket1_;
    } else if (socket2_ != NULL &&
               socket2_->direction() == Socket::OUTPUT) {
        return socket2_;
    } else {
        return NULL;
    }
}

/**
 * Returns true if this port can be used to read data from bus to the unit.
 *
 * @return True if input port.
 */
bool
Port::isInput() const {
    return inputSocket() != NULL;
}

/**
 * Returns true if this port can be used to write data from unit to bus.
 *
 * @return True if output port.
 */
bool
Port::isOutput() const {
    return outputSocket() != NULL;
}

/**
 * Returns the first or the second unconnected socket attached to this port.
 * If there is no such socket, returns NULL.
 *
 * @param index The index which indicates which of the unconnected sockets is
 *              returned. If there is only one unconnected socket, the index
 *              value 0 returns it and value 1 returns NULL.
 * @return The unconnected socket by the given index.
 * @exception OutOfRange If the given index is not 0 or 1.
 */
Socket*
Port::unconnectedSocket(int index) const
    throw (OutOfRange) {

    const string procName = "Port::unconnectedSocket";

    if (index < 0 || index > 1) {
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    if (socketCount() == 0) {
        return NULL;
    } else if (socketCount() == 1) {
        if (index == 1) {
            return NULL;
        } else if (socket1_ != NULL && socket1_->segmentCount() == 0) {
            return socket1_;
        } else if (socket2_ != NULL && socket2_->segmentCount() == 0) {
            return socket2_;
        } else {
            return NULL;
        }
    } else if (socketCount() == 2) {
        if (index == 0) {
            if (socket1_->segmentCount() == 0) {
                return socket1_;
            } else if (socket2_->segmentCount() == 0) {
                return socket2_;
            } else {
                return NULL;
            }
        } else {
            if (socket2_->segmentCount() == 0) {
                return socket2_;
            } else {
                return NULL;
            }
        }
    } else {
        string errorMsg = "Too many sockets attached to a port.";
        Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
        Application::abortProgram();

        // this return statement is just to avoid compilation warning in some
        // environments.
        return NULL;
    }
}


/**
 * Returns the number of sockets attached to the port.
 *
 * @return The number of sockets.
 */
int
Port::socketCount() const {
    if (socket1_ != NULL && socket2_ != NULL) {
        return 2;
    } else if (socket1_ != NULL || socket2_ != NULL) {
        return 1;
    } else {
        return 0;
    }
}


/**
 * Checks whether the port is connected to the given socket.
 *
 * @param socket The socket.
 * @return True if the port is connected to the socket, otherwise false.
 */
bool
Port::isConnectedTo(const Socket& socket) const {
    return (socket1_ == &socket || socket2_ == &socket);
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
Port::saveState() const {

    ObjectState* port = new ObjectState(OSNAME_PORT);
    port->setAttribute(OSKEY_NAME, name());

    // set first socket
    if (socket1_ != NULL) {
        port->setAttribute(OSKEY_FIRST_SOCKET, socket1_->name());
    } else if (socket2_ != NULL) {
        port->setAttribute(OSKEY_FIRST_SOCKET, socket2_->name());
    }

    // set second socket
    if (socket1_ != NULL && socket2_ != NULL) {
        port->setAttribute(OSKEY_SECOND_SOCKET, socket2_->name());
    }

    return port;
}


/**
 * Loads its state from the given ObjectState instance but does not create
 * connections to sockets.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
Port::loadStateWithoutReferences(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "Port::loadStateWithoutReferences";

    try {
        string name = state->stringAttribute(OSKEY_NAME);
        if (!MachineTester::isValidComponentName(name)) {
            throw InvalidName(__FILE__, __LINE__, procName);
        }
        name_ = name;
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the parent unit has a port by
 *                                        the same name as the coming name of
 *                                        this unit or if loading the
 *                                        references to sockets fails.
 */
void
Port::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "Port::loadState";
    detachAllSockets();
    loadStateWithoutReferences(state);
    string socketName = "";
    MOMTextGenerator textGenerator;

    try {
        if (state->hasAttribute(OSKEY_FIRST_SOCKET)) {
            socketName = state->stringAttribute(OSKEY_FIRST_SOCKET);
            attachSocket(socketName);
        }
        if (state->hasAttribute(OSKEY_SECOND_SOCKET)) {
            socketName = state->stringAttribute(OSKEY_SECOND_SOCKET);
            attachSocket(socketName);
        }
    } catch (const InstanceNotFound&) {
        format errorMsg = textGenerator.text(
            MOMTextGenerator::TXT_PORT_REF_LOAD_ERR_SOCKET);
        errorMsg % socketName % name() % parentUnit()->name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    } catch (const IllegalRegistration&) {
        format errorMsg = textGenerator.text(
            MOMTextGenerator::TXT_PORT_REF_LOAD_ERR_SOCKET);
        errorMsg % socketName % name() % parentUnit()->name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    } catch (const ComponentAlreadyExists&) {
        format errorMsg = textGenerator.text(
            MOMTextGenerator::TXT_PORT_EXISTS_BY_NAME);
        errorMsg % name() % parentUnit()->name();
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, errorMsg.str());
    } catch (const IllegalConnectivity& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Attaches the socket by the given name to the port.
 *
 * The given socket must be registered to the same machine as the parent
 * unit.
 *
 * @param socketName Name of the socket to be attached.
 * @exception InstanceNotFound If the given socket is not found from the
 *                             machine.
 * @exception IllegalRegistration If the parent unit is not registered to a
 *                                machine.
 * @exception ComponentAlreadyExists If a socket by the given name is already
 *                                   attached.
 * @exception IllegalConnectivity If the socket cannot be attached because
 *                                of connectivity reasons.
 */
void
Port::attachSocket(const std::string& socketName)
    throw (InstanceNotFound, IllegalRegistration, ComponentAlreadyExists,
           IllegalConnectivity) {

    const string procName = "Socket::attachSocket";

    Machine* mach = parentUnit()->machine();
    if (mach == NULL) {
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    Machine::SocketNavigator socketNav = mach->socketNavigator();
    Socket* socket = socketNav.item(socketName);
    attachSocket(*socket);
}


/**
 * Detaches all the sockets connected to the port.
 *
 * NOTE! If the parent unit is a register file, detaching sockets may affect
 * to the maximum simultaneous reads of the register file.
 */
void
Port::detachAllSockets() {
    if (socket1_ != NULL) {
        socket1_->detachPort(*this);
        socket1_ = NULL;
    }
    if (socket2_ != NULL) {
        socket2_->detachPort(*this);
        socket2_ = NULL;
    }
}


bool 
Port::PairComparator::operator()(
    const std::pair<const Port*, const Port*>& pp1, 
    const std::pair<const Port*, const Port*>& pp2)
    const {
    if (Comparator()(pp1.first, pp2.first)) {
        return false;
    }
    if (Comparator()(pp2.first, pp1.first)) {
        return true;
    }
    if (Comparator()(pp1.second, pp2.second)) {
        return true;
    }
    return false;
}

}

