/**
 * @file StateLocator.hh
 *
 * Declaration of StateLocator class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_STATE_LOCATOR_HH
#define TTA_STATE_LOCATOR_HH

#include <map>

#include "Exception.hh"

class StateData;

namespace TTAMachine {
    class MachinePart;
}

/**
 * Class that makes it possible to lookup state objects corresponding to
 * MOM component instances.
 */
class StateLocator {
public:
    StateLocator();
    virtual ~StateLocator();

    void addState(
        const TTAMachine::MachinePart& momComponent, 
        StateData& state);
    StateData& state(const TTAMachine::MachinePart& momComponent) const
        throw (IllegalParameters);
private:
    /// Copying not allowed.
    StateLocator(const StateLocator&);
    /// Assignment not allowed.
    StateLocator& operator=(const StateLocator&);

    /// State objects indexed by MOM components.
    typedef std::map<const TTAMachine::MachinePart*, StateData*> 
    StateContainer;
    
    /// Contains all the states indexed by MOM components.
    StateContainer states_;
};

#endif
