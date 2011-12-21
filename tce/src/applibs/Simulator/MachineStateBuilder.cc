/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file MachineStateBuilder.cc
 *
 * Definition of MachineStateBuilder class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @author Viljami Korhonen 2007 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include <typeinfo>
#include <string>

#include "MachineStateBuilder.hh"
#include "MachineState.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "FUState.hh"
#include "LongImmediateUnitState.hh"
#include "RegisterFileState.hh"
#include "FUPort.hh"
#include "OperationPool.hh"
#include "BusState.hh"
#include "TriggeringInputPortState.hh"
#include "InputPortState.hh"
#include "OpcodeSettingVirtualInputPortState.hh"
#include "HWOperation.hh"
#include "OutputPortState.hh"
#include "OperationContext.hh"
#include "OneCycleOperationExecutor.hh"
#include "SimpleOperationExecutor.hh"
#include "ConflictDetectingOperationExecutor.hh"
#include "GCUState.hh"
#include "ControlUnit.hh"
#include "Operation.hh"
#include "SimulatorToolbox.hh"
#include "Memory.hh"
#include "MemorySystem.hh"
#include "MemoryAccessingFUState.hh"
#include "StateLocator.hh"
#include "TransportPipeline.hh"
#include "StringTools.hh"
#include "SpecialRegisterPort.hh"
#include "SimValue.hh"
#include "PortState.hh"
#include "ExecutionPipeline.hh"
#include "GuardState.hh"
#include "Guard.hh"
#include "AddressSpaceCheck.hh"
#include "TCEString.hh"
#include "MachineCheckResults.hh"

using namespace TTAMachine;
using std::string;

/**
 * Constructor.
 */
MachineStateBuilder::MachineStateBuilder() : throwWhenConflict_(true) {
}

/**
 * Destructor.
 */
MachineStateBuilder::~MachineStateBuilder() {
}

/**
 * Builds machine state from given machine.
 *
 * @param machine The machine of which machine state is built from.
 * @param memSys Memory system instance.
 * @return The created machine state.
 * @exception IllegalMachine If machine state building fails.
 */
MachineState*
MachineStateBuilder::build(
    const Machine& machine, 
    MemorySystem& memSys) 
    throw (IllegalMachine) {

    StateLocator locator;
    detectors_ = NULL;
    return buildMachineState(machine, memSys, locator);
}

/**
 * Builds machine state from given machine.
 *
 * This method allows giving an index of FU resource conflict detectors to
 * create a model that detects FU resource conflicts.
 *
 * @param machine The machine of which machine state is built from.
 * @param memSys Memory system instance.
 * @param detectors The FU resource conflict detectors.
 * @param throwWhenConflict Builds a model that throws an exception in case of
 *        resource conflict is detected.
 * @return The created machine state.
 * @exception IllegalMachine If machine state building fails.
 */
MachineState*
MachineStateBuilder::build(
    const Machine& machine, 
    MemorySystem& memSys,
    FUConflictDetectorIndex& detectors,
    bool throwWhenConflict) 
    throw (IllegalMachine) {

    StateLocator locator;
    detectors_ = &detectors;
    throwWhenConflict_ = throwWhenConflict;
    return buildMachineState(machine, memSys, locator);
}

/**
 * Builds machine state from given machine.
 *
 * @param machine Machine of which machine state is build from.
 * @param memSys Memory system.
 * @param locator Indexes states with components as keys.
 * @return The created machine state instance.
 * @exception IllegalMachine If machine state building fails.
 */
MachineState*
MachineStateBuilder::build(
    const Machine& machine,
    MemorySystem& memSys,
    StateLocator& locator) 
    throw (IllegalMachine) {
    detectors_ = NULL;
    return buildMachineState(machine, memSys, locator);
}

/**
 * Builds MachineState from given Machine.
 *
 * @param machine Machine.
 * @param memSys Memory system.
 * @param locator State locator.
 * @return The created MachineState.
 * @exception IllegalMachine If machine state building fails.
 */
MachineState*
MachineStateBuilder::buildMachineState(
    const Machine& machine, 
    MemorySystem& memSys,
    StateLocator& locator)
    throw (IllegalMachine) {
    
    MachineState* machineState = new MachineState();

    int controlUnitGuardLatency = 0;
    if (machine.controlUnit() != NULL) {
        ControlUnit* controlUnit = machine.controlUnit();
        /// @todo This assumes that natural word width of GCU is 4 MAUs!
        GCUState* gcu = new GCUState(
            controlUnit->delaySlots() + 1, 4);
        machineState->addGCUState(gcu);
        controlUnitGuardLatency  = controlUnit->globalGuardLatency();

        for (int i = 0; i < controlUnit->portCount(); i++) {
            BaseFUPort* port = controlUnit->port(i);
            if (controlUnit->hasReturnAddressPort() &&
                dynamic_cast<SpecialRegisterPort*>(port) == 
                controlUnit->returnAddressPort()) {
                // return address port shares a register with the RA register
                // which resides inside the GCU
                addPortToFU(
                    machineState, port, gcu, controlUnit, locator, 
                    gcu->returnAddressRegister());
            } else {
                addPortToFU(machineState, port, gcu, controlUnit, locator);
            }
        }
        addVirtualOpcodeSettingPortsToFU(*machineState, *gcu, *controlUnit);
    }

    Machine::FunctionUnitNavigator FUs = machine.functionUnitNavigator();
    
    OperationPool& pool = SimulatorToolbox::operationPool();
    AddressSpaceCheck addressSpaceCheck(pool);
    MachineCheckResults results;
    if (!addressSpaceCheck.check(machine, results)) {
        string msg = "Problems while building machine state: " +
            results.error(0).second;
        throw IllegalMachine(__FILE__, __LINE__, __func__, msg) ;
    }
   
    for (int i = 0; i < FUs.count(); i++) {
        FunctionUnit* unit = FUs.item(i);
        FUState* state = NULL;
        if (unit->addressSpace() != NULL) {
            try {
                Memory& memory = memSys.memory(*unit->addressSpace());
                state = new MemoryAccessingFUState(memory);
            } catch (const Exception& i) {
                string msg = "Problems building machine state: " +
                    i.errorMessage();
                throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
            }
        } else {
            state = new FUState;
        }        
        machineState->addFUState(state, unit->name());
        
        for (int k = 0; k < unit->portCount(); k++) {            
            BaseFUPort* port = unit->port(k);
            addPortToFU(machineState, port, state, unit, locator);
        }    

        addVirtualOpcodeSettingPortsToFU(*machineState, *state, *unit);
    }
    
    Machine::ImmediateUnitNavigator immediateUnits = 
        machine.immediateUnitNavigator();
    for (int i = 0; i < immediateUnits.count(); i++) {
        LongImmediateUnitState* state = 
            new LongImmediateUnitState(
                immediateUnits.item(i)->numberOfRegisters(),
                immediateUnits.item(i)->latency(), 
                immediateUnits.item(i)->name(),
                immediateUnits.item(i)->width(),
                immediateUnits.item(i)->extensionMode() == 
                TTAMachine::Machine::SIGN);
                
        machineState->addLongImmediateUnitState(
            state, immediateUnits.item(i)->name());
    }

    Machine::RegisterFileNavigator registers = machine.registerFileNavigator();
    for (int i = 0; i < registers.count(); i++) {
        RegisterFile* regFile = registers.item(i);
        RegisterFileState* state = NULL;
        state = new RegisterFileState(
            regFile->numberOfRegisters(), regFile->width());
        machineState->addRegisterFileState(state, regFile->name());
    }

    Machine::BusNavigator busses = machine.busNavigator();
    for (int b = 0; b < busses.count(); ++b) {
        Bus* bus = busses.item(b);
        BusState* state = new BusState(bus->width());
        machineState->addBusState(state, bus->name());
        locator.addState(*bus, *state);

        for (int i = 0; i < bus->guardCount(); ++i) {
            const Guard* guard = bus->guard(i);
            assert(guard != NULL);

            const PortGuard* portGuard = 
                dynamic_cast<const PortGuard*>(guard);
            const RegisterGuard* registerGuard = 
                dynamic_cast<const RegisterGuard*>(guard);
            const UnconditionalGuard* uncondGuard = 
                dynamic_cast<const UnconditionalGuard*>(guard);

            int guardLatency = controlUnitGuardLatency;
            ReadableState* targetRegister = NULL;
            if (portGuard != NULL) {
        
                const BaseFUPort* thePort = 
                    dynamic_cast<const BaseFUPort*>(portGuard->port());
	
                assert(thePort != NULL);

                PortState& port = 
                    machineState->portState(
                        thePort->name(), thePort->parentUnit()->name());
	
                if (&port == &NullPortState::instance()) {
                    throw IllegalMachine(
                        __FILE__, __LINE__, __func__, 
                        SimulatorToolbox::textGenerator().text(
                            Texts::TXT_ILLEGAL_PROGRAM_PORT_STATE_NOT_FOUND).
                        str());
                }
	
                targetRegister = &port;
	
            } else if (registerGuard != NULL) {
	
                RegisterFileState& unit = 
                    machineState->registerFileState(
                        registerGuard->registerFile()->name());
	
                if (&unit == &NullRegisterFileState::instance()) {
                    throw IllegalMachine(
                        __FILE__, __LINE__, __func__, 
                        SimulatorToolbox::textGenerator().text(
                            Texts::TXT_ILLEGAL_PROGRAM_RF_STATE_NOT_FOUND
                            ).str());
                }
                guardLatency += registerGuard->registerFile()->guardLatency();
                
                targetRegister = 
                    &unit.registerState(registerGuard->registerIndex());
            } else if (uncondGuard != NULL) {
                // do not create a state object for unconditional guards,
                // it makes no sense
                continue;
            } else {
                abortWithError("Unknown guard type.");
            }   

            if (guardLatency == 1) {
                GuardState* guardState = 
                    new OneClockGuardState(*targetRegister);
                machineState->addGuardState(guardState, *guard);
            } else {
                GuardState* guardState = 
                    new GuardState(*targetRegister, guardLatency);
                machineState->addGuardState(guardState, *guard);
            }
        }
    }

    return machineState;
}

/**
 * Adds the virtual opcode-setting ports to the FU.
 *
 * Expects that the FU is otherwise completely built.
 *
 * @param machineState The machine state to use.
 * @param state The FUState to add the ports.
 * @param unit The static function unit data from MOM.
 */
void
MachineStateBuilder::addVirtualOpcodeSettingPortsToFU(
    MachineState& machineState,
    FUState& state,
    FunctionUnit& unit) 
    throw (IllegalMachine) {
    for (int i = 0; i < unit.portCount(); i++) {
        BaseFUPort& port = *unit.port(i);

        if (port.isOpcodeSetting()) {

            PortState* p = &machineState.portState(port.name(), unit.name());
            assert(p != &NullPortState::instance());

            TriggeringInputPortState* triggerPort = 
                dynamic_cast<TriggeringInputPortState*>(p);
            
            if (triggerPort == NULL) {
                throw IllegalMachine(
                    __FILE__, __LINE__, __func__,
                    std::string("The opcode setting port ") + port.name() + 
                    " is not triggering.");
            }
                
            // create a virtual port for all the opcodes that can be set
            // through the port
            for (int j = 0; j < unit.operationCount(); j++) {
                
                HWOperation& hwOp = *unit.operation(j);
                OperationPool& pool = SimulatorToolbox::operationPool();
                Operation* operation = NULL;
                try {
                    operation = &pool.operation(hwOp.name().c_str());
                } catch (const DynamicLibraryException& e) {
                    throw IllegalMachine(
                        __FILE__, __LINE__, __func__, e.errorMessage());
                }

                if (operation->isNull()) {
                    throw IllegalMachine(
                        __FILE__, __LINE__, __func__,
                        (boost::format(
                            "Operation '%s' not found in OSAL.") % 
                         hwOp.name()).str());
                }

                // add executor for the operation
                OperationExecutor* executor = NULL;
                GCUState* gcu = dynamic_cast<GCUState*>(&state);
                if (gcu != NULL) {
                    executor = new TransportPipeline(*gcu);
                } else {
                    if (hwOp.latency() == 0) {
                        throw IllegalMachine(
                            __FILE__, __LINE__, __func__,
                            std::string("Operation ") + hwOp.name() + " in " +
                            unit.name() + " has latency of 0, which is not " +
                            "supported by TCE.");                            
                    }

                    bool conflictDetection = false;
                    if (detectors_ != NULL && unit.needsConflictDetection()) {
                        FUConflictDetectorIndex::iterator detectorI =
                            detectors_->find(unit.name());

                        if (detectorI != detectors_->end()) {
                            conflictDetection = true;
                     
                            FUResourceConflictDetector& detector =
                                *(*detectorI).second;
                            // the FU has conflict detection enabled, always
                            // create a new ConflictDetectingOperationExecutor
                            // for all opcodes. 
                            executor = new ConflictDetectingOperationExecutor(
                                detector, detector.operationID(hwOp.name()),
                                state, hwOp, throwWhenConflict_);
                        }
                    }

                    if (!conflictDetection) {
                        bool multiLatencyOp = false;
                        const int totalLatency = hwOp.latency();
                        for (int i = operation->numberOfInputs() + 1;
                             i <= operation->numberOfInputs() + 
                                 operation->numberOfOutputs(); ++i) {
                            if (hwOp.latency(i) < totalLatency) {
                                multiLatencyOp = true;
                                break;
                            }                            
                        }

                        if (multiLatencyOp) {
                            executor = new MultiLatencyOperationExecutor(
                                state, hwOp); 
                        } else if (totalLatency == 1) {
                            executor = new OneCycleOperationExecutor(state);
                        } else {
                            executor = 
                                new SimpleOperationExecutor(
                                    totalLatency, state); 
                        }
                    }
                }

                bindPortsToOperands(
                    *executor, hwOp, machineState, state, unit);
                
                state.addOperationExecutor(*executor, *operation);
                machineState.addOperationExecutor(executor);

                // virtual ports are put to the index with names like
                // p1.add
                /// @todo Improve this and change the naming to alu.add.p1,
                /// or save the virtual ports to different index!
                string name = 
                    port.name() + "." + 
                    StringTools::stringToLower(operation->name());
                OperationExecutor* finalExecutor = 
                    state.executor(*operation);
                assert(executor != NULL);
                OpcodeSettingVirtualInputPortState* opcodePort =
                    new OpcodeSettingVirtualInputPortState(
                        *operation, *finalExecutor, state, *triggerPort);
                state.addInputPortState(*opcodePort);
                machineState.addPortState(
                    opcodePort, name, port.parentUnit()->name());
            }
        }
    }
}

/**
 * Adds a port that shares a register to FU.
 *
 * @param machineState Machine state which is built.
 * @param port Port to be added.
 * @param state FUState in which port is added.
 * @param fu FU of which FUState is build from.
 * @param locator StateLocator.
 * @param sharedRegister The register the FUPort shares, if any. If this is
 *                       set to NullSimValue then a new register (SimValue)
 *                       is created for the FUPort.
 */
void
MachineStateBuilder::addPortToFU(
    MachineState* machineState,
    BaseFUPort* port,
    FUState* state,
    FunctionUnit*,
    StateLocator& locator,
    SimValue& sharedRegister) {

    bool inputPort = false;
    // a shortcut for figuring whether the port shares a register (in practice,
    // connected to the return address register)
    const bool sharesRegister = (&sharedRegister != &NullSimValue::instance());

    // figure out the direction of the port
    if (port->inputSocket() != NULL) {
        inputPort = true;
    } else if (port->outputSocket() != NULL) {
        inputPort = false;
    } else {
        // Find out the direction of the port using operation<->port
        // connections. This way we can simulate also individual function 
        // units which are not connected to any socket. 
        const TTAMachine::FunctionUnit& parentFU = *port->parentUnit();

        // find out an operation that is connected to this port
        bool operationBindingFound = false;
        const TTAMachine::FUPort* fuPort = 
            dynamic_cast<TTAMachine::FUPort*>(port);
        for (int i = 0; fuPort != NULL && !operationBindingFound && 
                 i < parentFU.operationCount(); ++i) {
            const TTAMachine::HWOperation& operation = *parentFU.operation(i);
            if (!operation.isBound(*fuPort))
                continue;
            
            const TTAMachine::ExecutionPipeline* pipe = operation.pipeline();

            if (pipe == NULL)
                continue;
            // find out if there's a R/W pipeline component connected to
            // the port
            for (int cycle = 0; cycle < pipe->latency(); ++cycle) {
                if (pipe->isPortRead(*fuPort, cycle)) {
                    inputPort = true;
                    operationBindingFound = true;
                    break;
                } else if (pipe->isPortWritten(*fuPort, cycle)) {
                    inputPort = false;
                    operationBindingFound = true;
                    break;
                } else {
                    // no pipeline connection to this port at this cycle
                    continue; 
                }
            }            
        }   

        if (!operationBindingFound) {
            throw IllegalMachine(
                __FILE__, __LINE__, __func__,
                std::string("Cannot determine the direction of port ") +
                port->parentUnit()->name() + "." + port->name() + ".");
        }

    }

    if (inputPort) {
        if (port->isOpcodeSetting()) {
            
            TriggeringInputPortState* triggerPort = NULL;
            if (sharesRegister) {
                triggerPort = 
                    new TriggeringInputPortState(*state, sharedRegister);
            } else {
                triggerPort = 
                    new TriggeringInputPortState(*state, port->width());
            }

            state->addInputPortState(*triggerPort);
            machineState->addPortState(
                triggerPort, port->name(), port->parentUnit()->name());
            locator.addState(*port, *triggerPort);

        } else if (port->isTriggering()) {

            TriggeringInputPortState* input = NULL;
            if (sharesRegister) {
                input =  new TriggeringInputPortState(*state, sharedRegister);
            } else {
                input =  new TriggeringInputPortState(*state, port->width());
            }

            machineState->addPortState(
                input, port->name(), port->parentUnit()->name());
            state->addInputPortState(*input);
            locator.addState(*port, *input);
        } else {
            InputPortState* input = NULL;
            if (sharesRegister) {
                input =  new InputPortState(*state, sharedRegister);
            } else {
                input =  new InputPortState(*state, port->width());
            }
            machineState->addPortState(
                input, port->name(), port->parentUnit()->name());
            state->addInputPortState(*input);
            locator.addState(*port, *input);
        }
    } else {
        // port is an output port

        OutputPortState* output = NULL;
        if (sharesRegister) {
            output = new OutputPortState(*state, sharedRegister);
        } else {
            output = new OutputPortState(*state, port->width());
        }
        machineState->addPortState(
            output, port->name(), port->parentUnit()->name());
        state->addOutputPortState(*output);
        locator.addState(*port, *output);
    }
}

/**
 * Adds a port that does not share a register to FU.
 *
 * @param machineState Machine state which is built.
 * @param port Port to be added.
 * @param state FUState in which port is added.
 * @param fu FU of which FUState is build from.
 * @param locator StateLocator.
 */
void
MachineStateBuilder::addPortToFU(
    MachineState* machineState,
    BaseFUPort* port,
    FUState* state,
    FunctionUnit* unit,
    StateLocator& locator) {
    addPortToFU(
        machineState, port, state, unit, locator, NullSimValue::instance());
}

/**
 * Binds ports to operands.
 *
 * Operands are bound for OperationExecutor which is not yet inserted to 
 * the machine.
 *
 * @param executor OperationExecutor to bind the ports for.
 * @param machineState MachineState to be built.
 * @param state FUState of which ports are bound.
 * @param unit FunctionUnit that owns the ports that are bound.
 * @exception IllegalMachine If binding fails.
 */
void
MachineStateBuilder::bindPortsToOperands(
    OperationExecutor& executor,
    HWOperation& hwOperation,
    MachineState& machineState,
    FUState&,
    FunctionUnit& unit) throw (IllegalMachine) {
    
    OperationPool& pool = SimulatorToolbox::operationPool();
    Operation& op = pool.operation(hwOperation.name().c_str());

    if (&op == &NullOperation::instance()) {
        const string msg = 
            (SimulatorToolbox::textGenerator().text(
                Texts::TXT_OPERATION_X_NOT_FOUND) % hwOperation.name()).str();
        throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
    }
        
    const int operandCount = op.numberOfInputs() + op.numberOfOutputs();
    for (int j = 1; j <= operandCount; j++) {
            
        FUPort* port = hwOperation.port(j);
        if (port == NULL) {
            // unused operand
            continue;
        }
        PortState& portState =  
            machineState.portState(port->name(), unit.name());
        try {
            executor.addBinding(j, portState);
        } catch (const Exception& e) {
            string msg = "Problems while building machine state: " +
                e.errorMessage();
            throw IllegalMachine(__FILE__, __LINE__, __func__, msg);
        }
    }
}
