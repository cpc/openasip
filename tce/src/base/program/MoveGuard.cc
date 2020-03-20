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
 * @file MoveGuard.cc
 *
 * Implementation of MoveGuard class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include "MoveGuard.hh"
#include "Guard.hh"

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// MoveGuard
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.

 * @param guard The guard.
 */
MoveGuard::MoveGuard(const TTAMachine::Guard& guard):
    guard_(&guard) {
}

/**
 * Destructor.
 */
MoveGuard::~MoveGuard() {
}

/**
 * Tells whether this guard is a constant true or false value.
 *
 * @return false always.
 */
bool
MoveGuard::isUnconditional() const {
    return false;
}

/**
 * Tells whether the guard is inverted or not.
 *
 * @return True if the Boolean value computed out of the contents of
 *              the source register is inverted.
 */
bool
MoveGuard::isInverted() const {
    return guard_->isInverted();
}

/**
 * Returns the guard object.
 *
 * @return The guard object.
 */
const Guard&
MoveGuard::guard() const {
    return *guard_;
}

/**
 * Makes a copy of the move guard.
 *
 * @return A copy of the move guard.
 */
MoveGuard*
MoveGuard::copy() const {
    return new MoveGuard(*guard_);
}

}
