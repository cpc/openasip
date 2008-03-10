/**
 * @file UDataSection.cc
 *
 * Non-inline definitions of UDataSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#include "UDataSection.hh"

namespace TPEF {

UDataSection UDataSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if instance should register itself to base class.
 */
UDataSection::UDataSection(bool init) : RawSection() {

    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    setFlagNoBits();
    setStartingAddress(0);
}

/**
 * Destructor.
 */
UDataSection::~UDataSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
UDataSection::type() const {
    return ST_UDATA;
}

/**
 * Creates an instance of section.
 *
 * @return Newly created section.
 */
Section*
UDataSection::clone() const {
    return new UDataSection(false);
}

/**
 * Returns byte from section.
 *
 * In this case section contains only zeros.
 *
 * @param chunk The chunk in which offset byte is wanted.
 * @return Byte from the offset of the chunk.
 */
Byte
UDataSection::byte(const Chunk* /*chunk*/) const {
    return 0x00;
}

}
