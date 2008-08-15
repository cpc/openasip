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
 * @file DisassemblyMove.cc
 *
 * Implementation of DisassemblyMove class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyMove.hh"

/**
 * Creates disassembler of a move.
 *
 * @param source The source element of the move.
 * @param destination The destination element of the move.
 * @param guard Move guard, or NULL if the move is not guarded.
 */
DisassemblyMove::DisassemblyMove(
    DisassemblyElement* source,
    DisassemblyElement* destination,
    DisassemblyGuard* guard):
    DisassemblyInstructionSlot(),
    source_(source), destination_(destination), guard_(guard) {

}


/**
 * The destructor.
 */
DisassemblyMove::~DisassemblyMove() {
    delete source_;
    delete destination_;
    if (guard_ != NULL) {
	delete guard_;
    }
}


/**
 * Disassembles the move.
 *
 * @return Disassembled move as a string.
 */
std::string
DisassemblyMove::toString() const {

    std::string disassembly;
    if (guard_ != NULL) {
	disassembly = guard_->toString();
    }

    disassembly = disassembly + source_->toString() + " -> " +
        destination_->toString();

    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }
    
    return disassembly;
}
