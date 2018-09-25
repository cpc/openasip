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
 * @file ImmediateElement.hh
 *
 * Declaration of ImmediateElement class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 21 October 2003 by ml, jn, rl, pj
 *
 * @note rating: yellow
 */

#ifndef TTA_IMMEDIATE_ELEMENT_HH
#define TTA_IMMEDIATE_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "InstructionElement.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Immediate data element of CodeSection.
 */
class ImmediateElement : public InstructionElement {
public:
    ImmediateElement();
    virtual ~ImmediateElement();

    bool isInline() const;

    void addByte(Byte aByte);

    Byte byte(unsigned int index) const
        throw (OutOfRange);

    void setByte(unsigned int index, Byte aValue)
        throw (OutOfRange);

    void setWord(Word aValue);
    void setSignedWord(SignedWord aValue);
    void setSignedLong(SLongWord aValue);
    void setULongWord(ULongWord aValue);

    Word word() const
        throw (OutOfRange);

    LongWord longWord() const;
    SignedLongWord sLongWord() const;

    SignedWord signedWord() const;

    unsigned int length() const;
    
    Byte destinationUnit() const;
    void setDestinationUnit(Byte aDestinationUnit);

    Byte destinationIndex() const;
    void setDestinationIndex(Byte aDestinationIndex);

private:
    /// Value of immediate.
    std::vector<Byte> value_;
    /// Destination unit of immediate.
    Byte destinationUnit_;
    /// Destination index of immediate.
    Byte destinationIndex_;
};
}

#include "ImmediateElement.icc"

#endif
