/**
 * @file SectionSymElement.cc
 *
 * Definition of SectionSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "SectionSymElement.hh"

namespace TPEF {

/**
 * Constructor.
 */
SectionSymElement::SectionSymElement() :
    SymbolElement(), value_(0), size_(0) {

}

/**
 * Destructor.
 */
SectionSymElement::~SectionSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
SectionSymElement::type() const {
    return STT_SECTION;
}

/**
 * Returns value of the element.
 *
 * This might be for example relocation information.
 *
 * @return Value of the element.
 */
Word
SectionSymElement::value() const {
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
SectionSymElement::setValue(Word aValue) {
    value_ = aValue;
}

/**
 * Returns size of the referred object in MAUs.
 *
 * @return Size of the referred object in MAUs.
 */
Word
SectionSymElement::size() const {
    return size_;
}

/**
 * Sets size of referred object in MAUs.
 *
 * @param aSize of the referred object in MAUs.
 */
void
SectionSymElement::setSize(Word aSize) {
    size_ = aSize;
}

}
