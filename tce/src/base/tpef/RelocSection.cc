/**
 * @file RelocSection.cc
 *
 * Non-inline definitions of RelocSection class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#include "RelocSection.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

RelocSection RelocSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if registeration is wanted.
 */
RelocSection::RelocSection(bool init) :
    Section(), refSection_(&SafePointer::null) {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    unsetFlagNoBits();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
RelocSection::~RelocSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
RelocSection::type() const {
    return ST_RELOC;
}

/**
 * Creates an instance of class.
 *
 * @return Newly created section.
 */
Section*
RelocSection::clone() const {
    return new RelocSection(false);
}

}
