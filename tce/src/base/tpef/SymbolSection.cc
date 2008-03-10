/**
 * @file SymbolSection.cc
 *
 * Definition of SymbolSection class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SymbolSection.hh"

namespace TPEF {

SymbolSection SymbolSection::proto_(true);

/**
 * Constructor.
 *
 * @param init True if registeration is wanted.
 */
SymbolSection::SymbolSection(bool init) : Section() {
    if (init) {
        Section::registerSection(this);
    }

    unsetFlagVLen();
    unsetFlagNoBits();
}

/**
 * Destructor.
 */
SymbolSection::~SymbolSection() {
}

/**
 * Returns section's type.
 *
 * @return Type of section.
 */
Section::SectionType
SymbolSection::type() const {
    return ST_SYMTAB;
}

/**
 * Creates an instance of class.
 *
 * @return Newly created section.
 */
Section*
SymbolSection::clone() const {
    return new SymbolSection(false);
}

}
