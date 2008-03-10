/**
 * @file DisassemblyInstructionSlot.cc
 *
 * Implementation of DisassemblyInstructionSlot class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel@cs.tut.fi)
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
