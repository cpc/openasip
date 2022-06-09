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
 * @file InstructionReferenceManager.cc
 *
 * Implementation of InstructionReferenceManager class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "InstructionReferenceManager.hh"
#include "InstructionReference.hh"
#include "InstructionReferenceImpl.hh"
#include "Application.hh"
#include "Instruction.hh"
#include "Procedure.hh"

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
//    assert(references_.empty());
    clearReferences();
}

/**
 * Creates a new reference to an instruction.
 *
 * @param ins Referred instruction.
 * @return A new reference to an instruction, or if one already exists,
 *         return it.
 */
InstructionReference
InstructionReferenceManager::createReference(Instruction& ins) {
    RefMap::const_iterator iter = references_.find(&ins);
    if (iter == references_.end()) {
        InstructionReferenceImpl* newRef = 
            new InstructionReferenceImpl(ins, *this);
        references_[&ins] = newRef;
        return InstructionReference(newRef);
    } else {
        return InstructionReference(iter->second);
    }
}

/**
 * Replaces a referred instruction with another.
 * This replaces ALL references that point into the same instruction.
 *
 * @param insA Instruction to be replaced.
 * @param insB The new referred instruction.
 * @return A handle to the reference to the new referred instruction.
 * @exception InstanceNotFound if the instruction to be replaced is not
 *            found.
 */
void
InstructionReferenceManager::replace(Instruction& insA, Instruction& insB) {
    RefMap::iterator itera = references_.find(&insA);
    if (itera == references_.end()) {
        throw InstanceNotFound(
            __FILE__, __LINE__, "InstructionReferenceManager::replace()",
            "Instruction reference to be replaced not found.");
    }

    RefMap::iterator iterb = references_.find(&insB);
    if (iterb == references_.end()) { // just update one.
        // no ref to b, just update a to point to b.
        InstructionReferenceImpl* impl = itera->second;
        impl->setInstruction(insB);
        references_.erase(itera);
        references_[&insB] = impl;
        return;
    }

    // merge the two ref implementations.
    iterb->second->merge(*itera->second);
}

/**
 * Clears all instruction references. 
 *
 * The result is a totally empty instruction reference manager. This 
 * nullifies all instructionreferences handled by this reference manager.
 */ 
void
InstructionReferenceManager::clearReferences() {
    // nullify modifies so take new iter every round.
    for (RefMap::iterator iter = references_.begin(); 
         iter != references_.end(); iter = references_.begin()) {
        assert(iter->second != NULL);
        // nullify causes use count to drop to 0 which kills this.
        iter->second->nullify();
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
 * Tells how many alive references there are to an instruction.
 */
unsigned int
InstructionReferenceManager::referenceCount(Instruction& ins) const {
    RefMap::const_iterator iter = references_.find(&ins);
    if (iter == references_.end()) {
        return 0;
    }
    return iter->second->count();
}

/**
 * Notifies instructionreferencemanager that a reference has completely died.
 *
 * This causes the reference manager to remove the reference impl object.
 */
void 
InstructionReferenceManager::referenceDied(Instruction* ins) {
    RefMap::iterator iter = references_.find(ins);
    assert (iter != references_.end());
    assert (iter->second->count() == 0);
    delete iter->second; iter->second = NULL;
    references_.erase(iter);
}

/**
 * Performs sanity checks to the instruction references.
 *
 * Asserts in case of illegal irefs found. Before calling this method,
 * all instruction references must have been "stabilized", i.e.,
 * pointing to valid instructions inside the Program.
 */
void
InstructionReferenceManager::validate()  {

    for (InstructionReferenceManager::Iterator i = begin();
         i != end(); ++i) {
        Instruction& targetInstruction = i->instruction();

        if (!targetInstruction.isInProcedure()) {
            Application::logStream()
                << "Reference to an instruction " << &targetInstruction
                << " that is not in a Procedure." << std::endl;
            PRINT_VAR(targetInstruction.address().location());
            abort();
        } else if (!targetInstruction.parent().isInProgram()) {
            Application::logStream()
                << "Reference to an instruction " << &targetInstruction
                << " that is not in a Program." << std::endl;
            PRINT_VAR(&(*i));
            PRINT_VAR(
                dynamic_cast<TTAProgram::Procedure&>(
                    targetInstruction.parent()).name());
            PRINT_VAR(
                &targetInstruction.parent().firstInstruction());
            PRINT_VAR(targetInstruction.address().location());
            abort();
        }
    }    
}

} // namespace TTAProgram
