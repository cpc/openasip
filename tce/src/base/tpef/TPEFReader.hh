/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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

#include "BinaryReader.hh"
#include "Binary.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"
#include "Section.hh"
#include "BinaryStream.hh"
#include "TPEFHeaders.hh"
#include "ASpaceSection.hh"

namespace TPEF {

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

    virtual Binary* readData(BinaryStream& stream) const
        throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists,
               EndOfFile, OutOfRange, WrongSubclass, UnexpectedValue);

    virtual bool isMyStreamType(BinaryStream& stream) const
        throw (UnreachableStream);

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
