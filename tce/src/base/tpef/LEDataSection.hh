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
 * @file DataSection.hh
 *
 * Declaration of DataSection class.
 *
 * @author Mikael Lepist� 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_LEDATA_SECTION_HH
#define TTA_LEDATA_SECTION_HH

#include <vector>

#include "DataSection.hh"
#include "Section.hh"
#include "TPEFBaseType.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Data section for initialized data in Little-Endian ADFs
 *
 * Derives from big-endian data section so, this is extension
 * of DataSection. 
 */
class LEDataSection : public DataSection {
public:
    virtual ~LEDataSection();

    virtual SectionType type() const;

    virtual void writeValue(Word index, Word numOfMAUs, unsigned long value);
    virtual void writeValue(Word index, Word numOfMAUs, signed long value);

protected:
    LEDataSection(bool init);
    virtual Section* clone() const;
private:
    /// Prototype instance of section.
    static LEDataSection proto_;
};
}

#endif
