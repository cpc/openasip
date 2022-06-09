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
 * @file OneCycleOperationExecutor.hh
 *
 * Declaration of OneCycleOperationExecutor class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ONE_CYCLE_OPERATION_EXECUTOR_HH
#define TTA_ONE_CYCLE_OPERATION_EXECUTOR_HH

#include <string>

#include "OperationExecutor.hh"

class OperationContext;

/**
 * Operation executor for one cycle latency operations.
 */
class OneCycleOperationExecutor : public OperationExecutor {
public:
    explicit OneCycleOperationExecutor(FUState& parent);
    virtual ~OneCycleOperationExecutor();

    virtual int latency() const;
    virtual void startOperation(Operation& op);

    virtual void advanceClock();

    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);

private:
    /// Assignment not allowed.
    OneCycleOperationExecutor& operator=(const OneCycleOperationExecutor&);

    void initializeIOVector();

    /// Operation context.
    OperationContext* context_;
    /// Operation to be executed next.
    Operation* operation_;
    /// Inputs and outputs for the operation.
    SimValue** io_;   
    /// True if all the results are ready.
    bool ready_;
    /// True if the io_ vector is initialized-
    bool ioVectorInitialized_;
};

#endif
