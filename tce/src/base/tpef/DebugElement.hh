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
 * @file DebugElement.hh
 *
 * Declaration of DebugElement class.
 *
 * @author Mikael Lepistö 2006 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DEBUG_ELEMENT_HH
#define TTA_DEBUG_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "SectionElement.hh"
#include "Exception.hh"
#include "Chunk.hh"

namespace TPEF {

/**
 * Base class for all kinds of TPEF elements that belong to debug sections.
 *
 * Allows representing data of any debug element. All types of TPEF debug
 * elements share a common property: a string that defines (part of) the
 * information stored in the element.
 */
class DebugElement : public SectionElement {
public:
    enum ElementType {
        DE_STAB = 0x1
    };

    DebugElement();
    virtual ~DebugElement();

    virtual ElementType type() const = 0;

    Chunk* debugString() const;
    void setDebugString(const ReferenceManager::SafePointer* aString);
    void setDebugString(Chunk* aString);

    /// Returns one byte of additional data.
    virtual Byte byte(Word index) const throw (OutOfRange) = 0;
    /// Returns the length of additional data of debug element.
    virtual Word length() const = 0;

protected:
    /// String of debug element.
    const ReferenceManager::SafePointer* debugString_;
};

}

#endif
