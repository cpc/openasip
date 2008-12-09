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
 * @file LongImmUpdateAction.hh
 *
 * Declaration of LongImmUpdateAction class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_LONG_IMM_UPDATE_ACTION_HH
#define TTA_LONG_IMM_UPDATE_ACTION_HH

#include "SimValue.hh"
class LongImmediateRegisterState;

/**
 * Updates the value of the long immediate register state.
 */
class LongImmUpdateAction {
public:
    LongImmUpdateAction(SimValue value, LongImmediateRegisterState& state);
    virtual ~LongImmUpdateAction();

    void execute();

private:
    /// Copying not allowed.
    LongImmUpdateAction(const LongImmUpdateAction&);
    /// Assignment not allowed.
    LongImmUpdateAction& operator=(const LongImmUpdateAction&);

    /// Value to be updated.
    SimValue value_;
    /// Destination register.
    LongImmediateRegisterState& state_;
};

#endif
