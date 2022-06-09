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
 * @file RegisterFileState.hh
 *
 * Declaration of RegisterFileState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_FILE_STATE_HH
#define TTA_REGISTER_FILE_STATE_HH

#include <vector>

#include "Exception.hh"

class RegisterState;

//////////////////////////////////////////////////////////////////////////////
// RegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that manages RegisterState objects.
 */ 
class RegisterFileState {
public:
    explicit RegisterFileState(int size, int width);
    virtual ~RegisterFileState();

    virtual RegisterState& registerState(int index);

    virtual std::size_t registerCount() const;

private:
    /// Copying not allowed.
    RegisterFileState(const RegisterFileState&);
    /// Assignment not allowed.
    RegisterFileState& operator=(const RegisterFileState);

    /// Contains all the registers of the state.
    std::vector<RegisterState*> registerStates_;
};

//////////////////////////////////////////////////////////////////////////////
// NullRegisterFileState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing RegisterFileState.
 */
class NullRegisterFileState : public RegisterFileState {
public:
    static NullRegisterFileState& instance();

    virtual ~NullRegisterFileState();

    virtual RegisterState& registerState(int index);

private:
    NullRegisterFileState();
    /// Copying not allowed.
    NullRegisterFileState(const NullRegisterFileState&);
    /// Assignment not allowed.
    NullRegisterFileState& operator=(const NullRegisterFileState&);

    /// Unique instance of NullRegisterFileState.
    static NullRegisterFileState* instance_;
};

#endif
