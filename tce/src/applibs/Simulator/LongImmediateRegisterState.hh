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
 * @file LongImmediateRegisterState.hh
 *
 * Declaration of LongImmediateRegisterState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LONG_IMMEDIATE_REGISTER_STATE_HH
#define TTA_LONG_IMMEDIATE_REGISTER_STATE_HH

#include <string>

#include "StateData.hh"

class LongImmediateUnitState;
class SimValue;

/**
 * Represents a register of a long immediate unit.
 */
class LongImmediateRegisterState : public StateData {
public:
    LongImmediateRegisterState(
        LongImmediateUnitState* parent, 
        int index,
        int width,
        bool signExtend);
    virtual ~LongImmediateRegisterState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

private:
    /// Copying not allowed.
    LongImmediateRegisterState(const LongImmediateRegisterState&);
    /// Assignment not allowed.
    LongImmediateRegisterState& operator=(const LongImmediateRegisterState&);

    /// Parent unit of the register state.
    LongImmediateUnitState* parent_;
    /// Index of the register.
    int index_;
    /// The bit width of the register (needed only while extending).
    int width_;
    /// True in case the written values should be sign extended.
    bool signExtend_;
};

#endif
