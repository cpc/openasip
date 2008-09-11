/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
    CycleCount cycleCount() const;
    void setCycleCountVariable(CycleCount& cycleCount) {
        cycleCountVar_ = &cycleCount;
    }
    
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
    /// Number of times advanceClock() has been called since
    /// the creation.
    CycleCount cycleCount_;
    /// The external variable that contains the current simulation
    /// cycle count.
    CycleCount* cycleCountVar_;
};

#endif
