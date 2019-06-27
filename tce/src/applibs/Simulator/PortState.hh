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
 * @file PortState.hh
 *
 * Declaration of PortState class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PORT_STATE_HH
#define TTA_PORT_STATE_HH

#include <string>

#include "RegisterState.hh"
#include "Application.hh"

class FUState;

//////////////////////////////////////////////////////////////////////////////
// PortState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models port of function unit.
 */
class PortState : public RegisterState {
public:
    PortState(FUState& parent, int width);
    PortState(FUState& parent, SimValue& sharedRegister);
    virtual ~PortState();

    virtual FUState& parent() const;

protected:
    /// Parent of the port.
    FUState* parent_;
private:
    /// Copying not allowed.
    PortState(const PortState&);
    /// Assignment not allowed.
    PortState& operator=(const PortState&);    
};

//////////////////////////////////////////////////////////////////////////////
// NullPortState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing PortState.
 */
class NullPortState : public PortState {
public:
    static NullPortState& instance();

    virtual ~NullPortState();
    
    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;
    virtual FUState& parent() const;

private:
    NullPortState();
    /// Copying not allowed.
    NullPortState(const NullPortState&);
    /// Assignment not allowed.
    NullPortState& operator=(const NullPortState&);

    /// Unique instance of NullPortState class.
    static NullPortState* instance_;
};

#endif
