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
 * @file TriggeringInputPortState.hh
 *
 * Declaration of TriggeringInputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRIGGERING_INPUT_PORT_STATE_HH
#define TTA_TRIGGERING_INPUT_PORT_STATE_HH

#include <string>

#include "InputPortState.hh"

class SimValue;

/**
 * Models input port state that triggers operation.
 */
class TriggeringInputPortState : public InputPortState {
public:
    TriggeringInputPortState(
        FUState& parent, 
        int width);

    TriggeringInputPortState(
        FUState& parent, 
        SimValue& sharedRegister);

    virtual ~TriggeringInputPortState();

    virtual void setValue(const SimValue& value);

private:
    /// Copying not allowed.
    TriggeringInputPortState(const TriggeringInputPortState&);
    /// Assignment not allowed.
    TriggeringInputPortState& operator=(const TriggeringInputPortState&);
};

#endif
