/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
MoveGuard::MoveGuard(TTAMachine::Guard& guard):
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
Guard&
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
