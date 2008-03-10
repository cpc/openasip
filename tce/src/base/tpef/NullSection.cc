/**
 * @file NullSection.cc
 *
 * Non-inline definitions of NullSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "NullSection.hh"

namespace TPEF {

NullSection NullSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if instance should register itself to base class.
 */
NullSection::NullSection(bool init) : Section() {
    if (init) {
        Section::registerSection(this);
    }
    // null section does not contain data.
    setFlagNoBits();
    unsetFlagVLen();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
NullSection::~NullSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
NullSection::type() const {
    return ST_NULL;
}

/**
 * Creates an instance of section.
 *
 * @return Newly created section.
 */
Section*
NullSection::clone() const {
    return new NullSection(false);
}

}
