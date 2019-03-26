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
 * @file TPEFUDataSectionWriter.hh
 *
 * Declaration of TPEFUDataSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_UDATA_SECTION_WRITER_HH
#define TTA_TPEF_UDATA_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "Section.hh"

namespace TPEF {

/**
 * Writes unitialized data section in to stream.
 *
 * Actually implements only needed fall through methods
 * for setting element size and section size to section header and
 * for creating section offset keys for referred data chunks.
 */
class TPEFUDataSectionWriter : public TPEFSectionWriter {
protected:
    TPEFUDataSectionWriter();
    virtual ~TPEFUDataSectionWriter();

    virtual Section::SectionType type() const;

    virtual void createKeys(const Section* sect) const;

    virtual Word elementSize(const Section* section) const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

private:
    /// Copying not allowed.
    TPEFUDataSectionWriter(const TPEFUDataSectionWriter&);
    /// Assignment not allowed.
    TPEFUDataSectionWriter operator=(const TPEFUDataSectionWriter&);

    /// Unique instance of a class.
    static const TPEFUDataSectionWriter instance_;
};
}

#endif
