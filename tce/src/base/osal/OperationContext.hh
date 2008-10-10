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
 * @file OperationContext.hh
 *
 * Declaration of OperationContext class.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
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

    CycleCount cycleCount() const;
    void setCycleCountVariable(CycleCount& cycleCount);

private:
    /// Assignment not allowed
    OperationContext& operator=(const OperationContext&);
    /// Implementation in separate source file to speed up compiling.
    OperationContextPimpl* pimpl_;
};

#endif
