/**
 * @file DisassemblyFUOpcodePort.cc
 *
 * Implementation of DisassemblyFUOpcodePort class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyFUOpcodePort.hh"
#include "StringTools.hh"

using std::string;

/**
 * The constructor.
 *
 * @param fuName Name of the function unit.
 * @param port Name of the function unit port.
 * @param opName Name of the operation.
 */
DisassemblyFUOpcodePort::DisassemblyFUOpcodePort(
    string fuName, string port, string opName):
    DisassemblyElement(),
    fuName_(fuName),
    port_(port),
    opName_(StringTools::stringToLower(opName)) {
}


/**
 * The Destructor.
 */
DisassemblyFUOpcodePort::~DisassemblyFUOpcodePort() {
}


/**
 * Returns disassembly of the opcode port.
 *
 * @return Disassembly of the opcode port as a string.
 */
string
DisassemblyFUOpcodePort::toString() const {
    return fuName_ + "." + port_ + "." + opName_;
}
