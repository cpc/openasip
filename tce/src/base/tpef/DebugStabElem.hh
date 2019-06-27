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
 * @file DebugStabElem.hh
 *
 * Declaration of DebugStabElem class.
 *
 * @author Mikael Lepist� 2006 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_DEBUG_STAB_ELEMENT_HH
#define TTA_DEBUG_STAB_ELEMENT_HH

#include <vector>

#include "TPEFBaseType.hh"
#include "Exception.hh"
#include "DebugElement.hh"

namespace TPEF {

/**
 * Stab debug element type.
 */
class DebugStabElem : public DebugElement {
public:
    DebugStabElem(std::vector<Byte>& data);

    DebugStabElem(
        Byte stabType,
        Byte other,
        HalfWord description,
        Word value);

    virtual ~DebugStabElem();

    virtual ElementType type() const;

    virtual Byte byte(Word index) const;
    virtual Word length() const;

    Byte stabType() const;
    Byte other() const;
    HalfWord description() const;
    Word value() const;

private:
    /// a.out stab type.
    Byte stabType_;
    /// a.out stab other field.
    Byte other_;
    /// a.out stab description field.
    HalfWord description_;
    /// a.out stab value field.
    Word value_;
};

}

#endif
