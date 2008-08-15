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
 * @file PortConnectionProperty.cc
 *
 * Implementation of PortConnectionProperty class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "PortConnectionProperty.hh"

namespace ProGe {

/**
 * The constructor.
 *
 * @param port1FirstBit The first bit of the first port to connect.
 * @param port2FirstBit The first bit of the second port to connect.
 * @param width The width of the connection.
 */
PortConnectionProperty::PortConnectionProperty(
    int port1FirstBit,
    int port2FirstBit,
    int width) :
    port1FirstBit_(port1FirstBit), port2FirstBit_(port2FirstBit),
    width_(width) {
}


/**
 * The constructor.
 *
 * This constructor is used when all the bits of the ports are connected.
 *
 * @param name Name of the connection.
 */
PortConnectionProperty::PortConnectionProperty() :
    port1FirstBit_(0), port2FirstBit_(0), width_(0) {
}


/**
 * The destructor.
 */
PortConnectionProperty::~PortConnectionProperty() {
}


/**
 * Returns the width of the connection.
 *
 * @return The width of the connection.
 */
int
PortConnectionProperty::width() const {
    return width_;
}


/**
 * Tells whether the ports are fully connected.
 *
 * @return True if the ports are fully connected, otherwise false.
 */
bool
PortConnectionProperty::fullyConnected() const {
    return width() == 0;
}


/**
 * Returns the lowest bit of the port1 that is connected.
 *
 * @return The lowest connected bit.
 */
int
PortConnectionProperty::port1FirstBit() const {
    return port1FirstBit_;
}


/**
 * Returns the lowest bit of the port2 that is connected.
 *
 * @return The lowest connected bit.
 */
int
PortConnectionProperty::port2FirstBit() const {
    return port2FirstBit_;
}
}
