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
 * @file TPEFReader.hh
 *
 * Declaration of TPEFReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_READER_HH
#define TTA_TPEF_READER_HH

#include <map>

#include "BinaryReader.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {
    class BinaryStream;
    class Binary;
    class ASpaceElement;
    class ASpaceSection;
    class Section;
/**
 * Reads binary file that is in TPEF binary format.
 */
class TPEFReader : public BinaryReader {
public:
    Section* sectionOfAddress(
        const ASpaceElement* aSpaceId,
        AddressImage address) const;

    static BinaryReader* instance();

    SectionId aSpaceId();
    SectionId strTableId();

    void addSectionSize(const Section* sect, Word length);

    ASpaceSection* aSpaceSection() const;

protected:
    TPEFReader(const TPEFReader&);
    TPEFReader& operator=(const TPEFReader&);

    TPEFReader();
    virtual ~TPEFReader();

    virtual Binary* readData(BinaryStream& stream) const;

    virtual bool isMyStreamType(BinaryStream& stream) const;

private:
    Section* readSection(
        BinaryStream& stream,
        FileOffset headerOffset,
        Byte stringTableId,
        Byte aSpaceSectionId) const;


    /// Prototype instance registered into BinaryReader.
    static TPEFReader* proto_;

    /// Last or currently read binary.
    mutable Binary *binary_;
    /// Identification code of address space section (there is only
    /// one at maximum).
    mutable SectionId aSpaceId_;
    /// Identification code of string table containing section name strings.
    mutable SectionId strTableId_;
    /// Cache of sizes in addresses of read sections.
    mutable std::map<const Section*, Word> sectionSizes_;
};
}

#endif
