/**
 * @file GuardState.hh
 *
 * Declaration of GuardState class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
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
        int latency, 
        GlobalLock& lock);

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
 * Models a GuardState that has a latency of 1.
 */
class OneClockGuardState : public GuardState {
public:
    OneClockGuardState(const ReadableState& targetRegister);
    
    virtual ~OneClockGuardState();
    
    virtual const SimValue& value() const;

    virtual void endClock();
    virtual void advanceClock();
    
private:
    OneClockGuardState();
    /// Copying not allowed.
    OneClockGuardState(const OneClockGuardState&);
    /// Assignment not allowed.
    OneClockGuardState& operator=(const OneClockGuardState&);
    
    /// The target register watched by this guard.
    const ReadableState* target_;
};

#endif
