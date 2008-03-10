/**
 * @file Connection.cc
 *
 * Implementation of Connection class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
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
