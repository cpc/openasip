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
 * @file TPEFResourceSectionReader.hh
 *
 * Declaration of TPEFResourceSectionReader.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_RESOURCE_SECTION_READER_HH
#define TTA_TPEF_RESOURCE_SECTION_READER_HH

#include "TPEFSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads Processor Resource Table section from TPEF binary file.
 */
class TPEFResourceSectionReader : public TPEFSectionReader {
protected:
    TPEFResourceSectionReader();
    virtual ~TPEFResourceSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    TPEFResourceSectionReader(const TPEFResourceSectionReader&);
    /// Assignment not allowed.
    TPEFResourceSectionReader& operator=(TPEFResourceSectionReader&);

    /// Prototype instance to be registered.
    static TPEFResourceSectionReader proto_;
};
}

#endif
