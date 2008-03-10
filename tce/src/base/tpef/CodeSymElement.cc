/**
 * @file CodeSymElement.cc
 *
 * Definition of CodeSymElement class.
 *
 * @author Mikael Lepistö 2004 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "CodeSymElement.hh"

namespace TPEF {

using ReferenceManager::SafePointer;

/**
 * Constructor.
 */
CodeSymElement::CodeSymElement() :
    SymbolElement(), reference_(&SafePointer::null),
    size_(0) {
}

/**
 * Destructor.
 */
CodeSymElement::~CodeSymElement() {
}

/**
 * Returns type of the element.
 *
 * @return Type of the element.
 */
SymbolElement::SymbolType
CodeSymElement::type() const {
    return STT_CODE;
}

/**
 * Returns object which element refers.
 *
 * @return object which element refers.
 */
InstructionElement*
CodeSymElement::reference() const {
    return dynamic_cast<InstructionElement*>(reference_->pointer());
}

/**
 * Sets object which element refers.
 *
 * @param aReference Object which element refers.
 */
void
CodeSymElement::setReference(InstructionElement* aReference) {
    reference_ = SafePointer::replaceReference(reference_, aReference);
}

/**
 * Sets object which element refers.
 *
 * @param aReference Object which element refers.
 */
void
CodeSymElement::setReference(
    const ReferenceManager::SafePointer* aReference) {
    reference_ = aReference;
}

/**
 * Returns size of the referred object in MAUs.
 *
 * @return Size of the referred object in MAUs.
 */
Word
CodeSymElement::size() const {
    return size_;
}

/**
 * Sets size of referred object in MAUs.
 *
 * @param aSize of the referred object in MAUs.
 */
void
CodeSymElement::setSize(Word aSize) {
    size_ = aSize;
}

}
