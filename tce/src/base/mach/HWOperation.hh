/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file HWOperation.hh
 *
 * Declaration of HWOperation class.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_HW_OPERATION_HH
#define TTA_HW_OPERATION_HH

#include <string>
#include <map>

#include "MachinePart.hh"

class ObjectState;
class Operand;

namespace TTAMachine {

class FunctionUnit;
class ExecutionPipeline;
class FUPort;

/**
 * Represents an operation of the function unit.
 */
class HWOperation : public SubComponent {
public:
    HWOperation(const std::string& name, FunctionUnit& parent);
    HWOperation(const ObjectState* state, FunctionUnit& parent);
    ~HWOperation();

    const std::string& name() const;
    virtual void setName(const std::string& name);

    FunctionUnit* parentUnit() const;
    ExecutionPipeline* pipeline() const;

    int latency() const;
    int latency(int output) const;
    int slack(int input) const;

    virtual void bindPort(int operand, const FUPort& port);
    virtual void unbindPort(const FUPort& port);
    virtual FUPort* port(int operand) const;
    bool isBound(const FUPort& port) const;
    int io(const FUPort& port) const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

    /// ObjectState name for HWOperation.
    static const std::string OSNAME_OPERATION;
    /// ObjectState attribute key for name of the operation.
    static const std::string OSKEY_NAME;
    /// ObjectState name for an operand binding.
    static const std::string OSNAME_OPERAND_BINDING;
    /// ObjectState attribute key for operand index.
    static const std::string OSKEY_OPERAND;
    /// ObjectState attribute key for port name.
    static const std::string OSKEY_PORT;

private:
    /// Map for mapping operand indexes to FUPorts.
    typedef std::map<int, const FUPort*> OperandBindingMap;

    /// Name of the operation.
    std::string name_;
    /// Pipeline of the operation.
    ExecutionPipeline* pipeline_;
    /// The parent unit.
    FunctionUnit* parent_;
    /// Maps operands of operation to particular ports of the parent unit.
    OperandBindingMap operandBinding_;

};
}

#endif
