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
 * @file Encoding.hh
 *
 * Declaration of Encoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_ENCODING_HH
#define TTA_ENCODING_HH

#include <string>
#include "Exception.hh"

class InstructionField;
class ObjectState;

/**
 * Represents an encoding of a source of destination within a move slot.
 * This is a base class for different encodings.
 */
class Encoding {
public:
    virtual ~Encoding();

    InstructionField* parent() const;
    
    unsigned int encoding() const;
    unsigned int extraBits() const;
    virtual int width() const;

    /**
     * Returns the position of the encoding within the parent field.
     */
    virtual int bitPosition() const = 0;

    virtual ObjectState* saveState() const;

    /// ObjectState name for Encoding class.
    static const std::string OSNAME_ENCODING;
    /// ObjectState attribute key for the encoding.
    static const std::string OSKEY_ENCODING;
    /// ObjectState attribute key for the number of extra bits.
    static const std::string OSKEY_EXTRA_BITS;

protected:
    Encoding(
        unsigned int encoding,
        unsigned int extraBits,
        InstructionField* parent);
    Encoding(const ObjectState* state, InstructionField* parent);
    void setParent(InstructionField* parent);

private:
    /// The encoding.
    unsigned int encoding_;
    /// The number of extra bits.
    unsigned int extraBits_;
    /// The parent instruction field.
    InstructionField* parent_;
};

#endif
