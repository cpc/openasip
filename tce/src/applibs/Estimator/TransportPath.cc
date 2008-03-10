/**
 * @file TransportPath.cc
 *
 * Implementation of TransportPath class
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 *
 * @note rating: red
 */

#include "TransportPath.hh"

namespace CostEstimator {
/**
 * Constructor.
 */
TransportPath::TransportPath(
    const TTAMachine::Port& sourcePort, 
    const TTAMachine::Socket& sourceSocket,
    const TTAMachine::Bus& bus,
    const TTAMachine::Socket& destinationSocket,
    const TTAMachine::Port& destinationPort) :
    sourcePort_(sourcePort), sourceSocket_(sourceSocket), bus_(bus),
    destinationSocket_(destinationSocket), destinationPort_(destinationPort) {
}

/**
 * Destructor.
 */
TransportPath::~TransportPath() {
}

/**
 * Returns the source port.
 * 
 * @return Source port.
 */
const TTAMachine::Port& 
TransportPath::sourcePort() const {
    return sourcePort_;
}

/**
 * Returns the source socket.
 *
 * @return Source socket.
 */
const TTAMachine::Socket& 
TransportPath::sourceSocket() const {
    return sourceSocket_;
}

/**
 * Returns the bus.
 *
 * @return The bus.
 */
const TTAMachine::Bus& 
TransportPath::bus() const {
    return bus_;
}

/**
 * Returns the destination socket.
 *
 * @param Destination socket.
 */
const TTAMachine::Socket& 
TransportPath::destinationSocket() const {
    return destinationSocket_;
}

/**
 * Returns the destination port.
 *
 * @param Destination port.
 */
const TTAMachine::Port& 
TransportPath::destinationPort() const {
    return destinationPort_;
}

}
