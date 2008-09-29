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
 * @file DisassemblyIntRegister.cc
 *
 * Implementation of DisassemblyIntRegister class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyIntRegister.hh"
#include "Conversion.hh"

/**
 * The Constructor.
 *
 * @param index Index of the register.
 */
DisassemblyIntRegister::DisassemblyIntRegister(Word index):
    DisassemblyElement(),
    index_(index) {
}


/**
 * The destructor.
 */
DisassemblyIntRegister::~DisassemblyIntRegister() {
}


/**
 * Returns disassembly of the int register.
 *
 * @return Disassembly of the int register as a string.
 */
std::string
DisassemblyIntRegister::toString() const {

    switch (index_) {
    case 0: return "ires0"; // Return value.
    case 1: return "sp";    // Stack pointer.
    // Registers for passing function parameters:
    case 2: return "iarg1";
    case 3: return "iarg2";
    case 4: return "iarg3";
    case 5: return "iarg4";
    }

    return "r" + Conversion::toString(index_);
}
