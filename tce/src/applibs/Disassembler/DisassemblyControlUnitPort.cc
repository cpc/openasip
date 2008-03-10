/**
 * @file DisassemblyControlUnitPort.cc
 *
 * Implementation of DisassemblyControlUnitPort class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyControlUnitPort.hh"

/**
 * The constructor.
 *
 * @param gcuName Name of the control unit.
 */
DisassemblyControlUnitPort::DisassemblyControlUnitPort(std::string port):
    DisassemblyElement(), port_(port) {
}


/**
 * The destructor.
 */
DisassemblyControlUnitPort::~DisassemblyControlUnitPort() {
}


/**
 * Returns disassembly of the control unit port.
 *
 * @return Disassembly of the control unit port as a string.
 */
std::string
DisassemblyControlUnitPort::toString() const {
    return port_;
}
