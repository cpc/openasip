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
 * @file StateLocator.cc
 *
 * Definition of StateLocator class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "StateLocator.hh"
#include "StateData.hh"
#include "MachinePart.hh"

using std::string;
using namespace TTAMachine;

/**
 * Constructor.
 */
StateLocator::StateLocator() {
}

/**
 * Destructor.
 */
StateLocator::~StateLocator() {
}

/**
 * Adds state to to StateLocator.
 *
 * @param momComponent Machine object model component.
 * @param state Machine state model state.
 */
void
StateLocator::addState(const MachinePart& momComponent, StateData& state) {
    StateContainer::iterator iter = states_.find(&momComponent);
    if (iter != states_.end()) {
        (*iter).second = &state;
    } else {
        states_[&momComponent] = &state;
    }
}

/**
 * Returns state corresponding to the given component.
 *
 * @param momComponent Component.
 * @return State corresponding to given component.
 * @exception IllegalParameters If state is not found.
 */
StateData&
StateLocator::state(const MachinePart& momComponent) const
    throw (IllegalParameters) {

    StateContainer::const_iterator iter = states_.find(&momComponent);
    if (iter == states_.end()) {
        string msg = "State corresponding to component not found";
        throw IllegalParameters(__FILE__, __LINE__, __func__, msg);
    }

    return *((*iter).second);
}
