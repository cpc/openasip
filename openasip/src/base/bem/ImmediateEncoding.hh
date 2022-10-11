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
 * @file ImmediateEncoding.hh
 *
 * Declaration of ImmediateEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IMMEDIATE_ENCODING_HH
#define TTA_IMMEDIATE_ENCODING_HH

#include "Encoding.hh"

class SourceField;
class ObjectState;

/**
 * Represents the encoding for inline immediate within the source field.
 */
class ImmediateEncoding : public Encoding {
public:
    ImmediateEncoding(
        unsigned int encoding, unsigned int extraBits, int immediateWidth,
        SourceField& parent);
    ImmediateEncoding(const ObjectState* state, SourceField& parent);
    virtual ~ImmediateEncoding();

    SourceField* parent() const;
    int immediateWidth() const;
    int encodingWidth() const;
    int encodingPosition() const;
    int immediatePosition() const;

    virtual int bitPosition() const;
    virtual int width() const;
    virtual ObjectState* saveState() const;

    /// ObjectState name for immediate encoding.
    static const std::string OSNAME_IMM_ENCODING;
    /// ObjectState attribute key for the immediate width.
    static const std::string OSKEY_IMM_WIDTH;

private:
    int immediateWidth_;
};

#endif
