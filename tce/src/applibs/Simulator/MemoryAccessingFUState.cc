/**
 * @file MemoryAccessingFUState.cc
 *
 * Definition of MemoryAccessingFUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "MemoryAccessingFUState.hh"
#include "TargetMemory.hh"
#include "OperationContext.hh"
#include "Memory.hh"
#include "GlobalLock.hh"

/**
 * Constructor.
 *
 * @param memory Memory instance.
 * @param MAUSize Width of the minimum addressable unit.
 * @param lock Global lock signal.
 * @todo Do not hard code the MAU or NW sizes!
 */
MemoryAccessingFUState::MemoryAccessingFUState(
    Memory& memory,
    Word MAUSize,
    GlobalLock& lock) : 
    FUState(lock), memory_(new TargetMemory(memory, false, MAUSize)),
    operationContext_() {
       
    operationContext_.setMemory(memory_, 4);
}

/**
 * Destructor.
 */
MemoryAccessingFUState::~MemoryAccessingFUState() {
    delete memory_;
}

/**
 * Returns the operation context.
 *
 * This is basically a "template method" to allow differently initialized
 * OperationContext-classes in FUState subclasses.
 *
 * @return The operation context for the FU.
 */
OperationContext&
MemoryAccessingFUState::context() {
    return operationContext_;
}


