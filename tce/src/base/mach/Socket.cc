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
 * @file Socket.cc
 *
 * Implementation of Socket class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 * @note rating: red
 * @note reviewed 22 Jun 2004 by ao, ml, vpj, ll
 */

#include <set>

#include "Socket.hh"
#include "Machine.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Port.hh"
#include "Connection.hh"
#include "MachineTester.hh"
#include "MOMTextGenerator.hh"
#include "MachineTestReporter.hh"
#include "Application.hh"
#include "ContainerTools.hh"

using std::string;
using std::set;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string Socket::OSNAME_SOCKET = "socket";
const string Socket::OSKEY_DIRECTION = "direction";
const string Socket::OSVALUE_INPUT = "input";
const string Socket::OSVALUE_OUTPUT = "output";
const string Socket::OSVALUE_UNKNOWN = "unknown";

/**
 * Constructor.
 *
 * @param name Name of the socket.
 * @exception InvalidName If the given name is not valid for a component.
 */
Socket::Socket(const std::string& name)
    throw (InvalidName) :
    Component(name), direction_(UNKNOWN) {
}


/**
 * Constructor.
 *
 * Loads the state of the socket from the given ObjectState instance but
 * does not create connections to other components.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
Socket::Socket(const ObjectState* state)
    throw (ObjectStateLoadingException) :
    Component(state), direction_(UNKNOWN) {
}


/**
 * Destructor.
 */
Socket::~Socket() {
    unsetMachine();
}


/**
 * Sets the name of the socket.
 *
 * @param name New name of the socket.
 * @exception ComponentAlreadyExists If a socket with the given name is
 *                                   already in the same machine.
 * @exception InvalidName If the given name is not valid for a component.
 */
void
Socket::setName(const std::string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->socketNavigator().hasItem(name)) {
            string procName = "Socket::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}


/**
 * Sets the direction of the socket.
 *
 * The given direction must be either Socket::INPUT or Socket::OUTPUT.
 *
 * @param direction The new direction.
 * @exception IllegalConnectivity If the direction of the port cannot be
 *                                changed.
 */
void
Socket::setDirection(Direction direction)
    throw (IllegalConnectivity) {

    const string procName = "Socket::setDirection";

    if (!isRegistered()) {
        MOMTextGenerator textGenerator;
        format text = textGenerator.text(
            MOMTextGenerator::TXT_SET_DIR_SOCKET_NOT_REGISTERED);
        text % name();
        throw IllegalConnectivity(__FILE__, __LINE__, procName, text.str());
    }

    MachineTester& tester = machine()->machineTester();
    if (tester.canSetDirection(*this, direction)) {
        direction_ = direction;
    } else {
        string errorMsg = MachineTestReporter::socketDirectionSettingError(
            *this, direction, tester);
        throw IllegalConnectivity(__FILE__, __LINE__, procName, errorMsg);
    }
}


/**
 * Attaches a bus to the socket.
 *
 * If no bus is currently attached, the direction of the socket is set to
 * Socket::INPUT or Socket::OUTPUT. When possible, the direction is set to
 * Socket::INPUT. Otherwise, it is set to Socket::OUTPUT.
 *
 * @param bus The segment of a bus being attached.
 * @exception IllegalRegistration If the bus is not registered to the same
 *                                machine as the socket.
 * @exception IllegalConnectivity If the socket is attached to ports in such
 *                                a way that its direction cannot be either
 *                                input or output.
 */
void
Socket::attachBus(Segment& bus)
    throw (IllegalRegistration, IllegalConnectivity) {

    const string procName = "Socket::attachBus";

    ensureRegistration(*bus.parentBus());
    MachineTester& tester = machine()->machineTester();
    if (!tester.canConnect(*this, bus)) {
        string errorMsg = MachineTestReporter::socketSegmentConnectionError(
            *this, bus, tester);
        throw IllegalConnectivity(__FILE__, __LINE__, procName, errorMsg);
    }

    if (!bus.isConnectedTo(*this)) {
        const Connection* conn = new Connection(*this, bus);
        busses_.push_back(conn);
        bus.attachSocket(*this);
    } else {
        assert(false);
    }

    // set the direction
    if (direction_ == UNKNOWN) {
        if (tester.canSetDirection(*this, INPUT)) {
            setDirection(INPUT);
        } else if (tester.canSetDirection(*this, OUTPUT)) {
            setDirection(OUTPUT);
        } else {
            string errorMsg = "Direction of the socket cannot be set.";
            Application::writeToErrorLog(
                __FILE__, __LINE__, procName, errorMsg);
            Application::abortProgram();
        }
    }
}


/**
 * Detaches the given segment of a bus from the socket.
 *
 * If there are no buses connected to the socket after detaching the given
 * segment, the direction of the socket is set to Socket::UNKNOWN. Note that
 * detaching a segment may affect to the value of maximum simultaneous
 * register reads of register files connected to the socket.
 *
 * @param bus The segment to be detached.
 * @exception InstanceNotFound If the given segment is not attached to the
 *                             socket.
 */
void
Socket::detachBus(Segment& bus)
    throw (InstanceNotFound) {

    if (!isConnectedTo(bus)) {
        string procName = "Socket::detachBus";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    const Connection& conn = connection(bus);
    removeConnection(&conn);

    if (bus.isConnectedTo(*this)) {
        bus.detachSocket(*this);
        delete &conn;
    }

    // set socket direction to unknown if it has no connections to buses
    if (segmentCount() == 0) {
        direction_ = UNKNOWN;
    }
}


/**
 * Detaches all the segments of the given bus from the socket.
 *
 * If there are no buses connected to the socket after detaching the given
 * bus, the direction of the socket is set to Socket::UNKNOWN. Note that
 * detaching the bus may affect to the value of maximum simultaneous
 * register reads of register files connected to the socket.
 *
 * @param bus Bus to be detached.
 */
void
Socket::detachBus(Bus& bus) {
    int segments = bus.segmentCount();
    for (int i = 0; i < segments; i++) {
        Segment* segment = bus.segment(i);
        if (isConnectedTo(*segment)) {
            detachBus(*segment);
        }
    }
}


/**
 * Returns a port by the given index.
 *
 * The given index must be greater or equal to 0 and less than the number
 * of ports attached to the socket.
 *
 * @param index Index.
 * @return The port found by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
Port*
Socket::port(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= portCount()) {
        string procName = "Socket::port";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return ports_[index];
}


/**
 * Detaches all ports from the socket.
 */
void
Socket::detachAllPorts() {
    PortTable::iterator iter = ports_.begin();
    while (iter != ports_.end()) {
        // removes the socket from ports_
        (*iter)->detachSocket(*this);
        iter = ports_.begin();
    }
    ports_.clear();
}




/**
 * Returns the Connection object which connects this socket and the given
 * segment.
 *
 * This method is not intended for clients. Do not use this method. The
 * connection must exist before calling this function.
 *
 * @param bus The segment of a bus which is attached to this socket.
 * @return Connection object which joins the socket and the segment.
 */
const Connection&
Socket::connection(const Segment& bus) const {

    ConnectionTable::const_iterator iter = busses_.begin();
    while (iter != busses_.end()) {
        if ((*iter)->bus() == &bus) {
            return **iter;
        } else {
            iter++;
        }
    }

    string errorMsg = "The requested Connection object does not exist in "
        "Socket.";
    string procName = "Socket::connection";
    Application::writeToErrorLog(__FILE__, __LINE__, procName, errorMsg);
    Application::abortProgram();

    // this return statement is only to avoid warning in Solaris environment
    return **iter;
}


/**
 * Checks whether the socket is connected to the given bus.
 *
 * @param bus A bus.
 * @return True if connected, otherwise false.
 */
bool
Socket::isConnectedTo(const Bus& bus) const {

    int segments = bus.segmentCount();
    for (int i = 0; i < segments; i++) {
        if (isConnectedTo(*bus.segment(i))) {
            return true;
        }
    }
    return false;
}


/**
 * Checks whether the socket is connected to the given segment.
 *
 * @param bus Segment of a bus.
 * @return True if connected, otherwise false.
 */
bool
Socket::isConnectedTo(const Segment& bus) const {

    ConnectionTable::const_iterator iter = busses_.begin();
    while (iter != busses_.end()) {
        if ((*iter)->bus() == &bus) {
            return true;
        } else {
            iter++;
        }
    }

    return false;
}


/**
 * By the given index, returns the segment a socket is connected to.
 *
 * The given index must be greater or equal to 0 and smaller than the number
 * of segments attached to the socket.
 *
 * @param index The index.
 * @return Segment by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
Segment*
Socket::segment(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= segmentCount()) {
        string procName = "Socket::segment";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return busses_[index]->bus();
}


/**
 * Registers the socket to a machine.
 *
 * @param mach Machine to which the socket is to be registered.
 * @exception ComponentAlreadyExists If there already is another socket by
 *                                   the same name in the machine.
 */
void
Socket::setMachine(Machine& mach)
    throw (ComponentAlreadyExists) {

    internalSetMachine(mach);
    mach.addSocket(*this);
}


/**
 * Removes the socket from the machine it is registered to.
 *
 * Detaches all the busses and ports attached to the socket.
 */
void
Socket::unsetMachine() {

    if (machine() == NULL) {
        return;
    }

    Machine* mach = machine();
    internalUnsetMachine();

    detachAllBuses();
    detachAllPorts();

    mach->removeSocket(*this);
}


/**
 * Saves the contents of the socket to an ObjectState object.
 *
 * @return The newly created ObjectState object.
 */
ObjectState*
Socket::saveState() const {

    ObjectState* root = Component::saveState();
    root->setName(OSNAME_SOCKET);

    // set direction
    if (direction_ == INPUT) {
        root->setAttribute(OSKEY_DIRECTION, OSVALUE_INPUT);
    } else if (direction_ == OUTPUT) {
        root->setAttribute(OSKEY_DIRECTION, OSVALUE_OUTPUT);
    } else {
        root->setAttribute(OSKEY_DIRECTION, OSVALUE_UNKNOWN);
    }

    // add connections
    ConnectionTable::const_iterator iter = busses_.begin();
    while (iter != busses_.end()) {
        const Connection* conn = *iter;
        ObjectState* bus = conn->saveState();
        root->addChild(bus);
        iter++;
    }

    return root;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the machine already has a socket
 *                                        by the same name as the new
 *                                        name of this socket or if the
 *                                        given ObjectState tree is invalid.
 */
void
Socket::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    string procName = "Socket::loadState";
    MOMTextGenerator textGenerator;

    if (state->name() != OSNAME_SOCKET) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    Component::loadState(state);

    // load connections to busses
    detachAllBuses();

    try {

        for (int childIndex = 0; childIndex < state->childCount();
             childIndex++) {
            ObjectState* child = state->child(childIndex);

            if (child->name() == Connection::OSNAME_CONNECTION) {
                string busName =
                    child->stringAttribute(Connection::OSKEY_BUS);
                string segmentName =
                    child->stringAttribute(Connection::OSKEY_SEGMENT);

                if (!isRegistered()) {
                    format text = textGenerator.text(
                        MOMTextGenerator::TXT_SOCKET_REF_LOAD_ERR);
                    text % name();
                    throw ObjectStateLoadingException(
                        __FILE__, __LINE__, procName, text.str());
                }

                Machine::BusNavigator busNav = machine()->busNavigator();
                Bus* bus = NULL;
                Segment* segment = NULL;

                try {
                    bus = busNav.item(busName);
                } catch (InstanceNotFound&) {
                    format text = textGenerator.text(
                        MOMTextGenerator::TXT_SOCKET_REF_LOAD_ERR_BUS);
                    text % busName % name();
                    throw ObjectStateLoadingException(
                        __FILE__, __LINE__, procName, text.str());
                }

                try {
                    segment = bus->segment(segmentName);
                } catch (InstanceNotFound&) {
                    format text = textGenerator.text(
                        MOMTextGenerator::TXT_SOCKET_REF_LOAD_ERR_SEGMENT);
                    text % name() % segmentName % busName;
                    throw ObjectStateLoadingException(
                        __FILE__, __LINE__, procName, text.str());
                }

                attachBus(*segment);

            } else {
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName);
            }
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }

    // load direction
    string direction("");
    try {
        direction = state->stringAttribute(OSKEY_DIRECTION);
        if (direction == OSVALUE_INPUT) {
            setDirection(INPUT);
        } else if (direction == OSVALUE_OUTPUT) {
            setDirection(OUTPUT);
        } else if (direction == OSVALUE_UNKNOWN) {
            if (segmentCount() > 0) {
                format text = textGenerator.text(
                    MOMTextGenerator::
                    TXT_UNKNOWN_SOCKET_DIR_AND_SEGMENT_CONN);
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, text.str());
            }
            direction_ = UNKNOWN;
        } else {
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
        }

    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}


/**
 * Removes a connection from the connection table.
 *
 * @param connection The connection to be removed.
 */
void
Socket::removeConnection(const Connection* connection) {
    ContainerTools::removeValueIfExists(busses_, connection);
}


/**
 * Notifies the socket that a port has been attached to it.
 *
 * Private helper method used only by friend class Port.
 *
 * @param port The port that has been attached to this socket.
 */
void
Socket::attachPort(Port& port) {
    ports_.push_back(&port);
}


/**
 * Notifies the socket that a port has been detached from it.
 *
 * Private helper method used only by friend class Port.
 *
 * @param port The port that has been detached from this socket.
 */
void
Socket::detachPort(const Port& port) {
    ContainerTools::removeValueIfExists(ports_, &port);
}


/**
 * Detaches all the buses from this socket.
 */
void
Socket::detachAllBuses() {
    while (busses_.size() > 0) {
        Segment* bus = busses_[0]->bus();
        detachBus(*bus);
    }
}
}
