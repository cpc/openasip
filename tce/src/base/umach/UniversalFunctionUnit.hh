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
 * @file UniversalFunctionUnit.hh
 *
 * Declaration of UniversalFunctionUnit class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_UNIVERSAL_FUNCTION_UNIT_HH
#define TTA_UNIVERSAL_FUNCTION_UNIT_HH

#include "FunctionUnit.hh"

class Operation;
class OperationPool;
class SmartHWOperation;

namespace TTAMachine {
    class HWOperation;
}
/**
 * UniversalFunctionUnit class represents a function unit which has
 * all the operations of an operation pool. The operations are added
 * on demand, not at construction.
 */
class UniversalFunctionUnit : public TTAMachine::FunctionUnit {
public:
    UniversalFunctionUnit(const std::string& name, OperationPool& opPool);
    virtual ~UniversalFunctionUnit();

    virtual bool hasOperation(const std::string& name) const;
    virtual TTAMachine::HWOperation* operation(const std::string& name) const;
    virtual void addPipelineElement(TTAMachine::PipelineElement& element);
    virtual void loadState(const ObjectState* state);

    int portCountWithWidth(int width) const;
    TTAMachine::FUPort& portWithWidth(int index, int width) const;

    static bool is32BitOperation(const std::string& opName);

    /// Name of the 32 bit wide opcode setting port.
    static const std::string OC_SETTING_PORT_32;
    /// Name of the 64 bit wide opcode setting port.
    static const std::string OC_SETTING_PORT_64;

private:
    SmartHWOperation& addSupportedOperation(const Operation& operation);
    void ensureInputPorts(int width, int count);
    void ensureOutputPorts(int width, int count);

    /// Operation pool from which the operations are searched.
    OperationPool& opPool_;
    /// Table of names of 32 bit operations
    static const std::string OPERATIONS_OF_32_BITS[];


};

#endif
