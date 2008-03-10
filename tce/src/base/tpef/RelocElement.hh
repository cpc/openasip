/**
 * @file RelocElement.hh
 *
 * Declaration of RelocElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist@cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_RELOC_ELEMENT_HH
#define TTA_RELOC_ELEMENT_HH

#include "SectionElement.hh"
#include "SafePointer.hh"
#include "SymbolElement.hh"
#include "ASpaceElement.hh"

namespace TPEF {

/**
 * Relocation table entry.
 */
class RelocElement : public SectionElement {
public:
    /**
     * TPEF relocation types.
     */
    enum RelocType {
        RT_NOREL     = 0x00,   ///< No relocation
        RT_SELF      = 0x01,   ///< Absolute address, relocate relative to
                               ///< address self.
        RT_PAGE      = 0x02,   ///< Paged address, relocate page offset.
        RT_PCREL     = 0x03    ///< PC-relative, relocate only if
                               ///< displacement changes.
    };

    RelocElement();
    virtual ~RelocElement();

    RelocType type() const;
    void setType(RelocType aType);

    SectionElement* location() const;
    void setLocation(SectionElement* aLocation);
    void setLocation(const ReferenceManager::SafePointer* aLocation);

    SectionElement* destination() const;
    void setDestination(SectionElement* aDestination);
    void setDestination(const ReferenceManager::SafePointer* aDestination);

    Byte size() const;
    void setSize(Byte aSize);

    Byte bitOffset() const;
    void setBitOffset(Byte anOffset);

    SymbolElement* symbol() const;
    void setSymbol(SymbolElement* aSymbol);
    void setSymbol(const ReferenceManager::SafePointer* aSymbol);

    ASpaceElement* aSpace() const;
    void setASpace(ASpaceElement* anASpace);
    void setASpace(const ReferenceManager::SafePointer* anASpace);

    bool chunked() const;
    void setChunked(bool isChunked);

private:
    /// The type of the relocation.
    RelocType type_;
    /// The element containing location to be relocated.
    const ReferenceManager::SafePointer* location_;
    /// The destination element of the location to be relocated.
    const ReferenceManager::SafePointer* destination_;
    /// Bit width of value(or chunk of value) to be relocated.
    Byte size_;
    /// Bit offset where in whole value bits of this chunk should be.
    Byte bitOffset_;
    /// The symbol of relocation.
    const ReferenceManager::SafePointer* symbol_;
    /// The address space of relocated element.
    const ReferenceManager::SafePointer* aSpace_;
    /// Does relocation refer to chunked address.
    bool chunked_;
};
}

#include "RelocElement.icc"

#endif
