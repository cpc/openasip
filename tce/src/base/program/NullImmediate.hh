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
 * @file NullImmediate.hh
 *
 * Declaration of NullImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_IMMEDIATE_HH
#define TTA_NULL_IMMEDIATE_HH

#include "Immediate.hh"

namespace TTAProgram {

/**
 * A singleton class that represents a null immediate.
 *
 * Calling any method causes the program to abort.
 */
class NullImmediate : public Immediate {
public:
    virtual ~NullImmediate();
    static NullImmediate& instance();

    Terminal& destination();
    TerminalImmediate& value();

protected:
    NullImmediate();

private:
    /// Copying not allowed.
    NullImmediate(const NullImmediate&);
    /// Assignment not allowed.
    NullImmediate& operator=(const NullImmediate&);

    static NullImmediate instance_;
};

}

#endif
