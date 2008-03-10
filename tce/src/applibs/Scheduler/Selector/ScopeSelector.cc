/**
 * @file ScopeSelector.hh
 *
 * Dummy implementation of ScopeSelector interface.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#include "ScopeSelector.hh"

/**
 * Destructor for Scope Selector interface class.
 */
ScopeSelector::~ScopeSelector() {
}

/**
 * Builds the scopes. 
 *
 * This function has to be called before getScope can be called.
 *
 * @note Why? What's wrong with constructors?
 */
void 
ScopeSelector::buildScopes() {
}

/**
 * Returns a scope to schedule next.
 *
 * The client is responsible for deallocating the scopes after using 
 * This returns all scopes in program one-by-one, in undetermined order in
 * the point-of-view of the caller.
 *
 * @return A scheduling scope, or NULL if all scopes already given.
 */
SchedulingScope*
ScopeSelector::getScope() {
    throw NotAvailable(
        __FILE__,__LINE__, "ScopeSelector::getScope()",
        "This function should never be called");
}

