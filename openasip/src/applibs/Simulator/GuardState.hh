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
 * @file GuardState.hh
 *
 * Declaration of GuardState class.
 *
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_GUARD_STATE_HH
#define TTA_GUARD_STATE_HH

#include <vector>

#include "ClockedState.hh"
#include "ReadableState.hh"

class GlobalLock;

//////////////////////////////////////////////////////////////////////////////
// GuardState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models the programmer visible delay of guards with latency more than 1.
 *
 * GuardState with latency of 1 is the default latency which can be modeled
 * with a direct reference to the guarded register, thus this class should
 * not be used to model such cases. Guard latency of 1 means that the guard
 * value can be used in the same instruction in which the value itself can
 * be used. Thus, in case of a register guard, the instruction following the
 * write to the register. For example, guard with latency of 2 means that there
 * is one instruction cycle after updating the value of the guard in which
 * the new value is not yet visible, and so on. This latency is modeled with
 * a ring buffer which represents the history of guard's target values.
 */
class GuardState : public ClockedState, public ReadableState {
public:
    GuardState(
        const ReadableState& targetRegister, 
        int latency);

    virtual ~GuardState();

    virtual const SimValue& value() const;

    virtual void endClock();
    virtual void advanceClock();

protected:
    /// Only subclasses allowed to create empty GuardStates
    GuardState();

private:

    /// Copying not allowed.
    GuardState(const GuardState&);
    /// Assignment not allowed.
    GuardState& operator=(const GuardState&);
    /// The target register watched by this guard.
    const ReadableState* target_;
    /// Value history ring buffer.
    std::vector<SimValue> history_;
    /// History ring buffer position. Point to the index of the current
    /// value of the guard.
    int position_;    
};

//////////////////////////////////////////////////////////////////////////////
// NullGuardState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing GuardState.
 */
class NullGuardState : public GuardState {
public:
    static NullGuardState& instance();

    virtual ~NullGuardState();
    
private:
    NullGuardState();
    /// Copying not allowed.
    NullGuardState(const NullGuardState&);
    /// Assignment not allowed.
    NullGuardState& operator=(const NullGuardState&);
    
    /// Unique instance of NullGuardState (singleton).
    static NullGuardState instance_;
};

//////////////////////////////////////////////////////////////////////////////
// OneClockGuardState
//////////////////////////////////////////////////////////////////////////////
/**
 * Models a GuardState that reads value immediately from the source.
 * Register guard latency 1 or port guard latency 0
 */
class DirectGuardState : public GuardState {
public:
    DirectGuardState(const ReadableState& targetRegister);
    
    virtual ~DirectGuardState();
    
    virtual const SimValue& value() const;

    virtual void endClock();
    virtual void advanceClock();
    
private:
    DirectGuardState();
    /// Copying not allowed.
    DirectGuardState(const DirectGuardState&);
    /// Assignment not allowed.
    DirectGuardState& operator=(const DirectGuardState&);
    
    /// The target register watched by this guard.
    const ReadableState* target_;
};

#endif
