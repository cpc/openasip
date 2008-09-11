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
 * @file RegisterFileState.cc
 *
 * Definition of RegisterFileState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "RegisterFileState.hh"
#include "RegisterState.hh"
#include "SequenceTools.hh"
#include "Application.hh"

using std::string;

//////////////////////////////////////////////////////////////////////////////
// RegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param size The size of the RegisterFileState.
 * @param width Width of the registers.
 */
RegisterFileState::RegisterFileState(int size, int width) {
    for (int i = 0; i < size; i++) {
        registerStates_.push_back(new RegisterState(width));
    }
}

/**
 * Destructor.
 */
RegisterFileState::~RegisterFileState() {
    SequenceTools::deleteAllItems(registerStates_);
}

/**
 * Returns RegisterState with a given index.
 *
 * @param index Index of the wanted RegisterState.
 * @return RegisterState with a given index.
 * @exception OutOfRange If index is out of range.
 */
RegisterState&
RegisterFileState::registerState(int index)
    throw (OutOfRange) {

    if (index < 0 || index > static_cast<int>(registerStates_.size()) - 1) {
        string msg = "Register index out of range";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    return *registerStates_[index];
}

/**
 * Returns the count of registers in the register file.
 *
 * @return Count of registers.
 */
std::size_t 
RegisterFileState::registerCount() const {
    return registerStates_.size();
}

//////////////////////////////////////////////////////////////////////////////
// NullRegisterFileState
//////////////////////////////////////////////////////////////////////////////

NullRegisterFileState* NullRegisterFileState::instance_ = NULL;

/**
 * Returns the instance of NullRegisterFileState.
 *
 * @return The instance of NullRegisterFileState.
 */
NullRegisterFileState&
NullRegisterFileState::instance() {
    if (instance_ == NULL) {
        instance_ = new NullRegisterFileState();
    }
    return *instance_;
}

/**
 * Constructor.
 */
NullRegisterFileState::NullRegisterFileState() : RegisterFileState(0, 0) {
}

/**
 * Destructor.
 */
NullRegisterFileState::~NullRegisterFileState() {
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 * @exception OutOfRange Never throws.
 */
RegisterState&
NullRegisterFileState::registerState(int)
    throw (OutOfRange) {

    Application::abortWithError("registerState()");
    return NullRegisterState::instance();
}
