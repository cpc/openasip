/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file Segment.cc
 *
 * Implementation of Segment class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 14 Jun 2004 by am, tr, ao, ll
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>

#include "Segment.hh"
#include "Bus.hh"
#include "Socket.hh"
#include "Connection.hh"
#include "MOMTextGenerator.hh"
#include "MachineTester.hh"
#include "MachineTestReporter.hh"
#include "Application.hh"
#include "ContainerTools.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;

namespace TTAMachine {

// declaration of constant strings used in loadState and saveState
const string Segment::OSNAME_SEGMENT = "segment";
const string Segment::OSKEY_NAME = "name";
const string Segment::OSKEY_DESTINATION = "destination";

/**
 * Constructor.
 *
 * The segment is appended at the end of the segment chain of its parent
 * bus.
 *
 * @param name Name of the segment.
 * @param parent The bus this segment belongs to (parent bus).
 * @exception ComponentAlreadyExists If a segment with the same name already
 *                                   exists in the same segment chain.
 * @exception InvalidName If the given name is not a valid component name.
 */
Segment::Segment(const string& name, Bus& parent)
    throw (ComponentAlreadyExists, InvalidName) :
    name_(name), parent_(NULL), sourceSegment_(NULL),
    destinationSegment_(NULL) {

    if (!MachineTester::isValidComponentName(name)) {
        const string procName = "Segment::Segment";
        throw InvalidName(__FILE__, __LINE__, procName);
    }

    parent.addSegment(*this);
    parent_ = &parent;

    for (int i = 0; i < parent.segmentCount(); i++) {
        Segment* segment = parent.segment(i);
        if (segment->destinationSegment_ == NULL && segment != this) {
            segment->destinationSegment_ = this;
            sourceSegment_ = segment;
            break;
        }
    }
}


/**
 * Constructor.
 *
 * Creates a skeleton segment with name only. This method should be called
 * by Bus::loadStateWithoutReferences only. Do not use this constructor.
 *
 * @param state The ObjectState instance from which the name is loaded.
 * @param parent The bus this segment belongs to (parent bus).
 * @exception ComponentAlreadyExists If a segment with the same name already
 *                                   exists in the given parent bus.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
Segment::Segment(const ObjectState* state, Bus& parent)
    throw (ComponentAlreadyExists, ObjectStateLoadingException) :
    name_(""), parent_(NULL), sourceSegment_(NULL),
    destinationSegment_(NULL) {

    loadStateWithoutReferences(state);
    parent.addSegment(*this);
    parent_ = &parent;
}


/**
 * Destructor.
 *
 * Detaches all the sockets before destruction.
 */
Segment::~Segment() {

    Bus* parentBus = parent_;
    parent_ = NULL;
    parentBus->removeSegment(*this);

    detachAllSockets();

    if (sourceSegment_ != NULL) {
        sourceSegment_->destinationSegment_ = destinationSegment_;
    }
    if (destinationSegment_ != NULL) {
        destinationSegment_->sourceSegment_ = sourceSegment_;
    }
}


/**
 * Sets the name of the segment.
 *
 * @param name Name of the segment.
 * @exception ComponentAlreadyExists If a segment with the given name already
 *                                   exists in the segment chain of the
 *                                   parent bus.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
Segment::setName(const string& name)
    throw (ComponentAlreadyExists, InvalidName) {

    if (name == this->name()) {
        return;
    }

    const string procName = "Segment::setName";

    if (!MachineTester::isValidComponentName(name)) {
        throw InvalidName(__FILE__, __LINE__, procName);
    }

    Bus* parent = parentBus();
    for (int i = 0; i < parent->segmentCount(); i++) {
        Segment* segment = parent->segment(i);
        if (segment->name() == name) {
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        }
    }

    name_ = name;
}


/**
 * Connects a socket to the segment.
 *
 * @param socket Socket to be connected.
 * @exception IllegalRegistration If socket and segment do not belong to the
 *                                same machine.
 * @exception IllegalConnectivity If the connection would violate a
 *                                connectivity constraint.
 */
void
Segment::attachSocket(Socket& socket)
    throw (IllegalRegistration, IllegalConnectivity) {

    parentBus()->ensureRegistration(socket);

    if (socket.isConnectedTo(*this)) {
        if (!isConnectedTo(socket)) {
            const Connection* conn = &(socket.connection(*this));
            connections_.push_back(conn);
        } else {
            MachineTester& tester = parentBus()->machine()->machineTester();
            assert(!tester.canConnect(socket, *this));
            MachineTestReporter reporter;
            string errorMsg =
                reporter.socketSegmentConnectionError(socket, *this, tester);
            string procName = "Segment::attachSocket";
            throw IllegalConnectivity(
                __FILE__, __LINE__, procName, errorMsg);
        }
    } else {
        socket.attachBus(*this);
    }
}


/**
 * Detaches socket from the segment.
 *
 * @param socket Socket which is detached.
 * @exception InstanceNotFound If the segment is not attached to the given
 *                             socket.
 */
void
Segment::detachSocket(Socket& socket)
    throw (InstanceNotFound) {

    if (!isConnectedTo(socket)) {
        string procName = "Segment::detachSocket";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    const Connection& conn = connection(socket);
    removeConnection(&conn);

    if (socket.isConnectedTo(*this)) {
        socket.detachBus(*this);
        delete &conn;
    }
}


/**
 * Detaches all the sockets attached to the segment.
 */
void
Segment::detachAllSockets() {
    ConnectionTable::iterator iter = connections_.begin();
    while (iter != connections_.end()) {
        Socket* socket = (*iter)->socket();
        detachSocket(*socket); // removes the socket from connections_
        iter = connections_.begin();
    }
}


/**
 * Returns the Connection object which joins this segment and the given
 * socket.
 *
 * The connection must exist before calling this method. This method is not
 * intended for clients. Do not use this method.
 *
 * @param socket Socket which is attached to this segment.
 * @return Connection object which joins the socket and the segment.
 */
const Connection&
Segment::connection(const Socket& socket) const {

    ConnectionTable::const_iterator iter = connections_.begin();
    while (iter != connections_.end()) {
        if ((*iter)->socket() == &socket) {
            return **iter;
        } else {
            iter++;
        }
    }
    assert(false);

    // this return statement is only to avoid warning in Solaris environment
    return **iter;
}


/**
 * Checks whether the segment is connected to the given socket.
 *
 * @param socket Socket.
 * @return True if connected, otherwise false.
 */
bool
Segment::isConnectedTo(const Socket& socket) const {

    ConnectionTable::const_iterator iter = connections_.begin();
    while (iter != connections_.end()) {
        if ((*iter)->socket() == &socket) {
            return true;
        } else {
            iter++;
        }
    }

    return false;
}


/**
 * Returns the connected socket by the given index.
 *
 * The index must be greater or equal to 0 and less than the number of
 * socket connections.
 *
 * @param index Index.
 * @return The connected socket by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
Socket*
Segment::connection(int index) const
    throw (OutOfRange) {

    if (index < 0 || index >= connectionCount()) {
        string procName = "Segment::connection";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return connections_[index]->socket();
}


/**
 * Moves the segment to the front of the given segment in the segment chain.
 *
 * The successor of this segment will become the successor of the current
 * predecessor of this segment. The current predecessor of the given segment
 * will become the predecessor of this segment.
 *
 * @param segment The segment in front of which the segment is moved.
 * @exception IllegalRegistration If the given segment is in different bus
 *                                than this segment.
 */
void
Segment::moveBefore(Segment& segment)
    throw (IllegalRegistration) {

    if (parentBus() != segment.parentBus()) {
        string procName = "Segment::moveBefore";
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    // detach from chain
    if (destinationSegment_ != NULL) {
        destinationSegment_->sourceSegment_ = sourceSegment_;
    }
    if (sourceSegment_ != NULL) {
        sourceSegment_->destinationSegment_ = destinationSegment_;
    }

    // attach to new position
    if (segment.sourceSegment_ != NULL) {
        segment.sourceSegment_->destinationSegment_ = this;
    }

    sourceSegment_ = segment.sourceSegment_;
    destinationSegment_ = &segment;

    segment.sourceSegment_ = this;
}


/**
 * Moves the segment behind the given segment in the segment chain.
 *
 * The successor of this segment will become the successor of the current
 * predecessor of this segment. The current successor of the given segment
 * will become the successor the this segment.
 *
 * @param segment The segment behind of which the segment is moved.
 * @exception IllegalRegistration If the given segment is in different bus
 *                                than this segment.
 */
void
Segment::moveAfter(Segment& segment)
    throw (IllegalRegistration) {

    if (parentBus() != segment.parentBus()) {
        string procName = "Segment::moveAfter";
        throw IllegalRegistration(__FILE__, __LINE__, procName);
    }

    // detach from chain
    if (destinationSegment_ != NULL) {
        destinationSegment_->sourceSegment_ = sourceSegment_;
    }
    if (sourceSegment_ != NULL) {
        sourceSegment_->destinationSegment_ = destinationSegment_;
    }

    // attach to new position
    if (segment.destinationSegment_ != NULL) {
        segment.destinationSegment_->sourceSegment_ = this;
    }

    sourceSegment_ = &segment;
    destinationSegment_ = segment.destinationSegment_;

    segment.destinationSegment_ = this;
}


/**
 * Returns true if the segment has a source segment, otherwise false.
 *
 * @return True if the segment has a source segment, otherwise false.
 */
bool
Segment::hasSourceSegment() const {
    return (sourceSegment_ != NULL);
}


/**
 * Returns true if the segment has a destination segment, otherwise false.
 *
 * @return True if the segment has a destination segment, otherwise false.
 */
bool
Segment::hasDestinationSegment() const {
    return (destinationSegment_ != NULL);
}


/**
 * Removes connection from the connection table.
 *
 * @param connection Connection to be removed.
 */
void
Segment::removeConnection(const Connection* connection) {
    ContainerTools::removeValueIfExists(connections_, connection);
}


/**
 * Saves the contents of the segment to an ObjectState object.
 *
 * @return The created ObjectState object.
 */
ObjectState*
Segment::saveState() const {

    ObjectState* segmentState = new ObjectState(OSNAME_SEGMENT);
    segmentState->setAttribute(OSKEY_NAME, name());
    if (destinationSegment_ != NULL) {
        segmentState->setAttribute(
            OSKEY_DESTINATION, destinationSegment_->name());
    }

    return segmentState;
}


/**
 * Loads the state of the segment from the given ObjectState instance.
 *
 * Loads only name of the segment. Does not load connections to
 * sockets. The destination of the segment and the order the
 * segments in the chain is set later, in Bus::loadState.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the parent bus has a segment
 *                                        with the same name that is
 *                                        assigned to this segment or if the
 *                                        ObjectState tree is invalid.
 */
void
Segment::loadState(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    string procName = "Segment::loadState";
    MOMTextGenerator textGenerator;

    if (state->name() != OSNAME_SEGMENT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    detachAllSockets();
    loadStateWithoutReferences(state);
}


/**
 * Loads the state of the object from the given ObjectState instance without
 * references to other machine parts.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
void
Segment::loadStateWithoutReferences(const ObjectState* state)
    throw (ObjectStateLoadingException) {

    const string procName = "Segment::loadStateWithoutReferences";

    if (state->name() != OSNAME_SEGMENT) {
        throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
    }

    try {
        string name = state->stringAttribute(OSKEY_NAME);
        if (!MachineTester::isValidComponentName(name)) {
            throw InvalidName(__FILE__, __LINE__, procName);
        }
        if (parent_ != NULL && parent_->hasSegment(name) && 
            this->name() != name) {
            throw ObjectStateLoadingException(__FILE__, __LINE__, procName);
        }
        name_ = name;
    } catch (const Exception& exception) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, exception.errorMessage());
    }
}

}
