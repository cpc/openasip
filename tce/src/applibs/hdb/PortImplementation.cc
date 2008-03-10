/**
 * @file PortImplementation.cc
 *
 * Implementation of PortImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include "PortImplementation.hh"

namespace HDB {

/**
 * The constructor.
 *
 * @param name Name of the port.
 * @param loadPort Name of the load enable port.
 */
PortImplementation::PortImplementation(
    const std::string& name,
    const std::string& loadPort) :
    name_(name), loadPort_(loadPort) {
}


/**
 * The destructor.
 */
PortImplementation::~PortImplementation() {
}


/**
 * Sets the name of the port.
 *
 * @param name The new name.
 */
void
PortImplementation::setName(const std::string& name) {
    name_ = name;
}


/**
 * Returns the name of the port.
 *
 * @return The name of the port.
 */
std::string
PortImplementation::name() const {
    return name_;
}


/**
 * Sets the name of the load enable port.
 *
 * @param name The new name.
 */
void
PortImplementation::setLoadPort(const std::string& name) {
    loadPort_ = name;
}


/**
 * Returns the name of the load enable port.
 *
 * @return The name of the load enable port.
 */
std::string
PortImplementation::loadPort() const {
    return loadPort_;
}
}
