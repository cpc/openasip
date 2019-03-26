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
 * @file ASpaceSection.cc
 *
 * Definition of ASpaceSection class,
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "ASpaceSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

// registers section prototype to base class
ASpaceSection ASpaceSection::proto_(true);

/**
 * Constructor.
 *
 * @param init true if registeration is wanted
 */
ASpaceSection::ASpaceSection(bool init) :
    Section(), undefinedElement_(&SafePointer::null) {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    unsetFlagNoBits();
    setStartingAddress(0);
}

/**
 * Destructor
 */
ASpaceSection::~ASpaceSection() {
}

/**
 * Returns sections type.
 *
 * @return type of section.
 */
Section::SectionType
ASpaceSection::type() const {
    return ST_ADDRSP;
}

/**
 * Creates an instance of class.
 *
 * @return Newly created section.
 */
Section*
ASpaceSection::clone() const {
    return new ASpaceSection(false);
}

/**
 * Checks if the parameter, is undefined address space.
 *
 * @param aSpace Element to check.
 * @return True, if parameter is the undefined address space.
 */
bool
ASpaceSection::isUndefined(ASpaceElement *aSpace) const {
    assert(aSpace != NULL);
    assert(undefinedElement_ != &SafePointer::null);
    return (undefinedElement_->pointer() == aSpace);
}

/**
 * Set undefined address space.
 *
 * This method must be runned, before checking of undefined address
 * space is allowed. If set is runned many times, it overrides earlier
 * setings.
 *
 * @param aSpace Element to define as undefined address space.
 */
void
ASpaceSection::setUndefinedASpace(ASpaceElement* aSpace) {
    assert(aSpace != NULL);
    assert(aSpace->MAU() == 0);
    assert(aSpace->align() == 0);
    assert(aSpace->wordSize() == 0);
    undefinedElement_ = SafePointer::replaceReference(
        undefinedElement_, aSpace);
}

/**
 * Returns undefined address space element.
 *
 * @return Undefined address space element.
 */
ASpaceElement*
ASpaceSection::undefinedASpace() const {
    return dynamic_cast<ASpaceElement*>(
        undefinedElement_->pointer());
}

}
