/**
 * @file ASpaceSection.cc
 *
 * Definition of ASpaceSection class,
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
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
