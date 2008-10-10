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
 * @file UnboundRegisterFileState.cc
 *
 * Definition of UnboundRegisterFileState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "UnboundRegisterFileState.hh"
#include "RegisterState.hh"
#include "SequenceTools.hh"

/**
 * Constructor.
 */
UnboundRegisterFileState::UnboundRegisterFileState() : 
    RegisterFileState(-1, 32) {
}

/**
 * Destructor.
 */
UnboundRegisterFileState::~UnboundRegisterFileState() {
    SequenceTools::deleteAllItems(registerStates_);
}

/**
 * Returns a register with a given index.
 *
 * @param index The index of the wanted RegisterState.
 * @return RegisterState with a given index.
 * @exception OutOfRange Never throws.
 */
RegisterState&
UnboundRegisterFileState::registerState(int index) 
    throw (OutOfRange) {

    if (index > static_cast<int>(registerStates_.size()) - 1) {
        registerStates_.resize(index + 1, NULL);
    }

    if (registerStates_[index] == NULL) {
        registerStates_[index] = new RegisterState(32);
    }

    return *registerStates_[index];
}

/**
 * Returns the count of registers in the register file.
 *
 * @return Count of registers.
 */
std::size_t 
UnboundRegisterFileState::registerCount() const {
    return registerStates_.size();
}

/**
 * Returns true if the register with given index is in use.
 *
 * Register is considered to be in use when it's been written to,
 * that is, the SimValue storing its value has isActive() == true.
 */
bool
UnboundRegisterFileState::isRegisterInUse(std::size_t index) const {
    return registerStates_[index] != NULL &&
        registerStates_[index]->value().isActive();
}
