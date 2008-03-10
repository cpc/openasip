/**
 * @file DebugElement.hh
 *
 * Declaration of DebugElement class.
 *
 * @author Mikael Lepistö 2006 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DEBUG_ELEMENT_HH
#define TTA_DEBUG_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "Exception.hh"
#include "Chunk.hh"

namespace TPEF {

/**
 * Base class for all kinds of TPEF elements that belong to debug sections.
 *
 * Allows representing data of any debug element. All types of TPEF debug
 * elements share a common property: a string that defines (part of) the
 * information stored in the element.
 */
class DebugElement : public SectionElement {
public:
    enum ElementType {
        DE_STAB = 0x1
    };

    DebugElement();
    virtual ~DebugElement();

    virtual ElementType type() const = 0;

    Chunk* debugString() const;
    void setDebugString(const ReferenceManager::SafePointer* aString);
    void setDebugString(Chunk* aString);

    /// Returns one byte of additional data.
    virtual Byte byte(Word index) const throw (OutOfRange) = 0;
    /// Returns the length of additional data of debug element.
    virtual Word length() const = 0;

protected:
    /// String of debug element.
    const ReferenceManager::SafePointer* debugString_;
};

}

#endif
