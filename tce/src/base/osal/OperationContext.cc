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
#include "OperationState.hh"
#include "SimValue.hh"
#include "OperationContextPimpl.hh"

using std::string;

/**
 * Constructor for contexts suitable for basic operations.
 */
OperationContext::OperationContext() : pimpl_(new OperationContextPimpl()) {
}

/**
 * Constructor for contexts suitable for any kinds of operations.
 *
 * @param memory The memory model instance.
 * @param programCounter The program counter register.
 * @param returnAddress The return address register.
 *
 */
OperationContext::OperationContext(
    Memory* memory,
    InstructionAddress& programCounter,
    SimValue& returnAddress) :
    pimpl_(new OperationContextPimpl(
        memory, programCounter, returnAddress)) {
}

/**
 * A copy constructor that performs a deep copy for the pimpl_
 * 
 * @param context OperationContext
 */
OperationContext::OperationContext(const OperationContext& context) : 
    pimpl_(new OperationContextPimpl(*context.pimpl_)) {
}

/**
 * Destructor. Deletes the pimpl object
 */
OperationContext::~OperationContext() {
    delete pimpl_;
    pimpl_ = NULL;
}

/**
 * Looks up the (concrete) operation state identified by the string name.
 *
 * @param name The state identifier.
 * @return The found operation state instance.
 * @exception KeyNotFound If state by name couldn't be found.
 */
OperationState&
OperationContext::state(const char* name) const {
    return pimpl_->state(name);
}

/**
 * Advances the internal clock of each registered operation state object.
 */
void 
OperationContext::advanceClock() {
    pimpl_->advanceClock(*this);
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
    pimpl_->registerState(stateToRegister);
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
OperationContext::unregisterState(const char* name) {
    pimpl_->unregisterState(name);
}

/**
 * Returns a reference to the current value of the program counter register.
 *
 * The value of the program counter can be changed through this reference. 
 * This is used to implement control transfer operations like jumps and calls 
 * to subroutines. 
 *
 * @return The program counter value as a reference.
 */
InstructionAddress& OperationContext::programCounter() {
    return pimpl_->programCounter();
}
        
/**
 * Makes the return address to be saved in the RA register.
 *
 * This is used by CALL instruction to save the RA before jumping.
 *
 * @param value Value to set to.
 * @return The return address value as a reference.
 */
void 
OperationContext::setSaveReturnAddress(bool value) {
    pimpl_->setSaveReturnAddress(value);
}   

/**
 * Returns true if RA should saved before executing next control flow 
 * operation.
 *
 * @return The return address value as a reference.
 */
bool 
OperationContext::saveReturnAddress() {
    return pimpl_->saveReturnAddress();
}  

/**
 * Returns a reference to the current value of the return address register.
 *
 * The value of the return address can be changed through this reference. 
 * This is used in implementing calls to subroutines.
 *
 * @return The return address value as a reference.
 */
SimValue& 
OperationContext::returnAddress() {
    return pimpl_->returnAddress();
} 

/**
 * Returns true if there are no operation state objects stored in the
 * context.
 *
 * @return True if there are no operation state objects stored in the
 * context.
 */
bool 
OperationContext::isEmpty() const {
    return pimpl_->isEmpty();
}
        
        
/**
 * Sets the reference to a Memory Module instance.
 *
 * This instance can be accessed by the function unit and behavior 
 * simulation methods to simulate memory access.
 *
 * @param memory The Memory instance.
 */
void
OperationContext::setMemory(Memory* memory) {
    pimpl_->setMemory(memory);
}

/**
 * Returns a reference to a Memory Module wrapper instance.
 *
 * This instance can be accessed by the function unit and behavior 
 * simulation methods to simulate memory access.
 *
 * @return The Memory instance.
 */
Memory& 
OperationContext::memory() {
    return pimpl_->memory();
}

/**
 * Returns the unique id of the OperationContext instance.
 *
 * @return The unique id for the OperationContext instance.
 */
int 
OperationContext::contextId() const {
    return pimpl_->contextId();
}

/**
 * Returns the elapsed time since the beginning of simulation.
 *
 * Can be used to implement real time timer operations, etc. Uses
 * the simulator's cycle count, if available, otherwise uses internal
 * cycle counter that is incremented with the advanceClock();
 */
CycleCount
OperationContext::cycleCount() const {
    return pimpl_->cycleCount(); 
}

/**
 * Sets the variable that contains the current simulation cycle count
 * at the best possible accuracy. 
 *
 * Used by real time operations to track simulation time during
 * simulations.
 */
void 
OperationContext::setCycleCountVariable(CycleCount& cycleCount) {
    pimpl_->setCycleCountVariable(cycleCount);
}
