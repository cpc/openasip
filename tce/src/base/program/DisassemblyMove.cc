/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file DisassemblyMove.cc
 *
 * Implementation of DisassemblyMove class.
 *
 * @author Veli-Pekka Jääskeläinen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2008 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyMove.hh"
#include "DisassemblyGuard.hh"

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
        guard_ = NULL;
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
   
    disassembly = 
        disassembly + source_->toString() + " -> " + destination_->toString();

    for (int i = 0; i < annotationCount(); i++) {
        disassembly += annotation(i).toString();
    }
    
    return disassembly;
}
