/**
 * @file FileSymElement.cc
 *
 * Definition of FileSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "FileSymElement.hh"

namespace TPEF {

/**
 * Constructor.
 */
FileSymElement::FileSymElement() :
    SymbolElement(),
    value_(0) {
}

/**
 * Destructor.
 */
FileSymElement::~FileSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
FileSymElement::type() const {
    return STT_FILE;
}

/**
 * Returns value of the element.
 *
 * This might be for example relocation information.
 *
 * @return Value of the element.
 */
Word
FileSymElement::value() const {
    return value_;
}

/**
 * Sets value of the element.
 *
 * This might be for example relocation information.
 *
 * @param aValue Value of the element.
 */
void
FileSymElement::setValue(Word aValue) {
    value_ = aValue;
}

}
