/**
 * @file OperationContextPimpl.hh
 *
 * Declaration of OperationContextPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_CONTEXT_PIMPL_HH
#define OPERATION_CONTEXT_PIMPL_HH

#include <string>
#include <map>

#include "BaseType.hh"

class OperationState;
class SimValue;
class Memory;
class OperationContext;

class OperationContextPimpl {
public:
    friend class OperationContext;
    
    ~OperationContextPimpl();

private:
    OperationContextPimpl();
    OperationContextPimpl(
        Memory* memory,
        InstructionAddress& programCounter,
        SimValue& returnAddress);
    
    Memory& memory();
    void setMemory(Memory* memory);
    int contextId() const;

    InstructionAddress& programCounter();
    void setSaveReturnAddress(bool value);
    bool saveReturnAddress();
    SimValue& returnAddress();

    void advanceClock(OperationContext&);
    bool isEmpty() const;
    bool hasMemoryModel() const;

    // These methods are only for internal use. Used by the macro definitions
    // of OSAL.hh.
    void registerState(OperationState* state);
    void unregisterState(const char* name);
    OperationState& state(const char* name) const;
    
    /// Type of state registry.
    typedef std::map<std::string, OperationState*> StateRegistry;

    bool hasState(const char* name) const;
    void initializeContextId();

    /// The Memory model instance.
    Memory* memory_;

    /// Unique number that identifies a context instance.
    int contextId_;

    /// Context id for the next created context instance.
    static int nextContextId_;

    /// Simulates the program counter value. 
    InstructionAddress& programCounter_;

    /// Simulates the procedure return address. 
    SimValue& returnAddress_;    

    /// The state registry.
    StateRegistry stateRegistry_;

    /// Should the return address be saved?
    bool saveReturnAddress_;
};

#endif
