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
 * @file PortConnectionProperty.cc
 *
 * Implementation of PortConnectionProperty class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
    width_(width), inverted_(false) {
}

PortConnectionProperty::PortConnectionProperty(bool inverted)
    : port1FirstBit_(0), port2FirstBit_(0), width_(0), inverted_(inverted) {
}

/**
 * The constructor.
 *
 * This constructor is used when all the bits of the ports are connected.
 *
 * @param name Name of the connection.
 */
PortConnectionProperty::PortConnectionProperty() :
    port1FirstBit_(0), port2FirstBit_(0), width_(0), inverted_(false) {
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
 * Returns true if signal between connected ports are inverted.
 */
bool
PortConnectionProperty::inverted() const {
    return inverted_;
}

/**
 * Define inversion of connection.
 *
 * If set to true the signal need to be inverted.
 */
void
PortConnectionProperty::setInverted(bool setting) {
    inverted_ = setting;
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
