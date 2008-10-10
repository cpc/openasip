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
 * @file Connection.cc
 *
 * Implementation of Connection class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 11 Jun 2004 by am, pj, jn, ll
 * @note rating: red
 */

#include "Connection.hh"
#include "Bus.hh"
#include "Socket.hh"
#include "Segment.hh"
#include "Application.hh"

using std::string;

namespace TTAMachine {

// initialization of static data members
const string Connection::OSNAME_CONNECTION = "connection";
const string Connection::OSKEY_SOCKET = "socket";
const string Connection::OSKEY_BUS = "bus";
const string Connection::OSKEY_SEGMENT = "segment";


/**
 * Constructor.
 *
 * @param socket Socket which is connected to the given segment.
 * @param bus Segment of a bus which is connected to the socket.
 */
Connection::Connection(Socket& socket, Segment& bus) :
    socket_(&socket), bus_(&bus) {
}

/**
 * Destructor.
 *
 * If Connection object still joins a socket and a segment, they are detached
 * before destructing the Connection object.
 */
Connection::~Connection() {
    if (socket_->isConnectedTo(*bus_)) {
        assert(bus_->isConnectedTo(*socket_)); // sanity check
        socket_->detachBus(*bus_); // or bus_->detachSocket(socket_)
    } else {
        // this is for when destructor is called by a detach methods
        assert(!bus_->isConnectedTo(*socket_));
    }
}


/**
 * Saves its state to an ObjectState instance.
 *
 * @return The newly created ObjectState instance.
 */
ObjectState*
Connection::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_CONNECTION);
    state->setAttribute(OSKEY_SOCKET, socket()->name());
    state->setAttribute(OSKEY_BUS, bus()->parentBus()->name());
    state->setAttribute(OSKEY_SEGMENT, bus()->name());
    return state;
}

}
