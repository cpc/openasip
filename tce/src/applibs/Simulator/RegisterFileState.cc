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
 * @file RegisterFileState.cc
 *
 * Definition of RegisterFileState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
RegisterFileState::registerState(int index) {
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
NullRegisterFileState::registerState(int) {
    Application::abortWithError("registerState()");
    return NullRegisterState::instance();
}
