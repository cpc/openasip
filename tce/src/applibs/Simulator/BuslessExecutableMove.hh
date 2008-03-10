/**
 * @file BuslessExecutableMove.hh
 *
 * Declaration of BuslessExecutableMove class.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUSLESS_EXECUTABLE_MOVE_HH
#define TTA_BUSLESS_EXECUTABLE_MOVE_HH

#include "ExecutableMove.hh"
#include "InlineImmediateValue.hh"

class ReadableState;
class BusState;
class WritableState;
class SimValue;


/**
 * Represents an interpreted move which does not utilize a transport bus.
 *
 * Moves that utilize this class include control unit internal moves such
 * as control flow operations (jump, call) with immediate or RA source. 
 * This class can be used also for simulating data transports of sequential 
 * code, as writing to (the universal) bus is not necessary in that case.
 */
class BuslessExecutableMove : public ExecutableMove {
public:
    BuslessExecutableMove(
        const ReadableState& src, 
        WritableState& dst);

    BuslessExecutableMove(
        const ReadableState& src,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    BuslessExecutableMove(
        InlineImmediateValue* immediateSource,
        WritableState& dst,
        const ReadableState& guardReg,
        bool negated);

    BuslessExecutableMove(
        InlineImmediateValue* immediateSource,
        WritableState& dst);
    
    virtual ~BuslessExecutableMove();

    virtual void executeRead();
    virtual void executeWrite();

private:
    /// Copying not allowed.
    BuslessExecutableMove(const BuslessExecutableMove&);
    /// Assignment not allowed.
    BuslessExecutableMove& operator=(const BuslessExecutableMove&);
};

#endif
