/**
 * @file InputPortState.hh
 *
 * Declaration of InputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUT_PORT_STATE_HH
#define TTA_INPUT_PORT_STATE_HH

#include <string>

#include "PortState.hh"

class FUState;

/**
 * Models input port state of the function unit.
 */
class InputPortState : public PortState {
public:
    InputPortState(FUState& parent, int width);
    InputPortState(
        FUState& parent, 
        SimValue& registerToUse);

    virtual ~InputPortState();

private:
    /// Copying not allowed.
    InputPortState(const InputPortState&);
    /// Assignment not allowed.
    InputPortState& operator=(const InputPortState&);
};

#endif
