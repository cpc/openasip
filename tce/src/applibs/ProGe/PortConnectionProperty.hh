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
 * @file PortConnectionProperty.hh
 *
 * Declaration of PortConnectionProperty class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORT_CONNECTION_PROPERTY_HH
#define TTA_PORT_CONNECTION_PROPERTY_HH

#include <string>

namespace ProGe {

/**
 * Holds the properties of an edge of the netlist graph.
 *
 * Edges represent connections between ports of netlist blocks. A port
 * connection really consists of one or more connections between the bits of
 * two ports.
 *
 * Multi-bit ports may be connected completely or partially. In the latter
 * case, the connection involves only a subset of the bits of each port at
 * its ends.
 */
class PortConnectionProperty {
public:
    PortConnectionProperty(
        int port1FirstBit,
        int port2FirstBit,
        int width);
    PortConnectionProperty();
    virtual ~PortConnectionProperty();

    int width() const;
    bool fullyConnected() const;

    int port1FirstBit() const;
    int port2FirstBit() const;

private:
    /// The first bit of the first port to connect.
    int port1FirstBit_;
    /// The first bit of the second port to connect.
    int port2FirstBit_;
    /// The width of the connection.
    int width_;
};
}

#endif
