/**
 * @file BasicBlockSelector.hh
 *
 * Declaration of BasicBlockSelector class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASIC_BLOCK_SELECTOR_HH
#define TTA_BASIC_BLOCK_SELECTOR_HH

#include "HelperSchedulerModule.hh"
#include "SchedulingScope.hh"

/**
 * A scope selector that returns returns basic blocks from control flow
 * graphs one at a time.
 */
class BasicBlockSelector : public SchedulingScope {
public:
    BasicBlockSelector();
    virtual ~BasicBlockSelector();

    virtual void buildScopes();
    virtual SchedulingScope* getScope();
};

#endif
