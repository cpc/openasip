/**
 * @file ScopeSelector.hh
 *
 * Declaration of ScopeSelector interface.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCOPE_SELECTOR_HH
#define TTA_SCOPE_SELECTOR_HH

#include "HelperSchedulerModule.hh"
#include "SchedulingScope.hh"

/**
 * Scope selectors return one scheduling scope at a time to be scheduled.
 *
 * For example, a basic block scope selector returns basic blocks from the
 * CFG one at a time.
 */
class ScopeSelector : public HelperSchedulerModule {
public:
    virtual ~ScopeSelector();

    virtual void buildScopes() = 0;
    virtual SchedulingScope* getScope() = 0;
};

#endif
