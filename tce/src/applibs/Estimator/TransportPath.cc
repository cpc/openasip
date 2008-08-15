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
