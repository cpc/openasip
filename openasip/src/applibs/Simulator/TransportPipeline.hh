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
 * @file TransportPipeline.hh
 *
 * Declaration of TransportPipeline class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_TRANSPORT_PIPELINE_HH
#define TTA_TRANSPORT_PIPELINE_HH

#include <string>

#include "OperationExecutor.hh"
#include "OperationContext.hh"

class FUState;
class GCUState;
class Operation;

/**
 * Models the transport pipeline of global control unit.
 */
class TransportPipeline : public OperationExecutor {
public:
    explicit TransportPipeline(GCUState& parent);
    virtual ~TransportPipeline();

    virtual int latency() const;
    virtual void startOperation(Operation& op);
    virtual void advanceClock();
    virtual OperationExecutor* copy();
    virtual void setContext(OperationContext& context);
    bool hasPendingOperations() const {
        return true;
    }

    TransportPipeline& operator=(const TransportPipeline&) = delete;

private:
    /// Operation to be triggered next.
    Operation* operation_;
    /// Operation context used to fetch the values for PC and RA.
    OperationContext* context_;
    /// Operation context seen by the operation.
    OperationContext tempContext_;
    /// Storage for PC value in the context.
    InstructionAddress PC_;
    /// Storage for IRF block start in the context.
    InstructionAddress IBS_;
    /// Storage for RA value in the context.
    SimValue RA_;
    /// Branch delay cycles
    int branchDelayCycles_;
    /// The owner GCUState.
    GCUState& parent_;

};

#endif
