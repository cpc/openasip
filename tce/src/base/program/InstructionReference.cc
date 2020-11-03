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
 * @file InstructionReference.cc
 *
 * Implementation of InstructionReference class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

#include "InstructionReferenceImpl.hh"
#include "InstructionReference.hh"
#include "NullInstruction.hh"

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// InstructionReference
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @note Instruction references should be created using
 * InstructionReferenceManager.
 * @param ins Referred instruction.
 */
InstructionReference::InstructionReference(InstructionReferenceImpl* impl):
    impl_(impl) {
    if (impl_ != NULL) {
        impl_->addRef(*this);
    }
}

/**
 * Constructor.
 *
 * @note Instruction references should be created using
 * InstructionReferenceManager.
 * @param ins Referred instruction.
 */
InstructionReference::InstructionReference(const InstructionReference& ref):
    impl_(ref.impl_) {
    if (impl_ != NULL) {
        impl_->addRef(*this);
    }
}

/**
 * Assignment operator. 
 * 
 * Changes this reference to point to another instruction. Only changes
 * this reference, not other references pointing to same instruction.
 */
InstructionReference& 
InstructionReference::operator=(
    const InstructionReference& ref) {
    // if both point to same instruction, no need to do anything.
    if (ref.impl_ != impl_) {
        // stop pointing to old instruction
        if (impl_ != NULL) {
            impl_->removeRef(*this);
        }
        // pointing to new instruction
        impl_ = ref.impl_;
        if (impl_ != NULL) {
            impl_->addRef(*this);
        } 
    }
    return *this;
}


/**
 * Destructor. 
 *
 * Tells the impl that we are no longer pointing to it.
 * It may get also deleted if this was the last reference to it.
 */
InstructionReference::~InstructionReference() {
    if (impl_ != NULL) {
        impl_->removeRef(*this);
    }
}

/**
 * Sets a new referred instruction.
 *
 * The InstructionReferenceImpl is a "proxy object" that counts and 
 * keeps book of references
 *
 * @param newImpl New referred instruction.
 * @return true if old impl stays alive, false if it is deleted.
 */
bool
InstructionReference::setImpl(InstructionReferenceImpl* newImpl) {
    bool staysAlive = true;
    assert(newImpl != impl_);
    if (impl_ != NULL) {
        staysAlive = impl_->removeRef(*this);
    }
    impl_ = newImpl;
    if (impl_ != NULL) {
        impl_->addRef(*this);
    }
    return staysAlive;
}

/**
 * Returns the referred instruction.
 *
 * @return Referred instruction.
 */
Instruction&
InstructionReference::instruction() const {
    if (impl_ == NULL) {
        return NullInstruction::instance();
    } else {
        return impl_->instruction();
    }
}

bool
InstructionReference::operator ==(const InstructionReference& other) const {
    return impl_ == other.impl_;
}

}
