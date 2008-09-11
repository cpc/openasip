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
 * @file TPEFSectionReader.hh
 *
 * Declaration of TPEFSectionReader class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SECTION_READER_HH
#define TTA_TPEF_SECTION_READER_HH

#include "SectionReader.hh"
#include "TPEFReader.hh"

namespace TPEF {

/**
 * Abstract base class for all TPEFSectionReaders.
 *
 * Class implements default definitions of virtual methods declared in
 * SectionReader class and reading headers of TPEF sections.
 */
class TPEFSectionReader : public SectionReader {
public:
    virtual ~TPEFSectionReader();

protected:
    TPEFSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual BinaryReader* parent() const;

    virtual void readInfo(BinaryStream& stream, Section *sect) const;

    /**
     * Stores data of TPEF section header.
     */
    struct Header {
        SectionId sectionId;
        SectionId linkId;
        Word sectionSize;
        Word elementSize;
        Word bodyOffset;
        Word bodyLength;
    };

    static const Header& header();

private:
    TPEFSectionReader(const TPEFSectionReader&);

    /// Stores values that are needed in actual SectionReader classes.
    static Header header_;
};
}

#endif
