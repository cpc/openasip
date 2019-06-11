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
 * @file MachineState.cc
 *
 * Definition of MachineState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "MachineState.hh"
#include "GCUState.hh"
#include "BusState.hh"
#include "FUState.hh"
#include "LongImmediateUnitState.hh"
#include "RegisterFileState.hh"
#include "MapTools.hh"
#include "OutputPortState.hh"
#include "OperationExecutor.hh"
#include "SequenceTools.hh"
#include "SimulatorToolbox.hh"
#include "Operation.hh"
#include "InputPortState.hh"
#include "OperationContext.hh"
#include "OperationPool.hh"
#include "TriggeringInputPortState.hh"
#include "OpcodeSettingVirtualInputPortState.hh"
#include "MapTools.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "GuardState.hh"

using std::string;

/**
 * Constructor.
 */
MachineState::MachineState() : GCUState_(NULL), fuStateCount_(0) {  
}

/**
 * Destructor.
 */
MachineState::~MachineState() {
    clear();
}

/**
 * Reserved memory is freed, containers are emptied.
 */
void
MachineState::clear() {
    delete GCUState_;
    GCUState_ = NULL;

    // Clear all caches
    busCache_.clear();
    fuCache_.clear();
    portCache_.clear();
    longImmediateCache_.clear();
    rfCache_.clear();
    guardCache_.clear();

    MapTools::deleteAllValues(busses_);
    MapTools::deleteAllValues(FUStates_);
    MapTools::deleteAllValues(ports_);
    MapTools::deleteAllValues(longImmediates_);
    MapTools::deleteAllValues(registers_);
    MapTools::deleteAllValues(guards_);
    SequenceTools::deleteAllItems(executors_);
}

/**
 * Returns the GCUState of the machine state.
 *
 * @return GCUState.
 */
GCUState&
MachineState::gcuState() {
    return *GCUState_;
}

/**
 * Returns bus state with a given name.
 *
 * If bus state with a given name is not found, return NullBusState.
 *
 * @param name Name of the bus state.
 * @return Bus state with a given name. 
 */
BusState&
MachineState::busState(const std::string& name) { 
    BusContainer::iterator iter = busses_.find(name);
    if (iter == busses_.end()) {
        return NullBusState::instance();
    }
    return *((*iter).second);
}

/**
 * Returns the FUState with a given name.
 *
 * If FUState is not found, returns NullFUState.
 *
 * @param name Name of the FUState.
 * @return FUState with a given name.
 */
FUState&
MachineState::fuState(const std::string& name) {
    FUContainer::iterator iter = FUStates_.find(name);
    if (iter == FUStates_.end()) {
        return NullFUState::instance();
    }
    return *((*iter).second);
}

/**
 * Returns the FUState with a given index.
 *
 * @param index The index of the FUState.
 * @return The FUState with a given index.
 * @exception OutOfRange If index is out of range.
 */
FUState&
MachineState::fuState(int index)
    throw (OutOfRange) {

    const int count = FUStateCount();
    if (index < 0 || (index > count - 1)) {
        string msg = "FUState index out of range";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    
    int i = 0;
    FUContainer::iterator iter = FUStates_.begin();
    while (i < index) { 
        iter++;
        ++i;
    }
    return *((*iter).second);
}

/**
 * Returns PortState with a given name.
 *
 * If PortState is not found, return NullPortState.
 *
 * @param portName The name of the PortState.
 * @param fuName The name of the parent FU of the port.
 * @return PortState with a given name.
 */
PortState&
MachineState::portState(
    const std::string& portName, 
    const std::string& fuName) {
    
    PortContainer::iterator i = 
        ports_.find(fuName + "." + StringTools::stringToLower(portName));
    if (i == ports_.end())
        return NullPortState::instance();
    else
        return *(*i).second;
}

/**
 * Returns LongImmediateUnitState with a given name.
 *
 * If LongImmediateUnitState is not found, returns NullLongImmediateUnistState.
 *
 * @param name The name of the LongImmediateUnitState.
 * @return LongImmediateUnitState with a given name.
 */
LongImmediateUnitState&
MachineState::longImmediateUnitState(const std::string& name) {
    LongImmediateContainer::iterator iter = longImmediates_.find(name);
    if (iter == longImmediates_.end()) {
        return NullLongImmediateUnitState::instance();
    }
    return *((*iter).second);
}

/**
 * Returns RegisterFileState with a given name.
 *
 * If RegisterFileState is not found, return NullRegisterFileState.
 *
 * @param name The name of the state.
 * @return RegisterFileState with a given name.
 */
RegisterFileState&
MachineState::registerFileState(const std::string& name) {
    RegisterFileContainer::iterator iter = registers_.find(name);
    if (iter == registers_.end()) {
        return NullRegisterFileState::instance();
    }
    return *((*iter).second);
}

/**
 * Returns the GuardState associated with the given MOM Guard.
 *
 * If not found, returns NullGuardState::instace().
 *
 * @param name The MOM Guard instance.
 * @return GuardState associated with the instance.
 */
GuardState&
MachineState::guardState(const TTAMachine::Guard& guard) {
    GuardContainer::iterator iter = guards_.find(&guard);
    if (iter == guards_.end()) {
        return NullGuardState::instance();
    }
    return *((*iter).second);
}

/**
 * Adds GCUState to machine state.
 *
 * @param state GCUState to be added.
 */
void
MachineState::addGCUState(GCUState* state) {
    GCUState_ = state;
}

/**
 * Adds BusState to machine state.
 *
 * @param state BusState to be added.
 * @param name The name of the BusState.
 */
void
MachineState::addBusState(BusState* state, const std::string& name) {
    busses_[name] = state;
    busCache_.push_back(state);
}

/**
 * Adds FUState.
 *
 * @param state FUState to be added.
 * @param name The name of the FU in ADF.
 */
void
MachineState::addFUState(FUState* state, const std::string& name) {
    FUStates_[name] = state;
    fuCache_.push_back(state);
}

/**
 * Adds PortState.
 *
 * @param state PortState to be added.
 * @param name Name of the port in ADF.
 * @param fuName Name of the FU of the port in ADF.
 */
void
MachineState::addPortState(
    PortState* state, 
    const std::string& name, 
    const std::string& fuName) {
    ports_[fuName + "." + StringTools::stringToLower(name)] = state;
    portCache_.push_back(state);
}

/**
 * Adds LongImmediateUnitState.
 *
 * @param state LongImmediateUnitState to be added.
 * @param name The name of the state.
 */
void
MachineState::addLongImmediateUnitState(
    LongImmediateUnitState* state,
    const std::string& name) {

    longImmediates_[name] = state;
    longImmediateCache_.push_back(state);
}

/**
 * Adds RegisterFileState.
 *
 * @param state State to be added.
 * @param name Name of the state.
 */
void 
MachineState::addRegisterFileState(
    RegisterFileState* state,
    const std::string& name) {

    registers_[name] = state;
    rfCache_.push_back(state);
}

/**
 * Adds GuardState.
 *
 * @param state State to be added.
 * @param guard The machine object model guard the state represents.
 */
void 
MachineState::addGuardState(
    GuardState* state,
    const TTAMachine::Guard& guard) {

    guards_[&guard] = state;
    guardCache_.push_back(state);
}

/**
 * Adds operation executor.
 *
 * @param executor Operation executor.
 */
void
MachineState::addOperationExecutor(OperationExecutor* executor) {
    executors_.push_back(executor);
}
