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
#include "ObjectState.hh"

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
