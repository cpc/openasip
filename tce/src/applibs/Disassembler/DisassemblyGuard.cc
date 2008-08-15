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
 * @file DisassemblyGuard.cc
 *
 * Implementation of DisassemblyGuard class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyGuard.hh"

/**
 * The constructor.
 *
 * @param guardedElement The element which the guard refers to.
 * @param inverted Defines if the guard is inverted or not.
 */
DisassemblyGuard::DisassemblyGuard(
    DisassemblyElement* guardedElement, bool inverted):
    inverted_(inverted),
    guardedElement_(guardedElement) {
}


/**
 * The destructor.
 */
DisassemblyGuard::~DisassemblyGuard() {
    delete guardedElement_;
}


/**
 * True, if the guard is inverted.
 *
 * @return True if the guard is inverted, false if not.
 */
bool
DisassemblyGuard::inverted() const {
    return inverted_;
}


/**
 * Returns the element which the guard refers to.
 *
 * @return Guarded element.
 */
const DisassemblyElement&
DisassemblyGuard::guardedElement() const {
    return *guardedElement_;
}


/**
 * Returns disassembly of the guard.
 *
 * @return Disassembly of the guard as a string.
 */
std::string
DisassemblyGuard::toString() const {
    std::string disassembly;
    if (inverted_) {
	disassembly = "!";
    } else {
	disassembly = "?";
    }

    disassembly = disassembly + guardedElement_->toString() + " ";
    return disassembly;
}
