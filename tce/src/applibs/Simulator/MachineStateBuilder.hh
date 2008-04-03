/**
 * @file MachineStateBuilder.hh
 *
 * Declaration of MachineStateBuilder class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
