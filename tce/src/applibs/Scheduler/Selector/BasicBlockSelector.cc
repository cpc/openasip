/**
 * @file BasicBlockSelector.hh
 *
 * Dummy implementation of BasicBlockSelector interface.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include "BasicBlockSelector.hh"
#include "Application.hh"

/**
 * Constructor for BasicBlockSelector class.
 */
BasicBlockSelector::BasicBlockSelector() {
}

/**
 * Destructor for BasicBlockSelector class.
 */
BasicBlockSelector::~BasicBlockSelector() {
}

/**
 * Builds the scopes. 
 *
 * This function has to be called before getScope can be called.
 *
 * @note Why? What's wrong with constructors?
 */
void 
BasicBlockSelector::buildScopes() {
    abortWithError("Not yet implemented.");
}

/**
 * Returns a basic block to schedule next.
 *
 * The client is responsible for deallocating the returned basic block 
 * after using. This returns all basic blocks in the program one-by-one, one
 * procedure at a time, until all basic blocks have been returned.
 *
 * @return SchedulingScope, or NULL if all scopes already given.
 */
SchedulingScope*
BasicBlockSelector::getScope() {
    abortWithError("Not yet implemented.");
    return NULL;
}
