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
 * @file InstructionReferenceImpl.cc
 *
 * Implementation of InstructionReferenceImpl class.
 *
 * This class provides the internal tracking of instruction references to
 * a single instruction. This class contains information about
 * all references pointing to this instructions. When the reference count
 * reaches 0, this class is automatically deleted.
 *
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include <set>

#include <iostream>
#include "InstructionReference.hh"
#include "InstructionReferenceImpl.hh"
#include "InstructionReferenceManager.hh"

#include "Application.hh"


namespace TTAProgram {
class InstructionReference;


/**
 * Constructor. 
 *
 * Stores the instruction and reference manager.
 *
 * @param ins Instruction whose references this will handle
 * @param irm the InstructionReferenceManager owning this object.
 */
InstructionReferenceImpl::InstructionReferenceImpl(
    TTAProgram::Instruction& ins, InstructionReferenceManager& irm) :
    ins_(&ins), refMan_(&irm) {}


/** 
 * Destructor. 
 *
 * Just asserts no refs point into this.
 */
InstructionReferenceImpl::~InstructionReferenceImpl() {
    assert(refs_.empty());
}
    
/**
 * Nullifies all references pointing to this.
 *
 * This should only be called by instructionreferenec manager when removing
 * zombie refs.
 */
void 
InstructionReferenceImpl::nullify() {
    // the set is modified inside so cannot iterate normally.
    // get the first as long as there are some.
    while (!refs_.empty()) {
        // if setimpl return false, I am dead and cannot do
        // anything.
        if (!(*refs_.begin())->setImpl(NULL)) {
            return;
        }
    }
}

/** 
 * A new reference has been created for the instruction handled by
 * this object. Adds it to the list. 
 * 
 * @param InstructionReference new reference.
 */
void 
InstructionReferenceImpl::addRef(InstructionReference& ref) {
    refs_.insert(&ref);
}

/**
 * A reference no longer points to instruction handled by this object.
 *
 * Removes it from the list. If it was last, ask InstructionReferenceManager
 * to delete this object.
 * 
 * @param ref reference which no longer points here
 * @return true if impl stays alive, false if dies.(removed last ref)
 */
bool 
InstructionReferenceImpl::removeRef(InstructionReference& ref) {
    assert(refs_.find(&ref) != refs_.end());
    refs_.erase(&ref);
    if (refs_.empty()) {
        refMan_->referenceDied(ins_);
        return false;
    }
    return true;
}

/** 
 * Merges another irimpl to this object. 
 *
 * Makes all of it's references to point into this instead. This will also 
 * indirectly lead to the deletion of the other. This is used for irm.update() 
 * when the new target ins already has a ref.
 *
 * @param other InstructionRefererenceImpl which to merge into this.
 */
void 
InstructionReferenceImpl::merge(InstructionReferenceImpl& other) {
    // copy this in order to prevent it being deleted on last iteration
    std::set<InstructionReference*> otherRefs = other.refs_;
    for (std::set<InstructionReference*>::iterator iter = 
             otherRefs.begin(); iter != otherRefs.end(); iter++) {
        (*iter)->setImpl(this);
    }
}

/**
 * Sets this object to point into another instruction.
 * 
 * This method should be only called by InstructionReferenceManager.
 *
 * @param ins new insruction where to point.
 */
void 
InstructionReferenceImpl::setInstruction(Instruction& ins) {
    ins_ = &ins;
}
    
/**
 * Returns a reference pointing into instruction handled by this object.
 *
 * @return some InstructionReference pointing to this.
 */
const InstructionReference& 
InstructionReferenceImpl::ref() {
    return **refs_.begin();
}

}
