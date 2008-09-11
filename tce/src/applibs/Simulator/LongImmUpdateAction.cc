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
 * @file LongImmUpdateAction.cc
 *
 * Definition of LongImmUpdateAction class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "LongImmUpdateAction.hh"
#include "SimValue.hh"
#include "LongImmediateRegisterState.hh"
#include "Application.hh"

/**
 * Constructor.
 *
 * @param value Value to be updated.
 * @param state Destination state.
 */
LongImmUpdateAction::LongImmUpdateAction(
    SimValue value, 
    LongImmediateRegisterState& state) : value_(value), state_(state) {
}

/**
 * Destructor.
 */
LongImmUpdateAction::~LongImmUpdateAction() {
}

/**
 * Updates the value of the destination register state.
 */
void
LongImmUpdateAction::execute() {
    state_.setValue(value_);
}
