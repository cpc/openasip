/**
 * @file WritableState.hh
 *
 * Declaration of WritableState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_WRITABLE_STATE_HH
#define TTA_WRITABLE_STATE_HH

class SimValue;

/**
 * Models a piece of machine state that is writable.
 */
class WritableState {
public:
    WritableState();
    virtual ~WritableState();

    virtual void setValue(const SimValue& value) = 0;

private:
    /// Copying not allowed.
    WritableState(const WritableState&);
    /// Assignment not allowed.
    WritableState& operator=(const WritableState&);
};

#endif
