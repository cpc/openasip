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
 * @file TPEFSectionWriter.hh
 *
 * Declaration of TPEFSectionWriter class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SECTION_WRITER_HH
#define TTA_TPEF_SECTION_WRITER_HH

#include "SectionWriter.hh"

#include "Section.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Base class for all TPEF filetype writer classes.
 */
class TPEFSectionWriter : public SectionWriter {
protected:
    TPEFSectionWriter();
    virtual ~TPEFSectionWriter();

    virtual const BinaryWriter& parent() const;

    virtual void actualWriteHeader(
        BinaryStream& stream,
        const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    /// Returns SectionTypet hat actual reader or writer instance can handle.
    virtual Section::SectionType type() const = 0;

    virtual void createKeys(const Section* sect) const;

    virtual void writeSize(BinaryStream& stream, const Section* sect) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;

    virtual SectionId getSectionId() const;

    virtual Word elementSize(const Section* section) const;

private:
    TPEFSectionWriter(const TPEFSectionWriter&);

    void writeBodyStartOffset(
        BinaryStream& stream,
        const Section* sect) const;
};
}

#endif
