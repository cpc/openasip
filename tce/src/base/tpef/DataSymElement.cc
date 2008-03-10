/**
 * @file DataSymElement.cc
 *
 * Definition of DataSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "DataSymElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
DataSymElement::DataSymElement() :
    SymbolElement(),
    reference_(&SafePointer::null),
    size_(0) {
}

/**
 * Destructor.
 */
DataSymElement::~DataSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
DataSymElement::type() const {
    return STT_DATA;
}

/**
 * Returns object which element refers.
 *
 * @return object which element refers.
 */
Chunk*
DataSymElement::reference() const {
    return dynamic_cast<Chunk*>(reference_->pointer());
}

/**
 * Sets object which element refers.
 *
 * @param aReference Object which element refers.
 */
void
DataSymElement::setReference(Chunk* aReference) {
    reference_ = SafePointer::replaceReference(reference_, aReference);
}

/**
 * Sets object which element refers.
 *
 * @param aReference Object which element refers.
 */
void
DataSymElement::setReference(
    const ReferenceManager::SafePointer* aReference) {
    reference_ = aReference;
}

/**
 * Returns size of the referred object in MAUs.
 *
 * @return Size of the referred object in MAUs.
 */
Word
DataSymElement::size() const {
    return size_;
}

/**
 * Sets size of referred object in MAUs.
 *
 * @param aSize of the referred object in MAUs.
 */
void
DataSymElement::setSize(Word aSize) {
    size_ = aSize;
}

}
