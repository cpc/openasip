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
 * @file OpcodeSettingVirtualInputPortState.hh
 *
 * Declaration of OpcodeSettingVirtualInputPortState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi) 
 * @note rating: red
 */

#ifndef TTA_OPCODE_SETTING_VIRTUAL_INPUT_PORT_STATE_HH
#define TTA_OPCODE_SETTING_VIRTUAL_INPUT_PORT_STATE_HH

#include <string>

#include "InputPortState.hh"

class Operation;
class FUState;
class OperationExecutor;

/**
 * Operation setting input port state.
 *
 * Port is virtual, that is it doesn't model real input port state.
 * It has a pointer to the real port in which value is really set.
 */
class OpcodeSettingVirtualInputPortState : public InputPortState {
public:
    OpcodeSettingVirtualInputPortState(
        Operation& operation, 
        OperationExecutor& executor,
        FUState& parent, 
        InputPortState& real);

    OpcodeSettingVirtualInputPortState(
        Operation& operation, 
        FUState& parent, 
        InputPortState& real);

    virtual ~OpcodeSettingVirtualInputPortState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

    InputPortState* realPort() const;

private:
    /// Copying not allowed.
    OpcodeSettingVirtualInputPortState(
        const OpcodeSettingVirtualInputPortState&);
    /// Assignment not allowed.
    OpcodeSettingVirtualInputPortState& operator=(
        const OpcodeSettingVirtualInputPortState&);
    /// Operation of the port.
    Operation& operation_;
    /// Operation executor used to execute the operation in the target FU,
    /// this is an optimization to avoid searching for the executor every
    /// time the operation is triggered.
    OperationExecutor* executor_;
    /// Real input port.
    InputPortState& real_;
};

#endif
