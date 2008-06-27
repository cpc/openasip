/**
 * @file ExecutableMove.hh
 *
 * Declaration of ExecutableMove class.
 *
 * @author Jussi Nyk‰nen 2005 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_EXECUTABLE_MOVE_HH
#define TTA_EXECUTABLE_MOVE_HH

#include "InlineImmediateValue.hh"
#include "SimulatorConstants.hh"

class ReadableState;
class BusState;
class WritableState;
class SimValue;


/**
 * Represents an interpreted move.
 *
 * Interpreted means that source and destination of the move are already
 * resolved.
 */
class ExecutableMove {
public:
    ExecutableMove(
        const ReadableState& src, 
        BusState& bus, 
        WritableState& dst);

    ExecutableMove(
        const ReadableState& src,
        BusState& bus,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    ExecutableMove(
        InlineImmediateValue* immediateSource,
        BusState& bus,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    ExecutableMove(
        InlineImmediateValue* immediateSource,
        BusState& bus,
        WritableState& dst);
    
    virtual ~ExecutableMove();

    virtual void executeRead();
    virtual void executeWrite();
    virtual void evaluateGuard();
    virtual bool squashed() const;

    ClockCycleCount executionCount() const;
    void resetExecutionCount();

protected:
    ExecutableMove();
    
    /// Source of the move.
    const ReadableState* src_;
    /// Bus of the move.
    BusState* bus_;
    /// Destination of the move.
    WritableState* dst_;
    /// Guard of the move.
    const ReadableState* guardReg_;
    /// True if this is a guarded move.
    const bool guarded_;
    /// True if guard is inverted.
    const bool negated_;
    /// The count of times this move has been fully executed (without squash).
    ClockCycleCount executionCount_;
    /// True in case this move was squashed last time it was executed.
    bool squashed_;
   
private:
    /// Copying not allowed.
    ExecutableMove(const ExecutableMove&);
    /// Assignment not allowed.
    ExecutableMove& operator=(const ExecutableMove&);
    /// If the move source is an inline immediate, the instance
    /// is stored here so it can be deleted in the destructor.
    InlineImmediateValue* inlineImmediate_;
};

/**
 * Extremely simple wrapper for ExecutableMove, takes no params but exec count.
 */
class DummyExecutableMove : public ExecutableMove {
public:
    DummyExecutableMove(ClockCycleCount executionCount);
private:
};

#endif
