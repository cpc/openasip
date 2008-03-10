/**
 * @file OpcodeSettingVirtualInputPortState.cc
 *
 * Definition of OpcodeSettingVirtualInputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#include "OpcodeSettingVirtualInputPortState.hh"
#include "TriggeringInputPortState.hh"
#include "Operation.hh"
#include "FUState.hh"
#include "SimValue.hh"
#include "Application.hh"

using std::string;

/**
 * Constructor.
 *
 * @param Operation Operation of the port.
 * @param parent Parent function unit.
 * @param real The real port in which this port belongs to.
 */
OpcodeSettingVirtualInputPortState::OpcodeSettingVirtualInputPortState(
    Operation& operation,
    FUState& parent,
    InputPortState& real) : 
    InputPortState(parent, real.value().width()), 
    operation_(operation), executor_(NULL),
    real_(real) {
}

/**
 * Constructor.
 *
 * @param Operation Operation of the port.
 * @param executor OperationExecutor to use to execute the operation when it's
 *                 triggered.
 * @param parent Parent function unit.
 * @param real The real port in which this port belongs to.
 */
OpcodeSettingVirtualInputPortState::OpcodeSettingVirtualInputPortState(
    Operation& operation,
    OperationExecutor& executor,
    FUState& parent,
    InputPortState& real) : 
    InputPortState(parent, real.value().width()), 
    operation_(operation), executor_(&executor),
    real_(real) {
}


/**
 * Destructor.
 */
OpcodeSettingVirtualInputPortState::~OpcodeSettingVirtualInputPortState() {
}

/**
 * Sets the value of the port and sets operation to function unit.
 */
void
OpcodeSettingVirtualInputPortState::setValue(const SimValue& value) {

    if (executor_ != NULL) {
        parent_->setOperation(operation_, *executor_);
    } else {
        parent_->setOperation(operation_);
    }
    parent_->setTriggered(); 
    real_.setValue(value);
}

/**
 * Returns the value of the parent port.
 *
 * @return The value of the parent port.
 */
const SimValue&
OpcodeSettingVirtualInputPortState::value() const {
    return real_.value();
}

/**
 * Returns the real port.
 *
 * @return The real port.
 */
InputPortState*
OpcodeSettingVirtualInputPortState::realPort() const {
    return &real_;
}
