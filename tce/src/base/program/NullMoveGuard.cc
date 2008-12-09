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
 * @file NullMoveGuard.cc
 *
 * Implementation of NullMoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "NullMoveGuard.hh"
#include "Application.hh"
#include "NullTerminal.hh"
#include "NullGuard.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// NullMoveGuard
/////////////////////////////////////////////////////////////////////////////

NullMoveGuard NullMoveGuard::instance_;

/**
 * The constructor.
 */
NullMoveGuard::NullMoveGuard() :
    MoveGuard(NullGuard::instance()) {
}

/**
 * The destructor.
 */
NullMoveGuard::~NullMoveGuard() {
}

/**
 * Returns an instance of NullMoveGuard class (singleton).
 *
 * @return Singleton instance of NullMoveGuard class.
 */
NullMoveGuard&
NullMoveGuard::instance() {
    return instance_;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullMoveGuard::isUnconditional() const {
    abortWithError("isUnconditional()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullMoveGuard::isInverted() const {
    abortWithError("isInverted()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return A null terminal.
 */
Guard&
NullMoveGuard::guard() const {
    abortWithError("source()");
    return NullGuard::instance();
}

}
