/**
 * @file DisassemblyFUPort.cc
 *
 * Implementation of DisassemblyFUPort class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyFUPort.hh"

/**
 * The constructor.
 *
 * @param fuName Name of the function unit.
 * @param port Name of the function unit port.
 */
DisassemblyFUPort::DisassemblyFUPort(std::string fuName, std::string port):
    DisassemblyElement(),
    fuName_(fuName),
    port_(port) {
}


/**
 * The destructor.
 */
DisassemblyFUPort::~DisassemblyFUPort() {
}


/**
 * Returns disassembly of the function unit port.
 *
 * @return Disassembly of the function unit port as a string.
 */
std::string
DisassemblyFUPort::toString() const {
    return fuName_ + "." + port_;
}
