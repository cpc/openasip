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
 * @file TPEFSymbolSectionWriter.hh
 *
 * Declaration of TPEFSymbolSectionWriter class.
 *
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_TPEF_SYMBOL_SECTION_WRITER_HH
#define TTA_TPEF_SYMBOL_SECTION_WRITER_HH

#include "TPEFSectionWriter.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "SymbolElement.hh"

namespace TPEF {

/**
 * Writes TPEF symbol table to stream.
 */
class TPEFSymbolSectionWriter : public TPEFSectionWriter {
protected:
    TPEFSymbolSectionWriter();
    virtual ~TPEFSymbolSectionWriter();

    virtual Section::SectionType type() const;

    virtual void createKeys(const Section* sect) const;

    virtual void actualWriteData(
        BinaryStream& stream,
        const Section* section) const;

    virtual Word elementSize(const Section* section) const;

private:
    /// Copying not allowed.
    TPEFSymbolSectionWriter(const TPEFSymbolSectionWriter&);
    /// Assignment not allowed.
    TPEFSymbolSectionWriter operator=(const TPEFSymbolSectionWriter&);

    void writeValueAndSize(
        BinaryStream &stream,
        const SymbolElement* elem) const;

    /// An unique instance of class.
    static const TPEFSymbolSectionWriter instance_;
    /// The fixed size address space element.
    static const Word elementSize_;
};
}

#endif
