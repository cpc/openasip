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
 * @file DisassemblyInstructionSlot.cc
 *
 * Implementation of DisassemblyInstructionSlot class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "DisassemblyInstructionSlot.hh"
#include "DisassemblyAnnotation.hh"

/**
 * The Constructor.
 */
DisassemblyInstructionSlot::DisassemblyInstructionSlot() {
}


/**
 * The destructor.
 */
DisassemblyInstructionSlot::~DisassemblyInstructionSlot() {
    for (int i = 0; i < annotationCount(); i++) {
        delete &(annotation(i));
    }
    annotationes_.clear();
}

/**
 * Returns annotation of requested index.
 *
 * @param index Index of annotation to return.
 * @return Annotation of requested index.
 */
DisassemblyAnnotation& 
DisassemblyInstructionSlot::annotation(int index) const {
    return *(annotationes_.at(index));
}

/**
 * Adds an annotation to instruction slot.
 *
 * @param annotation Annotation to add.
 */
void 
DisassemblyInstructionSlot::addAnnotation(DisassemblyAnnotation* annotation) {
    annotationes_.push_back(annotation);
}

/**
 * Returns the number of annotationes stored for  the slot.
 *
 * @return The number of annotationes stored for  the slot.
 */
int 
DisassemblyInstructionSlot::annotationCount() const {
    return static_cast<int>(annotationes_.size());
}
