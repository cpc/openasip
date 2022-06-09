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
 * @file RelocSection.hh
 *
 * Declaration of RelocSection class.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 22 October 2003 by ml, jn, ao, tr
 *
 * @note rating: yellow
 */

#ifndef TTA_RELOC_SECTION_HH
#define TTA_RELOC_SECTION_HH

#include "Section.hh"
#include "SafePointer.hh"

namespace TPEF {

/**
 * Relocation table section.
 */
class RelocSection : public Section {

public:
    virtual ~RelocSection();

    virtual SectionType type() const;

    Section* referencedSection() const;
    void setReferencedSection(Section* section);
    void setReferencedSection(const ReferenceManager::SafePointer* section);

protected:
    RelocSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype instance of section.
    static RelocSection proto_;
    /// Section whose elements are being relocated by
    /// relocations of this section.
    const ReferenceManager::SafePointer* refSection_;
};
}

#include "RelocSection.icc"

#endif
