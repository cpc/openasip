/**
 * @file ReadableState.hh
 *
 * Declaration of ReadableState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_READABLE_STATE_HH
#define TTA_READABLE_STATE_HH

class SimValue;

/**
 * Models a piece of machine state model that can be read.
 */
class ReadableState {
public:
    ReadableState();
    virtual ~ReadableState();

    virtual const SimValue& value() const = 0;

private:
    /// Copying not allowed.
    ReadableState(const ReadableState&);
    /// Assignment not allowed.
    ReadableState& operator=(const ReadableState&);
};

#endif
