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
    static const int UNIT_WIDTH = 85;
    /// Minimum unit height.
    static const int UNIT_HEIGHT = 45;
    /// Space between units.
    static const int UNIT_SPACE = 24;
    /// Maximun unit name width in pixels.
    static const int MAX_UNIT_NAME_WIDTH = 250;
    /// Width of a port.
    static const int PORT_WIDTH = 16;
    /// Space between ports.
    static const int PORT_SPACE = 14;
    /// Width of a socket.
    static const int SOCKET_WIDTH = PORT_WIDTH;
    /// Height of a socket.
    static const int SOCKET_HEIGHT = 100;
    /// Space dividing unconnected sockets.
    static const int SOCKET_SPACE = SOCKET_WIDTH;
    /// Space dividing connected sockets.
    static const int SOCKET_MIN_SPACE = 2;
    /// Space dividing segments.
    static const int SEGMENT_SPACE = 14;
    /// Height of a segment.
    static const int SEGMENT_HEIGHT = 4;
    /// Minimum width of a bus.
    static const int BUS_MIN_WIDTH = 206;
    /// Minimum height of a bus.
    static const int BUS_MIN_HEIGHT = SEGMENT_HEIGHT;
    /// Space between buses and bus chains.
    static const int BUS_SPACE = 30;
    /// Width of a bridge.
    static const int BRIDGE_WIDTH = BUS_MIN_WIDTH;
    /// Height of a bridge.
    static const int BRIDGE_HEIGHT = 30;
    /// Space dividing buses and bridges.
    static const int BRIDGE_SPACE = 12;
    /// Top margin for the whole processor view.
    static const int VIEW_MARGIN = 30;
    /// Left margin.
    static const int VIEW_LEFT_MARGIN = 50;
    /// Space reserved for port-socket-connections.
    static const int CONNECTIONS_SPACE = 80;
    /// Size of the sockets' direction triangle.
    static const int TRIANGLE_HEIGHT = 15;
};

#endif
