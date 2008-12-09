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
 * @file NullBinary.hh
 *
 * Declaration of NullBinary class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_BINARY_HH
#define TTA_NULL_BINARY_HH

#include "Binary.hh"

namespace TPEF {

/**
 * A singleton class that represents a null binary.
 *
 * Calling any method causes the program to abort.
 */
class NullBinary : public Binary {
public:
    virtual ~NullBinary();
    static NullBinary& instance();

protected:
    NullBinary();

private:
    /// Copying not allowed.
    NullBinary(const NullBinary&);
    /// Assignment not allowed.
    NullBinary& operator=(const NullBinary&);

    /// Unique instance of NullBinary.
    static NullBinary instance_;
};

}

#endif
