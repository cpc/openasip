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
