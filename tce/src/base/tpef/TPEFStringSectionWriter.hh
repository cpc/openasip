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
 * @file TPEFStringSectionWriter.hh
 *
 * Declaration of TPEFStringSectionWriter class.
 *
 * @author Jussi Nyk�nen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepist� 18.12.2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_STRING_SECTION_WRITER_HH
#define TTA_TPEF_STRING_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes the contents of TPEF string section to stream.
 */
class TPEFStringSectionWriter : public TPEFSectionWriter {
protected:
    TPEFStringSectionWriter();
    virtual ~TPEFStringSectionWriter();

    virtual Section::SectionType type() const;
    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* sect) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFStringSectionWriter(const TPEFStringSectionWriter&);
    /// Assignment not allowed.
    TPEFStringSectionWriter& operator=(const TPEFStringSectionWriter&);

    /// Unique instance of class.
    static const TPEFStringSectionWriter instance_;
};
}

#endif
