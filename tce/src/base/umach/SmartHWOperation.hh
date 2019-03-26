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
 * @file SmartHWOperation.hh
 *
 * Declaration of SmartHWOperation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_SMART_HW_OPERATION_HH
#define TTA_SMART_HW_OPERATION_HH

#include "HWOperation.hh"

class Operation;
class UniversalFunctionUnit;

/**
 * Represents an operation in a function unit. The smart operation
 * automatically creates operand bindings on demand. This kind of
 * operations are used in UniversalFunctionUnit. The last operand
 * requested is bound to the operation code setting port.
 */
class SmartHWOperation : public TTAMachine::HWOperation {
public:
    SmartHWOperation(
        const Operation& operation,
        UniversalFunctionUnit& parent)
        throw (ComponentAlreadyExists, InvalidName);
    virtual ~SmartHWOperation();

    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);
    TTAMachine::FUPort* port(int operand) const;

    UniversalFunctionUnit* parentUnit() const;

    virtual void bindPort(int operand, const TTAMachine::FUPort& port)
        throw (IllegalRegistration, ComponentAlreadyExists, OutOfRange);
    virtual void unbindPort(const TTAMachine::FUPort& port);

    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    bool otherMandatoryInputsBound(int operand) const;

    /// The operation represented by this SmartHWOperation instance.
    const Operation& operation_;
    /// Tells whether this operation has 32 bits wide operands.
    bool is32BitOperation_;
};

#endif
