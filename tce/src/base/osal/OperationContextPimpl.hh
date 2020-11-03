/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file OperationContextPimpl.hh
 *
 * Declaration of OperationContextPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef OPERATION_CONTEXT_PIMPL_HH
#define OPERATION_CONTEXT_PIMPL_HH

#include <string>
#include <map>

#include "BaseType.hh"
#include "TCEString.hh"

class OperationState;
class SimValue;
class Memory;
class OperationContext;

class OperationContextPimpl {
public:
    friend class OperationContext;
    
    ~OperationContextPimpl();

    /// Type of state registry.
    typedef std::map<std::string, OperationState*> StateRegistry;

    void setStateRegistry(StateRegistry& stateRegistry) 
        { stateRegistry_ = &stateRegistry; }
    void unsetStateRegistry() { stateRegistry_ = NULL; }
    StateRegistry& stateRegistry() { return *stateRegistry_; }

private:
    OperationContextPimpl(const TCEString& name);
    OperationContextPimpl(
        const TCEString& name,
        Memory* memory,
        InstructionAddress& programCounter,
        SimValue& returnAddress,
        int delayCycles);
    
    Memory& memory();
    void setMemory(Memory* memory);
    int contextId() const;
    CycleCount cycleCount() const;
    void setCycleCountVariable(CycleCount& cycleCount) {
        cycleCountVar_ = &cycleCount;
    }
    
    InstructionAddress& programCounter();
    InstructionAddress& irfBlockStart();
    void setUpdateProgramCounter(bool value);
    bool updateProgramCounter() const;
    void setSaveReturnAddress(bool value);
    bool saveReturnAddress();
    SimValue& returnAddress();

    void advanceClock(OperationContext&);
    bool isEmpty() const;
    bool hasMemoryModel() const;
    const TCEString& functionUnitName();

    int branchDelayCycles();

    // These methods are only for internal use. Used by the macro definitions
    // of OSAL.hh.
    void registerState(OperationState* state);
    void unregisterState(const char* name);
    OperationState& state(const char* name) const;
    
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
    StateRegistry* stateRegistry_;
    /// Should the return address be saved?
    bool saveReturnAddress_;
    /// Should program counter be updated?
    bool updateProgramCounter_;
    /// Number of times advanceClock() has been called since
    /// the creation.
    CycleCount cycleCount_;
    /// The external variable that contains the current simulation
    /// cycle count.
    CycleCount* cycleCountVar_;
    /// Name of the FU instance -- passed down from MachineStateBuilder
    const TCEString FUName_;
    /// Amount of delay cycles caused by pipeline
    int branchDelayCycles_;
};

#endif
