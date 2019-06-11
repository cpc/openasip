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
 * @file OperationState.hh
 *
 * Declaration of OperationState class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_STATE_HH
#define TTA_OPERATION_STATE_HH

class OperationContext;
class SimValue;

/**
 * OperationState is the base class of all operation state classes.
 *
 * Operation state classes store state information specific to a family of
 * operations. 
 */
class OperationState {
public:
    OperationState();
    virtual ~OperationState();

    virtual const char* name() = 0;
    virtual bool isAvailable(const OperationContext& context) const;
    virtual void advanceClock(OperationContext& context);
};

//////////////////////////////////////////////////////////////////////////////
// NullOperationState
//////////////////////////////////////////////////////////////////////////////

/**
 * Singleton class that is used to represent a null value for operation state.
 */
class NullOperationState : public OperationState {
public:
    static NullOperationState* instance();

    virtual const char* name();
    virtual bool isAvailable(const OperationContext& context) const;
    virtual void advanceClock(OperationContext& context);

private:
    static NullOperationState* instance_;
};

#include "OperationState.icc"

#endif
