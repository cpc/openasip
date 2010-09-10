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
 * @file MachineStateBuilder.hh
 *
 * Declaration of MachineStateBuilder class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_MACHINE_STATE_BUILDER_HH
#define TTA_MACHINE_STATE_BUILDER_HH

#include <string>

#include "Exception.hh"
#include "OperationPool.hh"
#include "FUConflictDetectorIndex.hh"
#include "SimulatorConstants.hh"

class MachineState;
class MemorySystem;
class StateLocator;
class FUState;
class GlobalLock;
class SimValue;
class OperationExecutor;

namespace TTAMachine {
    class Machine;
    class BaseFUPort;
    class FunctionUnit;
    class HWOperation;
}

/**
 * Builds MachineState from Machine Object Model.
 */
class MachineStateBuilder {
public:
    MachineStateBuilder();
    virtual ~MachineStateBuilder();

    MachineState* build(
        const TTAMachine::Machine& machine, 
        MemorySystem& memSys)
        throw (IllegalMachine);

    MachineState* build(
        const TTAMachine::Machine& machine, 
        MemorySystem& memSys,
        FUConflictDetectorIndex& detectors,
        bool throwWhenConflict=true)
        throw (IllegalMachine);

    MachineState* build(
        const TTAMachine::Machine& machine,
        MemorySystem& memSys,
        StateLocator& locator) 
    throw (IllegalMachine);

private:
    /// Copying not allowed.
    MachineStateBuilder(const MachineStateBuilder&); 
    /// Assignment not allowed.
    MachineStateBuilder& operator=(const MachineStateBuilder&);

    MachineState* buildMachineState(
        const TTAMachine::Machine& machine,
        MemorySystem& memSys,
        StateLocator& locator)
        throw (IllegalMachine);

    void addPortToFU(
        MachineState* machineState, 
        TTAMachine::BaseFUPort* port, 
        FUState* state,
        TTAMachine::FunctionUnit* fu,
        StateLocator& locator);

    void addPortToFU(
        MachineState* machineState, 
        TTAMachine::BaseFUPort* port, 
        FUState* state,
        TTAMachine::FunctionUnit* fu,
        StateLocator& locator,
        SimValue& sharedRegister);

    void bindPortsToOperands(
        OperationExecutor& executor,
        TTAMachine::HWOperation& hwOperation,
        MachineState& machineState,
        FUState& state,
        TTAMachine::FunctionUnit& unit) 
        throw (IllegalMachine);

    void addVirtualOpcodeSettingPortsToFU(
        MachineState& machineState,
        FUState& state,
        TTAMachine::FunctionUnit& unit)
        throw (IllegalMachine);

    /// The FU resource conflict detectors. They are needed while building
    /// the machine state model.
    FUConflictDetectorIndex* detectors_;

    /// True in case the built model should throw an exception in case of
    /// a resource conflict.
    bool throwWhenConflict_;
};

#endif
