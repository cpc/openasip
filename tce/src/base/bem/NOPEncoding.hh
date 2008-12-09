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
 * @file NOPEncoding.hh
 *
 * Declaration of NOPEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NOP_ENCODING_HH
#define TTA_NOP_ENCODING_HH

#include "Encoding.hh"

class SlotField;
class ObjectState;

/**
 * Represents the encoding for NOP (no operation) in the source field.
 */
class NOPEncoding : public Encoding {
public:
    NOPEncoding(
        unsigned int encoding,
        unsigned int extraBits,
        SlotField& parent)
        throw (ObjectAlreadyExists);
    NOPEncoding(const ObjectState* state, SlotField& parent)
        throw (ObjectAlreadyExists, ObjectStateLoadingException);
    virtual ~NOPEncoding();

    SlotField* parent() const;

    virtual int bitPosition() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for NOP encoding.
    static const std::string OSNAME_NOP_ENCODING;
};

#endif
