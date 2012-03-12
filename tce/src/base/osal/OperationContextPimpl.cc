/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file OperationContextPimpl.cc
 *
 * Definition of OperationContextPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "OperationContextPimpl.hh"
#include "OperationContext.hh"
#include "Application.hh"
#include "Memory.hh"
#include "OperationState.hh"
#include "SimValue.hh"
#include "OperationContextPimpl.hh"
#include "Exception.hh"
#include <string>

using std::string;

/// Id given for the next created OperationContext instance.
int OperationContextPimpl::nextContextId_ = 0;

InstructionAddress dummyInstructionAddress;

/**
 * Constructor for contexts suitable for basic operations.
 */
OperationContextPimpl::OperationContextPimpl(std::string *name) : 
    memory_(NULL), 
    programCounter_(dummyInstructionAddress), 
    returnAddress_(NullSimValue::instance()),
    saveReturnAddress_(false), cycleCount_(0), 
    cycleCountVar_(NULL), FUName_(name) {
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
OperationContextPimpl::OperationContextPimpl(
	std::string *name,
    Memory* memory,
    InstructionAddress& programCounter,
    SimValue& returnAddress) :
    memory_(memory), programCounter_(programCounter), 
    returnAddress_(returnAddress), saveReturnAddress_(false), 
    cycleCount_(0), cycleCountVar_(NULL), FUName_(name) {
    initializeContextId();
}

/**
 * Default destructor
 */
OperationContextPimpl::~OperationContextPimpl() {
}

/**
 * Checks if state with given name can be found in the context.
 *
 * @param name Name of the state.
 * @return True if the state is found.
 */
bool 
OperationContextPimpl::hasState(const char* name) const {
    try {
        state(name);
    } catch (const KeyNotFound&) {
        return false;
    }
    return true;    
}

/**
 * Generates an unique context id for the current OperationContext instance.
 */
void
OperationContextPimpl::initializeContextId() {
    contextId_ = nextContextId_;
    nextContextId_++;
}

/**
 * Looks up the (concrete) operation state identified by the string name.
 *
 * @param name The state identifier.
 * @return The found operation state instance.
 * @exception KeyNotFound If state by name couldn't be found.
 */
OperationState&
OperationContextPimpl::state(const char* name) const {

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
OperationContextPimpl::advanceClock(OperationContext& context) {

    StateRegistry::iterator i = stateRegistry_.begin();

    while (i != stateRegistry_.end()) {
        (*i).second->advanceClock(context);
        ++i;
    }
    ++cycleCount_;
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
OperationContextPimpl::registerState(OperationState* stateToRegister) {
    string stateName = stateToRegister->name();
    // RegisterState() and unregisterState() are supposed to be used
    // only internally, they are not part of the "client IF", therefore
    // it's reasonable to assert.
    assert(!hasState(stateName.c_str()));
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
OperationContextPimpl::unregisterState(const char* name) {
    // RegisterState() and unregisterState() are supposed to be used
    // only internally, they are not part of the "client IF", therefore
    // it's reasonable to assert.
    assert(hasState(name));
    stateRegistry_.erase(name);
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
OperationContextPimpl::memory() {
    return *memory_;
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
OperationContextPimpl::setMemory(Memory* memory) {
    memory_ = memory;
}

/**
 * Returns the unique id of the OperationContext instance.
 *
 * @return The unique id for the OperationContext instance.
 */
int 
OperationContextPimpl::contextId() const {
    return contextId_;
}

/**
 * Returns the FU name of the OperationContext instance.
 *
 * @return The FU name for the OperationContext instance.
 */
std::string& 
OperationContextPimpl::functionUnitName() {
	return *FUName_;
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
InstructionAddress& 
OperationContextPimpl::programCounter() {
    return programCounter_;
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
OperationContextPimpl::returnAddress() {
    return returnAddress_;
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
OperationContextPimpl::setSaveReturnAddress(bool value) {
    saveReturnAddress_ = value;
}

/**
 * Returns true if RA should saved before executing next control flow 
 * operation.
 *
 * @return The return address value as a reference.
 */
bool
OperationContextPimpl::saveReturnAddress() {
    return saveReturnAddress_;
}

/**
 * Returns true if there are no operation state objects stored in the
 * context.
 *
 * @return True if there are no operation state objects stored in the
 * context.
 */
bool 
OperationContextPimpl::isEmpty() const {
    return stateRegistry_.size() == 0;
}

/**
 * Returns true if the context has memory model associated with it.
 * 
 * @return True if the context has memory model associated with it.
 */
bool
OperationContextPimpl::hasMemoryModel() const {
    return (memory_ != NULL);
}

/**
 * Returns the count of cycles simulated.
 *
 * Can be used to implement real time timer operations, etc. Uses
 * the simulator's cycle count, if available, otherwise uses internal
 * cycle counter that is incremented with the advanceClock();
 */
CycleCount
OperationContextPimpl::cycleCount() const {
    if (cycleCountVar_ != NULL) {
        return *cycleCountVar_;
    } else {
        return cycleCount_;
    }
}
