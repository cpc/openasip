/**
 * @file DebugElement.cc
 *
 * Non-inline definitions of DebugElement class.
 *
 * @author Mikael Lepistö 2006 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#include "DebugElement.hh"

namespace TPEF {

/**
 * Constructor.
 */
DebugElement::DebugElement() : SectionElement() {
}

/**
 * Destructor.
 */
DebugElement::~DebugElement() {
}

/**
 * Returns the debug string.
 *
 * @return Debug string.
 */
Chunk*
DebugElement::debugString() const {
    return dynamic_cast<Chunk*>(debugString_->pointer());
}

/**
 * Sets the debug string.
 *
 * @param aString Debug string to set.
 */
void
DebugElement::setDebugString(const ReferenceManager::SafePointer* aString) {
    debugString_ = aString;
}

/**
 * Sets the debug string.
 *
 * @param aString The debug string.
 */
void
DebugElement::setDebugString(Chunk* aString) {
    using namespace ReferenceManager;
    debugString_ = SafePointer::replaceReference(debugString_, aString);
}

} // namespace TPEF
