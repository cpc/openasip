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
 * @file FunctionUnit.cc
 *
 * Implementation of class FunctionUnit.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "ExecutionPipeline.hh"
#include "Machine.hh"
#include "FUPort.hh"
#include "Guard.hh"
#include "AddressSpace.hh"
#include "ControlUnit.hh"
#include "PipelineElement.hh"
#include "MOMTextGenerator.hh"
#include "Application.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"
#include "ResourceVector.hh"
#include "ResourceVectorSet.hh"
#include "CIStringSet.hh"
#include "ObjectState.hh"

using std::string;
using boost::format;

namespace TTAMachine {

// initialization of static data members
const string FunctionUnit::OSNAME_FU = "fu";
const string FunctionUnit::OSKEY_AS = "as";
const string FunctionUnit::OSKEY_ORDER_NUMBER = "order_no";

/**
 * Constructor.
 *
 * @param name Name of the function unit.
 * @exception InvalidName If the given name is not a valid component name.
 */
FunctionUnit::FunctionUnit(const string& name)
    : Unit(name), addressSpace_(NULL), orderNumber_(0) {}

/**
 * Constructor.
 *
 * Loads the state of the function unit from the given ObjectState instance.
 * Does not load references to other components.
 *
 * @param state The ObjectState instance to load the state from.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid.
 */
FunctionUnit::FunctionUnit(const ObjectState* state)
    : Unit(state), addressSpace_(NULL), orderNumber_(0) {
    loadStateWithoutReferences(state);
}

/**
 * Destructor.
 *
 * Deletes all the operations.
 */
FunctionUnit::~FunctionUnit() {
    unsetMachine();
    deleteAllOperations();
}


/**
 * Copies the instance.
 *
 * Current FunctionUnit state is copied to a new FunctionUnit object.
 *
 * @return Copy of the instance.
 */
FunctionUnit*
FunctionUnit::copy() const {

    return new FunctionUnit(saveState());
}


/**
 * Sets the name of the function unit.
 *
 * @param name Name of the function unit.
 * @exception ComponentAlreadyExists If a function unit with the given name
 *                                   is already in the same machine.
 * @exception InvalidName If the given name is not a valid component name.
 */
void
FunctionUnit::setName(const string& name) {
    if (name == this->name()) {
        return;
    }

    if (machine() != NULL) {
        if (machine()->functionUnitNavigator().hasItem(name) ||
            (machine()->controlUnit() != NULL &&
             machine()->controlUnit()->name() == name)) {
            string procName = "FunctionUnit::setName";
            throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
        } else {
            Component::setName(name);
        }
    } else {
        Component::setName(name);
    }
}

/**
 * Returns the requested port.
 *
 * @param name Name of the port.
 * @return The requested port.
 * @exception InstanceNotFound If there is no port by the given name.
 */
BaseFUPort*
FunctionUnit::port(const std::string& name) const {
    if (!hasPort(name)) {
        string procName = "FunctionUnit::port";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }

    Port* port = Unit::port(name);
    BaseFUPort* fuPort = dynamic_cast<BaseFUPort*>(port);
    assert(fuPort != NULL);
    return fuPort;
}

/**
 * Returns port by the given index.
 *
 * The index must be between 0 and the return value of numberOfPorts() - 1.
 *
 * @param index Index.
 * @return The port found by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
BaseFUPort*
FunctionUnit::port(int index) const {
    // the out of range test is already done in Unit::Port,
    // no need to do it here also.
    return static_cast<BaseFUPort*>(Unit::port(index));
}

/**
 * Returns the number of port used for inputs and outputs of operations.
 *
 * Control unit may contain also ports for special registers. Those ports
 * are ignored by this method.
 *
 * @return The number of operation ports.
 */
int
FunctionUnit::operationPortCount() const {

    int portCount = this->portCount();
    int opCount = 0;

    for (int i = 0; i < portCount; i++) {
        Port* port = Unit::port(i);
        if (dynamic_cast<FUPort*>(port) != NULL) {
            opCount++;
        }
    }
    return opCount;
}


/**
 * Tells whether the function unit has an operation port with the given name.
 *
 * @param name Name of the port.
 * @return True if the function unit has the operation port, otherwise false.
 */
bool
FunctionUnit::hasOperationPort(const std::string& name) const {
    if (!hasPort(name)) {
        return false;
    } else {
        Port* port = this->port(name);
        return dynamic_cast<FUPort*>(port) != NULL;
    }
}


/**
 * Returns an operation port by the given name.
 *
 * Operation port is a port which can be read or written by an operation.
 *
 * @param name Name of the port.
 * @return The requested port.
 * @exception InstanceNotFound If the requested port does not exist.
 */
FUPort*
FunctionUnit::operationPort(const std::string& name) const {
    Port* port = Unit::port(name);
    FUPort* fuPort = dynamic_cast<FUPort*>(port);
    if (fuPort == NULL) {
        const string procName = "FunctionUnit::operationPort";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    }
    return fuPort;
}

/**
 * Returns an operation port by the given index.
 *
 * Operation port is a port which can be read or written by an operation.
 *
 * @param index The index.
 * @return The port found by the given index.
 * @exception OutOfRange If the given index is less than 0 or greater or
 *                       equal to the number of operation ports.
 */
FUPort*
FunctionUnit::operationPort(int index) const {
    int portCount = this->portCount();
    int current(-1);

    for (int i = 0; i < portCount; i++) {
        Port* port = Unit::port(i);
        if (dynamic_cast<FUPort*>(port) != NULL) {
            current++;
            if (current == index) {
                return dynamic_cast<FUPort*>(port);
            }
        }
    }

    const string procName = "FunctionUnit::operandPort";
    throw OutOfRange(__FILE__, __LINE__, procName);
}

/**
 * Returns triggering port if found. Otherwise returns NULL.
 */
BaseFUPort*
FunctionUnit::triggerPort() const {
    int portc = portCount();
    for (int i = 0; i < portc; i++) {
        if (port(i)->isTriggering()) {
            return port(i);
        }
    }
    return NULL;
}

/**
 * Adds an operation into the function unit. This method is called from
 * HWOperation constructor. Do not use this method.
 *
 * @param operation Operation which is added.
 * @exception ComponentAlreadyExists If there is already an operation by the
 *                                   same name as the given operation.
 */
void
FunctionUnit::addOperation(HWOperation& operation) {
    // run time check that this method is called from HWOperation constructor
    // only.
    assert(operation.parentUnit() == NULL);

    if (!FunctionUnit::hasOperation(operation.name())) {
        operations_.push_back(&operation);
    } else {
        string procName = "FunctionUnit::addOperation";
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    }
}

/**
 * Deletes an operation from the function unit.
 *
 * Destructor of the operation is called.
 *
 * @param operation Operation to be deleted.
 * @exception InstanceNotFound If the given operation doesn't belong to this
 *                             function unit.
 */
void
FunctionUnit::deleteOperation(HWOperation& operation) {
    if (operation.parentUnit() == NULL) {
        bool removed = ContainerTools::removeValueIfExists(operations_,
                                                           &operation);
        assert(removed);
    } else {
        if (!ContainerTools::containsValue(operations_, &operation)) {
            string procName = "FunctionUnit::deleteOperation";
            throw InstanceNotFound(__FILE__, __LINE__, procName);
        }
        delete &operation;
    }
}

/**
 * Returns true if the requested operation exists in the function unit.
 *
 * @param operation Name of the operation.
 * @return True if the requested operation exists in the function unit.
 */
bool
FunctionUnit::hasOperation(const std::string& name) const {
    return hasOperationLowercase(StringTools::stringToLower(name));
}

/**
 * Returns true if the requested operation exists in the function unit.
 *
 * @param operation Name of the operation which MUST be in lowercase.
 * @return True if the requested operation exists in the function unit.
 */
bool
FunctionUnit::hasOperationLowercase(const std::string& name) const {
    OperationTable::const_iterator iter = operations_.begin();
    while (iter != operations_.end()) {
        if ((*iter)->name() == name) {
            return true;
        }
        iter++;
    }
    return false;
}

/**
 * Returns operation by the given name.
 *
 * The requested operation must exist, otherwise assertion fails.
 *
 * @param name Name of the operation.
 * @return Operation by the given name.
 * @exception InstanceNotFound If an operation is not found by the given
 *                             name.
 */
HWOperation*
FunctionUnit::operation(const string& name) const {
    return operationLowercase(StringTools::stringToLower(name));
}

/**
 * Returns operation by the given name.
 *
 * The requested operation must exist, otherwise assertion fails.
 *
 * @param name Name of the operation.
 * @return Operation by the given name which MUST be in lowercase.
 * @exception InstanceNotFound If an operation is not found by the given
 *                             name.
 */
HWOperation*
FunctionUnit::operationLowercase(const string& name) const {
    OperationTable::const_iterator iter = operations_.begin();
    while (iter != operations_.end()) {
        if ((*iter)->name() == name) {
            return *iter;
        }
        iter++;
    }

    string procName = "FunctionUnit::operation";
    throw InstanceNotFound(
        __FILE__, __LINE__, procName,"Operation not found:" + name );
}

/**
 * Returns operation by the given index.
 *
 * The value of given index must be between 0 and the return value of
 * operationCount() - 1.
 *
 * @param index Index.
 * @return Operation by the given index.
 * @exception OutOfRange If the given index is less than zero or greater or
 *                       equal to the number of operations in the function
 *                       unit.
 */
HWOperation*
FunctionUnit::operation(int index) const {
    if (index < 0 || index >= operationCount()) {
        string procName = "FunctionUnit::operation";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }
    return operations_[index];
}

/**
 * Returns the number of operations in the function unit.
 *
 * @return The number of operations in the function unit.
 */
int
FunctionUnit::operationCount() const {
    return operations_.size();
}


/**
 * Adds FUs operations to the set given as a parameter.
 */
void
FunctionUnit::operationNames(TCETools::CIStringSet& opNames) const {
    OperationTable::const_iterator iter = operations_.begin();
    while (iter != operations_.end()) {
        opNames.insert((*iter)->name());
        ++iter;
    }
}


/**
 * Returns the maximum latency among the operations of the function unit.
 *
 * @return The maximum latency.
 */
int
FunctionUnit::maxLatency() const {
    int max(0);
    for (int i = 0; i < operationCount(); i++) {
        if (operation(i)->latency() > max) {
            max = operation(i)->latency();
        }
    }
    return max;
}


/**
 * Adds a pipeline element to the function unit.
 *
 * Pipeline elements are added automatically if some operation uses it.
 * Clients must not add pipeline elements explicitly. PipelineElement adds
 * itself automatically to the parent function when it is created.
 *
 * @param name Name of the pipeline element.
 * @exception ComponentAlreadyExists If there is already a pipeline element
 *                                   by the given name.
 */
void
FunctionUnit::addPipelineElement(PipelineElement& element) {
    // sanity check to verify that this is called from PipelineElement's
    // constructor
    assert(element.parentUnit() == NULL);

    string name = element.name();
    if (hasPipelineElement(name)) {
        string procName = "FunctionUnit::addPipelineElement";
        throw ComponentAlreadyExists(__FILE__, __LINE__, procName);
    } else {
        pipelineElements_.push_back(&element);
    }
}

/**
 * Deletes the given pipeline element from function unit.
 *
 * The pipeline element is automatically deleted if it is not used by any
 * of the operations. Clients must not use this method!
 *
 * @param element The pipeline element to delete.
 */
void
FunctionUnit::deletePipelineElement(PipelineElement& element) {

    // sanity check to verify that this is called from PipelineElement's
    // destructor.
    assert(element.parentUnit() == NULL);

    bool removed = ContainerTools::removeValueIfExists(
        pipelineElements_, &element);
    assert(removed);
}


/**
 * Returns the number of pipeline elements in the function unit.
 *
 * @return The number of pipeline elements.
 */
int
FunctionUnit::pipelineElementCount() const {
    return pipelineElements_.size();
}


/**
 * Returns a pipeline element by the given index.
 *
 * The index must be greater or equal to 0 and less than the number of
 * pipeline elements in the function unit.
 *
 * @param index The index.
 * @return A pipeline element by the given index.
 * @exception OutOfRange If the given index is out of range.
 */
PipelineElement*
FunctionUnit::pipelineElement(int index) const {
    if (index < 0 || index >= pipelineElementCount()) {
        string procName = "FunctionUnit::pipelineElement";
        throw OutOfRange(__FILE__, __LINE__, procName);
    }

    return pipelineElements_[index];
}

/**
 * Returns true if the function unit has a pipeline element by the given
 * name.
 *
 * @return True if the function unit has a pipeline element by the given
 *         name.
 */
bool
FunctionUnit::hasPipelineElement(const std::string& name) const {
    PipelineElementTable::const_iterator iter = pipelineElements_.begin();
    while (iter != pipelineElements_.end()) {
        if ((*iter)->name() == name) {
            return true;
        }
        iter++;
    }
    return false;
}


/**
 * Returns the pipeline element which has the given name.
 *
 * @param name Name of the pipeline element.
 * @return The pipeline element which has the given name, or NULL
 *         If there is no pipeline element by the given
 *                             name.
 */
PipelineElement*
FunctionUnit::pipelineElement(const std::string& name) const {

    PipelineElementTable::const_iterator iter = pipelineElements_.begin();
    while (iter != pipelineElements_.end()) {
        if ((*iter)->name() == name) {
            return *iter;
        }
        iter++;
    }
    return NULL;
}


/**
 * Returns the address space used by the function unit.
 *
 * @return The address space used by the function unit.
 */
AddressSpace*
FunctionUnit::addressSpace() const {
    return addressSpace_;
}


/**
 * Adds an address space which can be accessed by the function unit.
 *
 * @param as Address space which can be accessed.
 * @exception IllegalRegistration If the given address space and the function
 *                                unit are not registrered to the same
 *                                machine.
 */
void
FunctionUnit::setAddressSpace(AddressSpace* as) {
    if (as != NULL) {
        ensureRegistration(*as);
    }

    addressSpace_ = as;
}

/**
 * Returns true if the address space is set.
 *
 * @return True if the address space is set.
 */
bool
FunctionUnit::hasAddressSpace() const {

    if (addressSpace_ == NULL) {
        return false;
    }
    return true;
}


/**
 * Deletes a pipeline element by the given name if it is not used by any
 * operation.
 */
void
FunctionUnit::cleanup(const std::string& resource) {

    if (!hasPipelineElement(resource)) {
        return;
    }

    for (int i = 0; i < operationCount(); i++) {
        HWOperation* operation = this->operation(i);
        ExecutionPipeline* pLine = operation->pipeline();
        for (int cycle = 0; cycle < pLine->latency(); cycle++) {
            if (pLine->isResourceUsed(resource, cycle)) {
                return;
            }
        }
    }


    PipelineElement* toDelete = pipelineElement(resource);
    delete toDelete;
}


/**
 * Removes registration of the function unit from its current machine.
 */
void
FunctionUnit::unsetMachine() {

    if (machine() == NULL) {
        return;
    }

    cleanupGuards();
    Machine* mach = machine();
    unsetMachineDerived();
    mach->removeFunctionUnit(*this);
}


/**
 * Removes function unit part of a derived class from machine.
 */
void
FunctionUnit::unsetMachineDerived() {
    addressSpace_ = NULL;
    Unit::unsetMachine();
}


/**
 * Saves the contents to an ObjectState tree.
 *
 * @return The newly created ObjectState tree.
 */
ObjectState*
FunctionUnit::saveState() const {

    ObjectState* fuState = Unit::saveState();
    fuState->setName(OSNAME_FU);
    fuState->setAttribute(FunctionUnit::OSKEY_ORDER_NUMBER, orderNumber());
    // set address space
    if (addressSpace_ != NULL) {
        fuState->setAttribute(OSKEY_AS, addressSpace_->name());
    }

    // add operations
    for (int i = 0; i < operationCount(); i++) {
        HWOperation* operation = this->operation(i);
        fuState->addChild(operation->saveState());
    }

    return fuState;
}


/**
 * Loads its state from the given ObjectState instance.
 *
 * @param state The ObjectState instance.
 * @exception ObjectStateLoadingException If the given ObjectState instance
 *                                        is invalid or if connections to
 *                                        other machine parts cannot be made.
 */
void
FunctionUnit::loadState(const ObjectState* state) {
    const string procName = "FunctionUnit::loadState";

    Unit::loadState(state);
    loadStateWithoutReferences(state);

    try {
        // set address space
        if (state->hasAttribute(OSKEY_AS)) {
            MOMTextGenerator textGenerator;
            Machine::AddressSpaceNavigator asNav =
                machine()->addressSpaceNavigator();
            string asName = state->stringAttribute(OSKEY_AS);
            if (!isRegistered() || !asNav.hasItem(asName)) {
                format errorMsg = textGenerator.text(MOMTextGenerator::
                                                     TXT_FU_REF_LOAD_ERR_AS);
                errorMsg % asName % name();
                throw ObjectStateLoadingException(
                    __FILE__, __LINE__, procName, errorMsg.str());
            } else {
                setAddressSpace(asNav.item(asName));
            }
        }

    } catch (const Exception& e) {
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

/**
 * Compares two FunctionUnit architectures.
 *
 * Names are not compared. Port width comparison can be omitted.
 *
 * @param fu Function unit to compare with.
 * @param checkPortWidths Boolean for if port widths are matched or not.
 *                        Defaul is true.
 * @return True if the architectures match otherwise false.
 */
bool
FunctionUnit::isArchitectureEqual(
    const FunctionUnit* fu, const bool checkPortWidths) const {
    if (operationCount() != fu->operationCount()) {
        return false;
    }
    int fuPortCount = fu->operationPortCount();
    if (operationPortCount() != fuPortCount) {
        return false;
    }
    int fuPipelineElementCount = fu->pipelineElementCount();
    if (pipelineElementCount() != fuPipelineElementCount) {
        return false;
    }
    if (addressSpace() != NULL && fu->addressSpace() != NULL) {
        if (addressSpace()->name() != fu->addressSpace()->name()) {
            return false;
        }
    } else if ((addressSpace() == NULL && fu->addressSpace() != NULL) ||
               (addressSpace() != NULL && fu->addressSpace() == NULL)) {
        return false;
    }
    for (int i = 0; i < fuPipelineElementCount; i++) {
        string element = pipelineElement(i)->name();
        if (fu->hasPipelineElement(element)) {
            return false;
        }
    }
    for (int i = 0; i < fuPortCount; i++) {
        if (checkPortWidths) {
            if (!operationPort(i)->isArchitectureEqual(fu->operationPort(i))) {
                return false;
            }
        } else {
            if (operationPort(i)->isTriggering() !=
                fu->operationPort(i)->isTriggering()) {
                return false;
            }
           if (operationPort(i)->isOpcodeSetting() !=
                fu->operationPort(i)->isOpcodeSetting()) {
                return false;
            }
       }
    }
    if (!(ResourceVectorSet(*this) == ResourceVectorSet(*fu))) {
        return false;
    }
    return true;
}


/**
 * Checks if all the operations in the FU have the same latency and there are 
 * no shared resources.
 * 
 * @return True if the FU needs conflict detection
 */
bool 
FunctionUnit::needsConflictDetection() const {
        
    if (operationCount() == 0) {
        return false;
    }
    
    // same latency for all operations?
    const int latency = operation(0)->latency();
    for (int i = 1; i < operationCount(); ++i) {
        if (latency != operation(i)->latency()) {
            return true;
        }
    }
    
    if (pipelineElementCount() == 0) {
        return false;
    }
    
    return true;
}


/**
 * Cleans up the guards that refer to a port of this function unit.
 */
void
FunctionUnit::cleanupGuards() const {

    Machine* mach = machine();
    if (mach == NULL) {
        return;
    }

    Machine::BusNavigator navi = mach->busNavigator();
    for (int busIndex = 0; busIndex < navi.count(); busIndex++) {
        Bus* bus = navi.item(busIndex);
        int guardIndex = 0;
        while (guardIndex < bus->guardCount()) {
            Guard* guard = bus->guard(guardIndex);
            PortGuard* portGuard =
                dynamic_cast<PortGuard*>(guard);
            if (portGuard != NULL) {
                BaseFUPort* port = portGuard->port();
                FunctionUnit* referenced = port->parentUnit();
                if (referenced == this) {

                    // guard is removed from bus automatically
                    delete portGuard;
                } else {
                    guardIndex++;
                }
            } else {
                guardIndex++;
            }
        }
    }
}


/**
 * Loads the state of the function unit without references to other
 * components.
 *
 * @param fuState The ObjectState instance from which the state is loaded.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state.
 */
void
FunctionUnit::loadStateWithoutReferences(const ObjectState* fuState) {
    deleteAllOperations();
    addressSpace_ = NULL;

    if (fuState->hasAttribute(FunctionUnit::OSKEY_ORDER_NUMBER)) {
	    setOrderNumber(fuState->intAttribute(FunctionUnit::OSKEY_ORDER_NUMBER));
    }
    
    try {
        // load operations
        for (int i = 0; i < fuState->childCount(); i++) {
            ObjectState* child = fuState->child(i);
            if (child->name() == HWOperation::OSNAME_OPERATION) {
                new HWOperation(child, *this);
            }
        }
    } catch (const Exception& e) {
        const string procName = "FunctionUnit::loadStateWithoutReferences";
        throw ObjectStateLoadingException(
            __FILE__, __LINE__, procName, e.errorMessage());
    }
}

/**
 * Deletes all the operations of the function unit.
 */
void
FunctionUnit::deleteAllOperations() {
    while (operations_.size() > 0) {
        delete operations_[0];
    }
}

/**
 * Returns the order number of the FU.
 * 
 * In certain architectures (Cell SPU) the "relative order" of the function 
 * units matters when it comes to accessing same register by multiple operations
 * in the same cycle.   
 * This method returns number indicating possition of the FU in the ADF file. 
 * Alows putting the FU into the order in the instruction.
 */
int 
FunctionUnit::orderNumber() const {
    return orderNumber_;
}

/**
 * Sets order number of the FU.
 *
 * In certain architectures (Cell SPU) the "relative order" of the function 
 * units matters when it comes to accessing same register by multiple operations
 * in the same cycle.   
 * This method sets number indicating possition of the FU in the ADF file. 
 */
void
FunctionUnit::setOrderNumber(int number) {
    orderNumber_ = number;
}
}
