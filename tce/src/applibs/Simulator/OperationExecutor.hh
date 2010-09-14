/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file OperationExecutor.hh
 *
 * Declaration of OperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_EXECUTOR_HH
#define TTA_OPERATION_EXECUTOR_HH

#include <vector>
#include "Exception.hh"

class SimValue;
class Operation;
class FUState;
class PortState;
class OperationContext;

/**
 * Executes operations in function units.
 */
class OperationExecutor {
public:
    OperationExecutor() : hasPendingOperations_(false), parent_(NULL) {}
    explicit OperationExecutor(FUState& parent);
    virtual ~OperationExecutor();

    FUState& parent() const;
    void setParent(FUState& parent) { parent_ = &parent; }

    void addBinding(int io, PortState& port)
        throw (IllegalRegistration);
    PortState& binding(int io) const
        throw (OutOfRange);

    virtual void startOperation(Operation& op) = 0;
    bool hasPendingOperations() const;
    virtual void advanceClock() = 0;
    virtual OperationExecutor* copy() = 0;
    virtual void setContext(OperationContext& context) = 0;

protected:
    /// PortStates that are bound to a certain input or output operand.
    std::vector<PortState*> bindings_;
    /// This is set to true if the OperationExecutor is not in 'idle'
    /// mode.
    bool hasPendingOperations_;
private:
    /// Assignment not allowed.
    OperationExecutor& operator=(const OperationExecutor&);

    /// Parent of the executor.
    FUState* parent_;
};

/// Maximum count of operation operands supported by the executors.
/// This is used to allocate some static arrays, etc.
#define EXECUTOR_MAX_OPERAND_COUNT 512

#include "OperationExecutor.icc"

#endif
