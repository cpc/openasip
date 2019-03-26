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
