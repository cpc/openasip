/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
    RefMap::const_iterator iter = references_.find(&ins);
    if (iter == references_.end()) {
        InstructionReference* newRef = new InstructionReference(ins);
        references_.insert(
            std::pair<Instruction*,InstructionReference*>(&ins, newRef));
        return *newRef;
    } else {
        return *iter->second;
    }
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

    InstructionReference* ir = NULL;
    RefMap::iterator iter = references_.find(&insA);
    while (iter != references_.end()) {
        ir = iter->second;
        ir->setInstruction(insB);
        references_.erase(iter);
        references_.insert(
            std::pair<Instruction*,InstructionReference*>(&insB, ir));
        iter = references_.find(&insA);
    }
    if (ir != NULL) {
        return *ir;
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
    for (RefMap::iterator iter = references_.begin(); 
         iter != references_.end(); iter++) {
        delete iter->second;
        iter->second = NULL;
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
    return references_.find(&ins) != references_.end();
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
    for (RefMap::const_iterator iter = references_.begin(); 
         iter != references_.end(); iter++) {
        newManager->createReference(*iter->first);
    }
    return newManager;
}
    

} // namespace TTAProgram
