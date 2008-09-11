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
 * @file InstructionReferenceManager.cc
 *
 * Implementation of InstructionReferenceManager class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "InstructionReferenceManager.hh"
#include "InstructionReference.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// InstructionReferenceManager
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @note Should not be instantiated independent of a Program instance.
 */
InstructionReferenceManager::InstructionReferenceManager() {
}

/**
 * Destructor.  Clears all instruction references.
 */
InstructionReferenceManager::~InstructionReferenceManager() {
    clearReferences();
}

/**
 * Creates a new reference to an instruction.
 *
 * @param ins Referred instruction.
 * @return A new reference to an instruction, or if one already exists,
 *         return it.
 */
InstructionReference&
InstructionReferenceManager::createReference(Instruction& ins) {
    for (unsigned int i = 0; i < references_.size(); i++) {
        if (&references_.at(i)->instruction() == &ins) {
            return *references_.at(i);
        }
    }
    InstructionReference* newRef = new InstructionReference(ins);
    references_.push_back(newRef);
    return *newRef;
}

/**
 * Replaces a referred instruction with another.
 *
 * @param insA Instruction to be replaced.
 * @param insB The new referred instruction.
 * @return A handle to the reference to the new referred instruction.
 * @exception InstanceNotFound if the instruction to be replaced is not
 *            found.
 */
InstructionReference&
InstructionReferenceManager::replace(Instruction& insA, Instruction& insB)
    throw (InstanceNotFound) {

    for (unsigned int i = 0; i < references_.size(); i++) {
        if (&references_.at(i)->instruction() == &insA) {
            references_.at(i)->setInstruction(insB);
            return *references_.at(i);
        }
    }
    throw InstanceNotFound(
        __FILE__, __LINE__, "InstructionReferenceManager::replace()",
        "Instruction reference to be replaced not found.");
}

/**
 * Clears all instruction references. The result is a totally empty
 * instruction reference manager.
 */
void
InstructionReferenceManager::clearReferences() {
    for (unsigned int i = 0; i < references_.size(); i++) {
        delete references_.at(i);
        references_.at(i) = NULL;
    }
    references_.clear();
}

/**
 * Tells whether the manager has created a reference to the given instruction.
 *
 * @return True if the manager has created a reference to the given instruction.
 */
bool
InstructionReferenceManager::hasReference(Instruction& ins) const {
    for (unsigned int i = 0; i < references_.size(); i++) {
        if (&references_.at(i)->instruction() == &ins) {
            return true;
        }
    }
    return false;
}

/**
 * Return the number of instruction references.
 *
 * @return The number of instruction references.
 */
int
InstructionReferenceManager::referenceCount() const {
    return references_.size();
}

/**
 * Return the reference in the given index.
 *
 * @return The reference in the given index.
 * @exception OutOfRange If the index is out of range.
 */
InstructionReference&
InstructionReferenceManager::reference(int index) const throw (OutOfRange){
    if (index < 0 || (unsigned int)index >= references_.size()) {
        throw OutOfRange(__FILE__, __LINE__);
    }
    return *references_.at(index);
}

/**
 * Creates and returns an exact copy of the reference manager.
 *
 * @return An exact copy of the reference manager.
 */
InstructionReferenceManager*
InstructionReferenceManager::copy() const {
    InstructionReferenceManager* newManager;
    newManager = new InstructionReferenceManager();
    for (int i = 0; i < referenceCount(); i++) {
        newManager->createReference(reference(i).instruction());
    }
    return newManager;
}

} // namespace TTAProgram
