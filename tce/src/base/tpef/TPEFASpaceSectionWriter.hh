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
 * @file TPEFASpaceSectionWriter.hh
 *
 * Declaration of TPEFASpaceSectionWriter class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_ASPACE_SECTION_WRITER_HH
#define TTA_TPEF_ASPACE_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "BinaryStream.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Writes TPEF address space section to stream.
 */
class TPEFASpaceSectionWriter : public TPEFSectionWriter {
protected:
    TPEFASpaceSectionWriter();
    virtual ~TPEFASpaceSectionWriter();

    virtual Section::SectionType type() const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFASpaceSectionWriter(const TPEFASpaceSectionWriter&);
    /// Assignment not allowed.
    TPEFASpaceSectionWriter operator=(const TPEFASpaceSectionWriter&);

    void writeElement(
        BinaryStream &stream,
        const ASpaceElement* elem,
        SectionId id,
        Word index) const;

    /// An unique instance of class.
    static const TPEFASpaceSectionWriter instance_;
    /// The fixed size address space element.
    static const Word elementSize_;
};
}

#endif
