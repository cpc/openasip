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
