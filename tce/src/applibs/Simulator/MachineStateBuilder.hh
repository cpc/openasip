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
        MemorySystem& memSys,
        GlobalLock& lock)
        throw (IllegalMachine);

    MachineState* build(
        const TTAMachine::Machine& machine, 
        MemorySystem& memSys,
        GlobalLock& lock,
        FUConflictDetectorIndex& detectors,
        bool throwWhenConflict=true)
        throw (IllegalMachine);

    MachineState* build(
        const TTAMachine::Machine& machine,
        MemorySystem& memSys,
        StateLocator& locator,
        GlobalLock& lock) throw (IllegalMachine);

private:
    /// Copying not allowed.
    MachineStateBuilder(const MachineStateBuilder&); 
    /// Assignment not allowed.
    MachineStateBuilder& operator=(const MachineStateBuilder&);

    MachineState* buildMachineState(
        const TTAMachine::Machine& machine,
        MemorySystem& memSys,
        StateLocator& locator,
        GlobalLock& lock)
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
