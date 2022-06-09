/*
    Copyright (c) 2002-2009 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file RelocElement.hh
 *
 * Declaration of RelocElement class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_RELOC_ELEMENT_HH
#define TTA_RELOC_ELEMENT_HH

#include "SectionElement.hh"
#include "SymbolElement.hh"
#include "ASpaceElement.hh"

namespace TPEF {
    namespace ReferenceManager {
        class SafePointer;
    }

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
