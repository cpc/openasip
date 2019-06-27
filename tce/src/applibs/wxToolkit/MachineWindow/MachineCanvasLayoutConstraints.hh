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
 * @file MachineCanvasLayoutConstraints.hh
 *
 * Declaration of MachineCanvasLayoutConstraints class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: yellow
 * @note reviewed Jul 22 2004 by tr, ml, jm, am
 */

#ifndef TTA_MACHINE_CANVAS_LAYOUT_CONSTRAINTS_HH
#define TTA_MACHINE_CANVAS_LAYOUT_CONSTRAINTS_HH

/**
 * Defines some layout constraints used in visualizing the machine.
 */
class MachineCanvasLayoutConstraints {
public:
    /// Minimum unit width.
    static const int UNIT_WIDTH;
    /// Minimum unit height.
    static const int UNIT_HEIGHT;
    /// Space between units.
    static const int UNIT_SPACE;
    /// Maximun unit name width in pixels.
    static const int MAX_UNIT_NAME_WIDTH;
    /// Default Width of a port.
    static const int PORT_WIDTH;
    /// Minimum Width of a port.
    static const int PORT_MIN_WIDTH;
    /// Constant used for port width calculation
    static const int PORT_BASE_WIDTH;
    /// Space between ports.
    static const int PORT_SPACE;
    /// Width of a socket.
    static const int SOCKET_WIDTH;
    /// Height of a socket.
    static const int SOCKET_HEIGHT;
    /// Space dividing unconnected sockets.
    static const int SOCKET_SPACE;
    /// Space dividing connected sockets.
    static const int SOCKET_MIN_SPACE;
    /// Space dividing segments.
    static const int SEGMENT_SPACE;
    /// Height of a segment.
    static const int SEGMENT_HEIGHT_BASE;
    /// Minimum width of a bus.
    static const int BUS_MIN_WIDTH;
    /// Minimum height of a bus.
    static const int BUS_MIN_HEIGHT;
    /// Space between buses and bus chains.
    static const int BUS_SPACE;
    /// Width of a bridge.
    static const int BRIDGE_WIDTH;
    /// Height of a bridge.
    static const int BRIDGE_HEIGHT;
    /// Space dividing buses and bridges.
    static const int BRIDGE_SPACE;
    /// Top margin for the whole processor view.
    static const int VIEW_MARGIN;
    /// Left margin.
    static const int VIEW_LEFT_MARGIN;
    /// Space reserved for port-socket-connections.
    static const int CONNECTIONS_SPACE;
    /// Size of the sockets' direction triangle.
    static const int TRIANGLE_HEIGHT;
};

#endif
