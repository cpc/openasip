/**
 * @file DebugSection.cc
 *
 * Definition of DebugSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "DebugSection.hh"

namespace TPEF {

DebugSection DebugSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if registeration is wanted.
 */
DebugSection::DebugSection(bool init) : Section() {
    if (init) {
        Section::registerSection(this);
    }
    setFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
DebugSection::~DebugSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
DebugSection::type() const {
    return ST_DEBUG;
}

/**
 * Creates an instance of class.
 *
 * @return Newly created section.
 */
Section*
DebugSection::clone() const {
    return new DebugSection(false);
}

} // namespace TPEF
