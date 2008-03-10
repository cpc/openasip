/**
 * @file MemoryAccessingFUState.hh
 *
 * Declaration of MemoryAccessingFUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MEMORY_ACCESSING_FU_STATE_HH
#define TTA_MEMORY_ACCESSING_FU_STATE_HH

#include "FUState.hh"

class Memory;
class TargetMemory;
class GlobalLock;

/**
 * Models function unit that access memory.
 */
class MemoryAccessingFUState : public FUState {
public:
    MemoryAccessingFUState(
        Memory& memory, 
        Word MAUSize,
        GlobalLock& lock);
    virtual ~MemoryAccessingFUState();

protected:
    virtual OperationContext& context();

private:
    /// Copying not allowed.
    MemoryAccessingFUState(const MemoryAccessingFUState&);
    /// Assignment not allowed.
    MemoryAccessingFUState& operator=(const MemoryAccessingFUState&);

    /// Memory wrapper instance.
    TargetMemory* memory_;

    /// The operation context for this FU.
    OperationContext operationContext_;

};

#endif
