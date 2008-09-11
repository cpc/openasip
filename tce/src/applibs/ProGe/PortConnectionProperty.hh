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
