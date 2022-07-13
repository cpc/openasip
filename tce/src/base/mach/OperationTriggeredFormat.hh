/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file OperationTriggeredFormat.hh
 *
 * Declaration of OperationTriggeredFormat class.
 *
 * @author Kari Hepola 2022 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_TRIGGERED_FORMAT_HH
#define TTA_OPERATION_TRIGGERED_FORMAT_HH

#include "MachinePart.hh"
#include <vector>

namespace TTAMachine {

class Machine;
class OperationTriggeredOperand;

class OperationTriggeredFormat : public Component {
public:
    OperationTriggeredFormat(const std::string& name, Machine& owner);

    OperationTriggeredFormat(const ObjectState* state, Machine& owner);

    virtual ~OperationTriggeredFormat();

    void addOperation(const std::string& op);
    void removeOperation(const std::string& op);

    std::vector<std::string> operations() const;

    int operationCount() const;
    std::string operation(int index) const;
    bool hasOperation(const std::string& opName) const;


    void addOperand(OperationTriggeredOperand& operand);
    std::vector<OperationTriggeredOperand*> operands() const;

    virtual void setMachine(Machine& machine);
    virtual void unsetMachine();
    
    // methods inherited from Serializable interface
    virtual void loadState(const ObjectState* state);
    virtual ObjectState* saveState() const;

    static const std::string OSNAME_FORMAT;
    static const std::string OSKEY_OPERATION;


private:

    std::vector<std::string> operations_;
    std::vector<OperationTriggeredOperand*> operands_;

};
}

#endif
