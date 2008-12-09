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
 * @file RegisterState.hh
 *
 * Declaration of RegisterState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_REGISTER_STATE_HH
#define TTA_REGISTER_STATE_HH

#include <string>

#include "StateData.hh"
#include "SimValue.hh"


//////////////////////////////////////////////////////////////////////////////
// RegisterState
//////////////////////////////////////////////////////////////////////////////

/**
 * Class that represents the contents of a register.
 */
class RegisterState : public StateData {
public:
    RegisterState(int width);
    RegisterState(SimValue& sharedRegister);
    virtual ~RegisterState();
    
    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;
    
protected:
    /// Value of the RegisterState. @todo Fix this mutable mess.
    /// It's needed because OutputPortState needs to mask the value_ in
    /// its value() implementation.
    mutable SimValue& value_;

private:
    /// Copying not allowed.
    RegisterState(const RegisterState&);
    /// Assignment not allowed.
    RegisterState& operator=(const RegisterState&);    
    /// Is the storage of this RegisterState shared with someone else?
    bool shared_;
};

//////////////////////////////////////////////////////////////////////////////
// NullRegisterState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing RegisterState.
 */
class NullRegisterState : public RegisterState {
public:
    static NullRegisterState& instance();

    virtual ~NullRegisterState();
    
    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

private:
    NullRegisterState();
    /// Copying not allowed.
    NullRegisterState(const NullRegisterState&);
    /// Assignment not allowed.
    NullRegisterState& operator=(const NullRegisterState&);

    /// Unique instance of NullRegisterState.
    static NullRegisterState* instance_;
};

#endif
