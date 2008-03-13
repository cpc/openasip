/**
 * @file OperationContext.hh
 *
 * Declaration of OperationContext class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen@tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_CONTEXT_HH
#define TTA_OPERATION_CONTEXT_HH

#include <string>
#include <map>

#include "BaseType.hh"
#include "Exception.hh"
#include "OperationState.hh"
#include "SimValue.hh"
#include "Memory.hh"

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

    virtual ~OperationContext();

    Memory& memory();
    void setMemory(Memory* memory);
    int contextId() const;

    InstructionAddress& programCounter();
    void setSaveReturnAddress(bool value);
    bool saveReturnAddress();
    SimValue& returnAddress();

    void advanceClock();
    bool isAvailable() const;
    bool isEmpty() const;
    bool hasMemoryModel() const;

    // These methods are only for internal use. Used by the macro definitions
    // of OSAL.hh.
    void registerState(OperationState* state);
    void unregisterState(const std::string& name);
    OperationState& state(const std::string& name) const
	throw (KeyNotFound);

private:

    /// Type of state registry.
    typedef std::map<std::string, OperationState*> StateRegistry;

    bool hasState(const std::string& name) const;
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

#include "OperationContext.icc"

#endif
