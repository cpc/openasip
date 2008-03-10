/**
 * @file MachineCanvasLayoutConstraints.hh
 *
 * Declaration of MachineCanvasLayoutConstraints class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme@tut.fi)
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
    static const int UNIT_HEIGHT = 65;
    /// Space between units.
    static const int UNIT_SPACE = 36;
    /// Maximun unit name width in pixels.
    static const int MAX_UNIT_NAME_WIDTH = 250;
    /// Width of a port.
    static const int PORT_WIDTH = 16;
    /// Space between ports.
    static const int PORT_SPACE = 20;
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
