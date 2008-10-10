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
