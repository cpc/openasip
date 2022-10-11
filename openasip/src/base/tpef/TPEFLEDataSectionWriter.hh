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
 * @file TPEFLEDataSectionWriter.hh
 *
 * Declaration of TPEFLEDataSectionWriter class.
 *
 * @author Heikki Kultala 2014(hkultala-no-spam-cs.tut.fi)
 */

#ifndef TTA_TPEF_LEDATA_SECTION_WRITER_HH
#define TTA_TPEF_LEDATA_SECTION_WRITER_HH

#include "TPEFDataSectionWriter.hh"

namespace TPEF {

/**
 * Writes data section to TPEF stream.
 */
class TPEFLEDataSectionWriter : public TPEFDataSectionWriter {
protected:
    virtual ~TPEFLEDataSectionWriter();
    TPEFLEDataSectionWriter();

    virtual Section::SectionType type() const;
private:
    /// No copies.
    TPEFLEDataSectionWriter(const TPEFLEDataSectionWriter&);
    /// No copies.
    TPEFLEDataSectionWriter operator=(const TPEFLEDataSectionWriter&);

    /// Only existing instance of class.
    static const TPEFLEDataSectionWriter instance_;
};
}

#endif
