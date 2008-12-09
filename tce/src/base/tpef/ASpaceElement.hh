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
 * @file ASpaceElement.hh
 *
 * Declaration of ASpaceElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_ASPACEELEMENT_HH
#define TTA_ASPACEELEMENT_HH

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "Chunk.hh"

namespace TPEF {

/**
 * Address space element.
 * Holds information of single address space.
 */
class ASpaceElement : public SectionElement {
public:
    ASpaceElement();
    virtual ~ASpaceElement();

    Byte MAU() const;
    void setMAU(Byte aMAU);

    Byte align() const;
    void setAlign(Byte aAlign);

    Byte wordSize() const;
    void setWordSize(Byte aWordSize);

    void setName(const ReferenceManager::SafePointer* aName);
    void setName(Chunk* aName);
    Chunk* name() const;

private:
    /// Minimum addressable unit.
    Byte mau_;
    /// Aligment.
    Byte align_;
    /// Word size.
    Byte wSize_;
    /// Name.
    const ReferenceManager::SafePointer* name_;
};
}

#include "ASpaceElement.icc"

#endif
