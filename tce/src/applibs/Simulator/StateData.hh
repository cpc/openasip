/**
 * @file StateData.hh
 *
 * Declaration of StateData class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STATE_DATA_HH
#define TTA_STATE_DATA_HH

#include "WritableState.hh"
#include "ReadableState.hh"

/**
 * Each accessible object of machine state model implements usually this
 * interface.
 */
class StateData : public WritableState, public ReadableState {
public:
    StateData();
    virtual ~StateData();

private:
    /// Copying not allowed.
    StateData(const StateData&);
    /// Assignment not allowed.
    StateData& operator=(const StateData&);
};

#endif
