/**
 * @file OperationContext.cc
 *
 * Non-inline definitions of OperationContext class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include <string>

#include "OperationContext.hh"
#include "Application.hh"
#include "Memory.hh"

using std::string;

/// Id given for the next created OperationContext instance.
int OperationContext::nextContextId_ = 0;

/// Integer used to represent an illegal natural word width.
const int SOME_ILLEGAL_NATURAL_WORD_WIDTH = -1;

InstructionAddress dummyInstructionAddress;

/**
 * Constructor for contexts suitable for basic operations.
 */
OperationContext::OperationContext() : 
    memory_(NULL), 
    programCounter_(dummyInstructionAddress), 
    returnAddress_(NullSimValue::instance()),
    saveReturnAddress_(false)  {
    initializeContextId();
}

/**
 * Constructor for contexts suitable for any kinds of operations.
 *
 * @param memory The memory model instance.
 * @param nww The natural word width of the memory model.
 * @param programCounter The program counter register.
 * @param returnAddress The return address register.
 * @param syscallHandler The syscall handler register.
 * @param syscallNumber The syscall code register.
 *
 */
OperationContext::OperationContext(
    Memory* memory,
    InstructionAddress& programCounter,
    SimValue& returnAddress) :
    memory_(memory), programCounter_(programCounter), 
    returnAddress_(returnAddress), saveReturnAddress_(false) {
    initializeContextId();
}

/**
 * Destructor.
 */
OperationContext::~OperationContext() {
}

/**
 * Looks up the (concrete) operation state identified by the string name.
 *
 * @param name The state identifier.
 * @return The found operation state instance.
 * @exception KeyNotFound If state by name couldn't be found.
 */
OperationState&
OperationContext::state(const std::string& name) const
    throw (KeyNotFound) {

    StateRegistry::const_iterator i = stateRegistry_.find(name);

    if (i == stateRegistry_.end()) {
        throw KeyNotFound(__FILE__, __LINE__, __func__, "State not found.");
    }

    return *(*i).second;
}

/**
 * Advances the internal clock of each registered operation state object.
 */
void 
OperationContext::advanceClock() {

    StateRegistry::iterator i = stateRegistry_.begin();

    while (i != stateRegistry_.end()) {
        (*i).second->advanceClock(*this);
        ++i;
    }
}

/**
 * Registers the operation state for given name.
 *
 * Called in the createState() of the custom OperationBehavior classes.
 * If state with given identifier is already found in the context, program
 * is aborted. This method is only for internal use. Used by the macro 
 * definitions of OSAL.hh.
 *
 * @param name The identifier for the registered state instance.
 * @param stateToRegister The state instance.
 */
void 
OperationContext::registerState(OperationState* stateToRegister) {
    string stateName = stateToRegister->name();
    // RegisterState() and unregisterState() are supposed to be used
    // only internally, they are not part of the "client IF", therefore
    // it's reasonable to assert.
    assert(!hasState(stateName));
    stateRegistry_[stateName] = stateToRegister;	
}

/**
 * Unregisters the operation state of given name.
 *
 * This method is only for internal use. Used by the macro 
 * definitions of OSAL.hh.
 *
 * Called in deleteState() of the custom OperationBehavior classes.
 * Aborts if there's no state with given identifier in the context.
 */    
void 
OperationContext::unregisterState(const std::string& name) {
    // RegisterState() and unregisterState() are supposed to be used
    // only internally, they are not part of the "client IF", therefore
    // it's reasonable to assert.
    assert(hasState(name));
    stateRegistry_.erase(name);
}

/**
 * Checks if state with given name can be found in the context.
 *
 * @param name Name of the state.
 * @return True if the state is found.
 */
bool 
OperationContext::hasState(const std::string& name) const {
    try {
        state(name);
    } catch (const KeyNotFound&) {
        return false;
    }
    return true;    
}
