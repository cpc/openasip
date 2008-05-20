/**
 * @file MachineTester.cc
 *
 * Implementation of MachineTester class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#include <string>
#include <cctype>

#include "MachineTester.hh"
#include "Socket.hh"
#include "Bus.hh"
#include "Segment.hh"
#include "Port.hh"
#include "Unit.hh"
#include "ImmediateUnit.hh"
#include "ControlUnit.hh"
#include "ContainerTools.hh"
#include "AssocTools.hh"
#include "Application.hh"

using std::set;
using std::string;
using std::pair;
using namespace TTAMachine;

/**
 * Constructor.
 *
 * @param machine The machine on which the tests are done.
 */
MachineTester::MachineTester(const Machine& machine) :
    machine_(&machine) {

    clearState();
}


/**
 * Destructor.
 */
MachineTester::~MachineTester() {
}


/**
 * Tests if the given socket can be connected to the given segment.
 *
 * After calling this function, the reason for a test failure can be
 * queried. Functions for querying the possible reasons are:
 * connectionExists, illegalRegistration, sameDirSocketConnection,
 * forbiddenSocketDirection.
 *
 * Note that only the first reason encountered is recorded.
 *
 * @param socket The socket.
 * @param segment The segment.
 * @return True if the connection can be made.
 */
bool
MachineTester::canConnect(const Socket& socket, const Segment& segment) {

    clearState();

    if (socket.machine() != machine_) {
        illegalRegistration_ = true;
        return false;
    }
    if (segment.parentBus()->machine() != machine_) {
        illegalRegistration_ = true;
        return false;
    }
    if (socket.isConnectedTo(segment)) {
        connectionExists_ = true;
	return false;
    }

    // check that the direction of the socket can be set if needed
    if (socket.segmentCount() == 0) {
        if (!legalPortConnections(socket, Socket::INPUT) &&
            !legalPortConnections(socket, Socket::OUTPUT)) {
            return false;
        }
    }

    return true;
}


/**
 * Checks if the given socket can be connected to the given port.
 *
 * If false is returned, the reason for the test failure can be queried
 * using the following functions: illegalRegistration, connectionExists,
 * wrongSocketDirection, maxConnections.
 *
 * @param socket The socket.
 * @param port The port.
 * @return True if the connection can be made, otherwise false.
 */
bool
MachineTester::canConnect(const Socket& socket, const Port& port) {

    clearState();

    // check registration
    if (socket.machine() != machine_ ||
        port.parentUnit()->machine() != machine_) {
        illegalRegistration_ = true;
        return false;
    }

    // check the existing connections of the port
    Socket* inputSocket = port.inputSocket();
    Socket* outputSocket = port.outputSocket();
    Socket* firstUnconnected = port.unconnectedSocket(0);
    Socket* secondUnconnected = port.unconnectedSocket(1);

    if (inputSocket == &socket || outputSocket == &socket ||
        firstUnconnected == &socket || secondUnconnected == &socket) {
        connectionExists_ = true;
        return false;
    }

    if ((inputSocket != NULL && socket.direction() == Socket::INPUT) ||
        (outputSocket != NULL && socket.direction() == Socket::OUTPUT)) {
        wrongSocketDirection_ = true;
        return false;
    }
    if (port.socketCount() == 2) {
        maxConnections_ = true;
        return false;
    }

    // check the special cases of allowed socket directions
    Unit* parentUnit = port.parentUnit();
    if (dynamic_cast<ImmediateUnit*>(parentUnit) != NULL) {
        assert(port.socketCount() <= 1);
        if (port.socketCount() == 1) {
            maxConnections_ = true;
            return false;
        }
        if (socket.direction() == Socket::INPUT) {
            wrongSocketDirection_ = true;
            return false;
        }
    }

    // ports of a register file cannot be connected to same socket
    RegisterFile* parentRf = dynamic_cast<RegisterFile*>(parentUnit);
    if (parentRf != NULL) {
        for (int i = 0; i < parentRf->portCount(); i++) {
            Port* rfPort = parentRf->port(i);
            if (rfPort->isConnectedTo(socket)) {
                rfPortAlreadyConnected_ = true;
                return false;
            }
        }
    }

    return true;
}


/**
 * Checks if the given source and destination buses can be joined by a new
 * bridge.
 *
 * If the test has negative outcome, the reason for it can be queried using
 * the following functions: illegalRegistration, loop, connectionExists,
 * branchedBus.
 *
 * @param source The source bus of the bridge.
 * @param destination The destination bus of the bridge.
 * @return True if the can be created, otherwise false.
 */
bool
MachineTester::canBridge(const Bus& source, const Bus& destination) {

    clearState();

    // check registration of the buses
    if (source.machine() != machine_ || destination.machine() != machine_) {
        illegalRegistration_ = true;
        return false;
    }

    if (&source == &destination) {
        return false;
    }

    // check if there already exists an equal bridge
    if (source.canWrite(destination)) {
        connectionExists_ = true;
        return false;
    } else if (destination.canWrite(source)) {
        // if an opposite bridge already exists between the buses, the new
        // bridge can be created
        return true;
    }

    // check if a loop would be created

    // Buses cannot be joined by a new bridge if they are already in the same
    // chain and are not adjacent. If they are adjacent then it is possible
    // to join them in order the create a bidirection bridge.
    if (areInSameChain(source, destination)) {
        bool adjacent = false;
        if (source.hasNextBus()) {
            if (source.nextBus() == &destination) {
                adjacent = true;
            }
        }
        if (source.hasPreviousBus()) {
            if (source.previousBus() == &destination) {
                adjacent = true;
            }
        }
        if (!adjacent) {
            loop_ = true;
            return false;
        }
    }

    // check that a branch will not be created
    if (source.hasNextBus() && source.hasPreviousBus()) {
        if (source.nextBus() != &destination &&
            source.previousBus() != &destination) {
            branchedBus_ = &source;
            return false;
        }
    }
    if (destination.hasNextBus() && destination.hasPreviousBus()) {
        if (destination.nextBus() != &source &&
            destination.previousBus() != &source) {
            branchedBus_ = &destination;
            return false;
        }
    }

    return true;
}


/**
 * Checks if the direction of the given socket can be set to the given value.
 *
 * Takes into account the ports that are directly connected to the given
 * socket only. The reason for error can be queried for using the following
 * functions: unknownSocketDirection, noConnections, sameDirSocketConnection,
 * forbiddenSocketDirection.
 *
 * @param socket The socket.
 * @param direction The new direction of the socket.
 * @return True if the direction can be set, otherwise false.
 */
bool
MachineTester::canSetDirection(
    const Socket& socket,
    Socket::Direction direction) {

    clearState();

    if (direction == Socket::UNKNOWN) {
        unknownDir_ = true;
        return false;
    }
    if (socket.segmentCount() == 0) {
        noConnections_ = true;
        return false;
    }
    if (direction == socket.direction()) {
        return true;
    }

    return legalPortConnections(socket, direction);
}


/**
 * Checks whether the given string is a valid machine component name.
 *
 * @param name The name to check.
 * @return True if the name is valid, otherwise false.
 */
bool
MachineTester::isValidComponentName(const std::string& name) {

    if (name.length() == 0) {
        return false;
    }

    for (unsigned int i = 0; i < name.length(); i++) {
        char character = name[i];
        if (i == 0) {
            if (!isalpha(character)) {
                return false;
            }
        } else {
            if (!isalpha(character) && !isdigit(character) &&
                character != '_' && character != ':') {
                return false;
            }
        }
    }
    return true;
}


/**
 * Returns true if the reason for failure of the previous test was that a
 * connection exists already between the components.
 *
 * @return True if a connection exists, otherwise false.
 */
bool
MachineTester::connectionExists() const {
    return connectionExists_;
}


/**
 * Returns true if the reason for failure of the previous test was illegal
 * registration of components.
 *
 * @return True if the components were not registered to the same machine,
 *         otherwise false.
 */
bool
MachineTester::illegalRegistration() const {
    return illegalRegistration_;
}


/**
 * If the reason for failure when tried to join two buses by a bridge was
 * loop in bus chain, returns true. Otherwise returns false.
 *
 * @return True, if loop was the reason for failure.
 */
bool
MachineTester::loop() const {
    return loop_;
}


/**
 * If the reason for failure when tried to join two buses by a bridge was
 * a branched bus chain, returns the bus that was going to be branched.
 * Otherwise returns NULL.
 *
 * @return The branched bus.
 */
Bus*
MachineTester::branchedBus() const {
    if (branchedBus_ == NULL) {
        return NULL;
    } else {
        return const_cast<Bus*>(branchedBus_);
    }
}


/**
 * Checks if wrong direction of socket was the reason for failure when tried
 * to attach a port to a socket.
 *
 * @return True, if wrong direction of socket was the reason, otherwise
 *         false.
 */
bool
MachineTester::wrongSocketDirection() const {
    return wrongSocketDirection_;
}


/**
 * Checks if maximum number of connections in port was the reason for
 * failure when tried to attach a port to a socket.
 *
 * @return True, if maximum number of connection was the reason, otherwise
 *         false.
 */
bool
MachineTester::maxConnections() const {
    return maxConnections_;
}


/**
 * Returns true if the reason for failure when tried to set socket direction
 * was unknown direction. The direction of socket can never be set to
 * unknown.
 *
 * @return True if the reason for failure was unknown socket direction,
 *         otherwise false.
 */
bool
MachineTester::unknownSocketDirection() const {
    return unknownDir_;
}


/**
 * Returns true if the reason for failure when tried to set socket direction
 * was that the socket has no connections to segments.
 *
 * @return True if the socket has no connections to segments, otherwise
 *         false.
 */
bool
MachineTester::noConnections() const {
    return noConnections_;
}


/**
 * If the reason for failure when tried to set socket direction was that the
 * socket was connected to a port that was connected to another socket that
 * has the same direction, returns that port. Otherwise returns NULL.
 *
 * @param direction The direction that was tried to set.
 * @return The port.
 */
Port*
MachineTester::sameDirSocketConnection(Socket::Direction direction) const {
    if (direction == Socket::INPUT) {
        return sameDirInputSocketConn_;
    } else if (direction == Socket::OUTPUT) {
        return sameDirOutputSocketConn_;
    } else {
        return NULL;
    }
}


/**
 * If the reason for failure when tried to set socket direction was that the
 * socket was connected to a port of a unit that doesn't allow sockets of
 * such direction to be connected, returns the port. Otherwise returns NULL.
 *
 * @param direction The direction that was tried to set.
 * @return The port.
 */
Port*
MachineTester::forbiddenSocketDirection(Socket::Direction direction) const {
    if (direction == Socket::INPUT) {
        return forbiddenInputSocketDir_;
    } else {
        return NULL;
    }
}


/**
 * Tells whether the reason for failure when tried to attach a port to a
 * socket was that the parent unit of the port was register file and a port
 * of the register file was already connected to the socket.
 *
 * @return True if that was the reason, otherwise false.
 */
bool
MachineTester::registerFilePortAlreadyConnected() const {
    return rfPortAlreadyConnected_;
}


/**
 * Clears the state of the object.
 */
void
MachineTester::clearState() {
    connectionExists_ = false;
    rfPortAlreadyConnected_ = false;
    illegalRegistration_ = false;
    loop_ = false;
    branchedBus_ = NULL;
    wrongSocketDirection_ = false;
    maxConnections_ = false;
    unknownDir_ = false;
    noConnections_ = false;
    sameDirInputSocketConn_ = NULL;
    sameDirOutputSocketConn_ = NULL;
    forbiddenInputSocketDir_ = NULL;
}


/**
 * Checks if the given buses are in the same bus chain.
 *
 * @param bus1 The first bus.
 * @param bus2 The second bus.
 * @return True if they are in the same chain, otherwise false.
 */
bool
MachineTester::areInSameChain(const Bus& bus1, const Bus& bus2) {

    if (&bus1 == &bus2) {
        return true;
    }

    const Bus* bus = &bus1;
    while (bus->hasPreviousBus()) {
        bus = bus->previousBus();
        if (bus == &bus2) {
            return true;
        }
    }

    bus = &bus1;
    while (bus->hasNextBus()) {
        bus = bus->nextBus();
        if (bus == &bus2) {
            return true;
        }
    }

    return false;
}


/**
 * Checks if the given socket will have legal connections to ports if its
 * direction is changed to the given value.
 *
 * @param socket The socket to test.
 * @param direction The direction to test.
 * @return True if the connections will stay legal, otherwise false.
 */
bool
MachineTester::legalPortConnections(
    const Socket& socket,
    Socket::Direction direction) {

    for (int i = 0; i < socket.portCount(); i++) {
        Port* port = socket.port(i);
        if (direction == Socket::INPUT && port->inputSocket() != NULL) {
            sameDirInputSocketConn_ = port;
            return false;
        } else if (direction == Socket::OUTPUT &&
                   port->outputSocket() != NULL) {
            sameDirOutputSocketConn_ = port;
            return false;
        }
        Unit* parent = port->parentUnit();
        if (dynamic_cast<ImmediateUnit*>(parent) != NULL &&
            direction == Socket::INPUT) {
            forbiddenInputSocketDir_ = port;
            return false;
        }
    }

    return true;
}
