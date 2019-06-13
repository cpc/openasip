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
