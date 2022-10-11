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
 * @file OpcodeSettingVirtualInputPortState.hh
 *
 * Declaration of OpcodeSettingVirtualInputPortState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi) 
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
