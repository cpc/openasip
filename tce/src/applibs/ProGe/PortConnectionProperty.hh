/**
 * @file PortConnectionProperty.hh
 *
 * Declaration of PortConnectionProperty class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
