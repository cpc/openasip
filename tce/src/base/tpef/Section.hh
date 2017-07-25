/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file Section.hh
 *
 * Declaration of Section and RawSection classes.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_HH
#define TTA_SECTION_HH

#include <map>
#include <list>
#include <vector>

#include "TPEFBaseType.hh"
#include "ASpaceElement.hh"
#include "SafePointable.hh"
#include "Exception.hh"
#include "Chunk.hh"

namespace TPEF {
    namespace ReferenceManager {
        class SafePointer;
    }

    class SectionElement;

/////////////////////////////////////////////////////////////////////////////
// Section
/////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for concrete sections.
 *
 * Stores section header data and handles registration requests and
 * book keeping for concrete sections. See prototype design pattern.
 */
class Section : public SafePointable {
public:
    /**
     * TPEF section type ids.
     */
    enum SectionType {
        ST_NULL   = 0x00, ///< NULL Section
        ST_STRTAB = 0x01, ///< String table.
        ST_SYMTAB = 0x02, ///< Symbol table.
        ST_DEBUG  = 0x03, ///< Debug section.
        ST_RELOC  = 0x04, ///< Relocation section.
        ST_LINENO = 0x05, ///< Line number section.
        ST_NOTE   = 0x06, ///< Note section.
        ST_ADDRSP = 0x07, ///< Address space section.
        ST_MR     = 0x0A, ///< Machine resources section.
        ST_CODE   = 0x81, ///< Text section.
        ST_DATA   = 0x82, ///< Initialized data section.
        ST_UDATA  = 0x83, ///< Uninitialized data section.
        ST_LEDATA = 0x84, ///< Initialized little endian data section.
        ST_DUMMY  = 0xff  ///< Dummy section type for testing purposes.
    };

    /**
     * TPEF section flags.
     */
    enum SectionFlag {
        SF_VLEN   = 0x40, ///< Contains elements with variable length.
        SF_NOBITS = 0x80  ///< Not initialized or not stored in this file.
    };

    /// Returns SectioType of actual section instance.
    virtual SectionType type() const = 0;

    virtual ~Section();

    static Section* createSection(SectionType type)
        throw (InstanceNotFound);

    virtual bool isChunkable() const;

    virtual Chunk* chunk(SectionOffset offset) const
        throw (NotChunkable);

    bool isProgramSection() const;
    static bool isProgramSection(SectionType type);

    bool isAuxSection() const;

    virtual void addElement(SectionElement* element);

    virtual void setElement(Word index, SectionElement* element);

    SectionElement* element(Word index) const;
    Word elementCount() const;

    void setFlagNoBits();
    void unsetFlagNoBits();

    bool noBits() const;

    bool vLen() const;

    Byte flags() const;
    void setFlags(Byte flagByte);

    void setStartingAddress(AddressImage address);
    AddressImage startingAddress() const;

    void setLink(const ReferenceManager::SafePointer* aLink);
    void setLink(Section* aLink);
    Section* link() const;

    void setASpace(const ReferenceManager::SafePointer* addrSpace);
    void setASpace(ASpaceElement* addrSpace);
    ASpaceElement* aSpace() const;

    void setName(const ReferenceManager::SafePointer* sectionName);
    void setName(Chunk* sectionName);
    Chunk* name() const;

    virtual bool isDataSection() { return false; }
protected:
    /// Creates clone of instance.
    virtual Section* clone() const = 0;

    Section();

    static void registerSection(const Section* section);

    // this section type specific flag can't be changed by user
    void setFlagVLen();
    void unsetFlagVLen();

private:
    Section(const Section&);

    bool flag(SectionFlag aFlag) const;
    void setFlag(SectionFlag aFlag);
    void unsetFlag(SectionFlag aFlag);

    /// Type of map that contains section prototypes.
    typedef std::map<SectionType, const Section*> SectionPrototypeMap;

    /// Container for registere section prototypes.
    static SectionPrototypeMap* prototypes_;
    /// Contain elements.
    std::vector<SectionElement*> elements_;

    /// TPEF link field.
    const ReferenceManager::SafePointer* link_;
    /// TPEF address space field.
    const ReferenceManager::SafePointer* aSpace_;
    /// TPEF name field.
    const ReferenceManager::SafePointer* name_;
    /// TPEF flag byte.
    Byte flags_;
    /// TPEF startin memory address field.
    Word startingAddress_;
    /// Mask for checking if section is auxiliary or program section.
    static const Byte PROGRAM_SECTION_MASK;
};

/////////////////////////////////////////////////////////////////////////////
// RawSection
/////////////////////////////////////////////////////////////////////////////

/**
 * Abstract base class for sections that stores raw data.
 *
 * These sections are chunkable, that is they can create elements (chunks)
 * on demand. These chunks point (using offsets) to section's raw data.
 */
class RawSection : public Section {
public:
    virtual bool isChunkable() const;

    virtual Chunk* chunk(
        SectionOffset offset) const
        throw (NotChunkable);

    virtual void assureInSection(
        SectionOffset offset) const;

    virtual ~RawSection();

    bool empty() const;

    virtual void setLengthInMAUs(Word length);
    virtual Word lengthInMAUs() const;

    virtual void setDataLength(Word length);
    virtual Word length() const;

    virtual Word bytesToMAUs(Word byteCount) const;
    virtual Word MAUsToBytes(Word mauCount) const;

    virtual Word chunkToMAUIndex(const Chunk* chunk) const;

    Word referredChunkCount() const;
    Chunk* referredChunk(Word index) const;
    bool belongsToSection(const Chunk* chunk) const;
    
protected:
    RawSection();

private:
    RawSection(const RawSection&);

    /// The length of raw data section.
    Word length_;

    /// Type of map that contains chunks.
    typedef std::map<SectionOffset, Chunk*> ChunkMap;

    /// Container for already created chunks.
    mutable ChunkMap dataChunks_;
    
    /// Table containing all the chunks that are requested from section.
    mutable std::vector<Chunk*> referredChunksCache_;
};
}

#include "Section.icc"

#endif
