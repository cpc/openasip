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
 * @file OpcodeSettingVirtualInputPortState.cc
 *
 * Definition of OpcodeSettingVirtualInputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
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
