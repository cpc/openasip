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
#include "TCEString.hh"

class OperationState;
class SimValue;
class Memory;
class OperationContextPimpl;

#define DEFAULT_FU_NAME "unnamed_fu"

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
    OperationContext(const TCEString& name);
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
    const TCEString& functionUnitName();

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
