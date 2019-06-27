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
 * @file LineNumElement.hh
 *
 * Declaration of LineNumElement class.
 *
 * @author Mikael Lepist� 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_LINE_NUM_ELEMENT_HH
#define TTA_LINE_NUM_ELEMENT_HH

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "SafePointer.hh"
#include "InstructionElement.hh"
#include "SymbolElement.hh"

namespace TPEF {

/**
 * Line number element.
 */
class LineNumElement : public SectionElement {
public:
    LineNumElement();
    virtual ~LineNumElement();

    void setLineNumber(HalfWord lineNum);
    HalfWord lineNumber() const;

    void setInstruction(ReferenceManager::SafePointer* aRef);
    void setInstruction(InstructionElement* aRef);
    InstructionElement* instruction() const;

private:
    /// Line number which line in the procedure this element refer.
    HalfWord lineNumber_;
    /// Instruction element of source line.
    const ReferenceManager::SafePointer* instruction_;

};
}

#include "LineNumElement.icc"

#endif
