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
 * @file GuardEncoding.hh
 *
 * Declaration of GuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUARD_ENCODING_HH
#define TTA_GUARD_ENCODING_HH

#include "Exception.hh"

class GuardField;
class ObjectState;

/**
 * GuardEncoding class captures the properties shared by all types of guard
 * encoding.
 */
class GuardEncoding {
public:
    virtual ~GuardEncoding();

    GuardField* parent() const;
    bool isGuardInverted() const;
    unsigned int encoding() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for guard encoding.
    static const std::string OSNAME_GUARD_ENCODING;
    /// ObjectState attribute key for invert flag.
    static const std::string OSKEY_INVERTED;
    /// ObjectState attribute key for encoding.
    static const std::string OSKEY_ENCODING;

protected:
    GuardEncoding(bool inverted, unsigned int encoding);
    GuardEncoding(const ObjectState* state);
    void setParent(GuardField* parent);

private:
    /// The "invert" flag.
    bool inverted_;
    /// The encoding.
    unsigned int encoding_;
    /// The parent guard field.
    GuardField* parent_;
};

#endif
