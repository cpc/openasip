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
 * @file NullImmediate.cc
 *
 * Implementation of NullImmediate class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullImmediate.hh"
#include "NullTerminal.hh"
#include "NullInstructionTemplate.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullImmediate
/////////////////////////////////////////////////////////////////////////////

NullImmediate NullImmediate::instance_;

/**
 * The constructor.
 */
NullImmediate::NullImmediate() :
    Immediate(
        NULL, &NullTerminal::instance()) {
}

/**
 * The destructor.
 */
NullImmediate::~NullImmediate() {
}

/**
 * Returns an instance of NullImmediate class (singleton).
 *
 * @return Singleton instance of NullImmediate class.
 */
NullImmediate&
NullImmediate::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Terminal&
NullImmediate::destination() {
    abortWithError("destination()");
    return NullTerminal::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return Null since there is no NullTerminalImmediate.
 */
TerminalImmediate& NullImmediate::value() {
    abortWithError("value()");
    return *static_cast<TerminalImmediate*>(NULL);
}

}
