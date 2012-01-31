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
 * @file TPEFRelocSectionReader.hh
 *
 * Declaration of TPEFRelocSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RELOC_SECTION_READER_HH
#define TTA_TPEF_RELOC_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads relocation section from TPEF binary file.
 */
class TPEFRelocSectionReader : public TPEFSectionReader {
protected:
    TPEFRelocSectionReader();
    virtual ~TPEFRelocSectionReader();

    virtual void finalize(Section* section) const;

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

    void readInfo(BinaryStream& stream, Section* sect) const;

private:
    /// Copying not allowed.
    TPEFRelocSectionReader(const TPEFRelocSectionReader&);
    /// Assignment not allowed.
    TPEFRelocSectionReader& operator=(TPEFRelocSectionReader&);

    /// Prototype instance of TPEFRelocSectionReader to be registered to
    /// SectionReader.
    static TPEFRelocSectionReader proto_;
    
    /// Section id of last referenced section.
    static SectionId refSectionId_;
};
}

#endif
