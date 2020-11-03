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
 * @file TPEFCodeSectionWriter.hh
 *
 * Declaration of TPEFCodeSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 18.12.2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_CODE_SECTION_WRITER_HH
#define TTA_TPEF_CODE_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "TPEFHeaders.hh"

namespace TPEF {

/**
 * Writes TPEF code section to file.
 */
class TPEFCodeSectionWriter : public TPEFSectionWriter {
protected:
    TPEFCodeSectionWriter();
    virtual ~TPEFCodeSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    virtual void writeInfo(BinaryStream& stream, const Section* sect) const;
    virtual void writeId(BinaryStream& stream, HalfWord id) const;

private:
    /// Copying not allowed.
    TPEFCodeSectionWriter(const TPEFCodeSectionWriter&);
    /// Assignment not allowed.
    TPEFCodeSectionWriter& operator=(const TPEFCodeSectionWriter&);

    void writeAttributeField(
        BinaryStream& stream,
        SectionElement* elem,
        bool isEnd) const;

    void writeDataField(BinaryStream& stream, SectionElement* elem) const;

    void writeAnnotations(BinaryStream& stream, SectionElement* elem) const;

    /// Unique instance of class.
    static const TPEFCodeSectionWriter instance_;
    /// Maximum number of bytes that single immediate can contain.
    static const Byte IMMEDIATE_VALUE_MAX_BYTES;
};
}

#endif
