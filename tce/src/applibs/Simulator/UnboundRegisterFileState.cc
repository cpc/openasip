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
