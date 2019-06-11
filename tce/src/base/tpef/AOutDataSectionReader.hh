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
 * @file AOutDataSectionReader.hh
 *
 * Declaration of AOutDataSectionReader class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 7 October 2003 by jn, ml, tr, ll
 *
 * @note rating: yellow
 */

#ifndef TTA_AOUT_DATA_SECTION_READER_HH
#define TTA_AOUT_DATA_SECTION_READER_HH

#include "AOutSectionReader.hh"
#include "Section.hh"
#include "BinaryReader.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Reads data section from a.out binary file.
 */
class AOutDataSectionReader : public AOutSectionReader {
public:
    virtual ~AOutDataSectionReader();

protected:
    AOutDataSectionReader();

    virtual void readData(
        BinaryStream& stream,
        Section* section) const
        throw (UnreachableStream, KeyAlreadyExists, EndOfFile,
               OutOfRange, WrongSubclass, UnexpectedValue);

    virtual Section::SectionType type() const;

private:
    /// Copying not allowed.
    AOutDataSectionReader(const AOutDataSectionReader&);

    /// Prototype instance of AOutDataSectionReader to be registered to
    /// SectionReader.
    static AOutDataSectionReader proto_;
};
}

#endif
