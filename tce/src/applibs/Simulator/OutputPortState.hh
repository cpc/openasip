/**
 * @file OutputPortState.hh
 *
 * Declaration of OutputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUT_PORT_STATE_HH
#define TTA_OUTPUT_PORT_STATE_HH

#include <string>

#include "PortState.hh"

/**
 * Models an output port state of function unit.
 */
class OutputPortState : public PortState {
public:
    OutputPortState(
        FUState& parent, 
        std::size_t width);

    OutputPortState(
        FUState& parent, 
        SimValue& registerToUse);

    virtual ~OutputPortState();
   
private:
    /// Copying not allowed.
    OutputPortState(const OutputPortState&);
    /// Assignment not allowed.
    OutputPortState& operator=(const OutputPortState&);
};

#endif
