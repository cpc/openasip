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
 * @file DisassemblyFUOpcodePort.cc
 *
 * Implementation of DisassemblyFUOpcodePort class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
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
