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
 * @file TransportPath.cc
 *
 * Implementation of TransportPath class
 *
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
