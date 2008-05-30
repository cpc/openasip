/**
 * @file OperationContext.hh
 *
 * Declaration of OperationContext class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_CONTEXT_HH
#define TTA_OPERATION_CONTEXT_HH

#include "BaseType.hh"

class OperationState;
class SimValue;
class Memory;
class OperationContextPimpl;


/**
 * OperationContext is used to store any implementation and context dependent 
 * data that may affect the result of an operation. 
 *
 * In typical client applications, operation implementations are related to 
 * a function unit.
 */
class OperationContext {
public:

    OperationContext();
    OperationContext(
        Memory* memory,
        InstructionAddress& programCounter,
        SimValue& returnAddress);
    
    OperationContext(const OperationContext&);
    
    virtual ~OperationContext();

    Memory& memory();
    void setMemory(Memory* memory);
    int contextId() const;

    InstructionAddress& programCounter();
    void setSaveReturnAddress(bool value);
    bool saveReturnAddress();
    SimValue& returnAddress();

    void advanceClock();
    bool isEmpty() const;
    bool hasMemoryModel() const;

    // These methods are only for internal use. Used by the macro definitions
    // of OSAL.hh.
    void registerState(OperationState* state);
    void unregisterState(const char* name);
    OperationState& state(const char* name) const;

private:
    /// Assignment not allowed
    OperationContext& operator=(const OperationContext&);
    /// Implementation in separate source file to speed up compiling.
    OperationContextPimpl* pimpl_;
};

#endif
