/**
 * @file TriggeringInputPortState.hh
 *
 * Declaration of TriggeringInputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
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
