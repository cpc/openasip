/*
    Copyright (c) 2002-2017 Tampere University of Technology.

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
 * @file FUState.hh
 *
 * Declaration of FUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005,2017 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_FU_STATE_HH
#define TTA_FU_STATE_HH

#include <vector>
#include <string>
#include <map>
#include <set>

#include "ClockedState.hh"
#include "PortState.hh"
#include "OperationContext.hh"

class Operation;
class OperationExecutor;
class OperationContext;
class DetailedOperationSimulator;

//////////////////////////////////////////////////////////////////////////////
// FUState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models the state of the function unit of a TTA processor.
 */
class FUState : public ClockedState {
public:
    FUState();
    FUState(const TCEString& name);
    virtual ~FUState();

    void setTriggered();
    void setOperation(Operation& operation);
    void setOperation(Operation& operation, OperationExecutor& executor);
    virtual bool isIdle();

    virtual void endClock();
    virtual void advanceClock();

    virtual void addInputPortState(PortState& port);
    virtual void addOutputPortState(PortState& port);

    virtual void addOperationExecutor(
        OperationExecutor& opExec,
        Operation& op);

    virtual void setOperationSimulator(DetailedOperationSimulator& sim);

    virtual void replaceOperationExecutor(
        Operation& op,
        OperationExecutor* newExecutor);

    virtual OperationExecutor* executor(Operation& op);

    virtual OperationContext& context();

    virtual void reset();

protected:
    /// The idle status of the FU. The derived classes should
    /// alway set this to true when possible to avoid unnecessary
    /// advanceClock() and endClock() calls.
    bool idle_;

private:
    /// Copying not allowed.
    FUState(const FUState&);
    /// Assignment not allowed.
    FUState& operator=(const FUState&);

    void clearPorts();
    bool sameBindings(
        OperationExecutor& exec1, 
        OperationExecutor& exec2,
        Operation& op);

    virtual void setOperationSimulator(
        Operation& op, 
        DetailedOperationSimulator& sim);

    /// Maps operations to executors.
    typedef std::map<Operation*, OperationExecutor*> ExecutorContainer;
    /// Contains all the different instances of executors.
    typedef std::vector<OperationExecutor*> ExecutorList;

    /// True if operation is triggered in current clock cycle.
    bool trigger_;
    /// Operation to be triggered next.
    Operation* nextOperation_;
    /// OperationExecutor to be used for the next operation (an optimization).
    OperationExecutor* nextExecutor_;
    /// The operation context for this FU.
    OperationContext operationContext_;
    /// All operation executors.
    ExecutorContainer executors_;
    /// All the different instances of OperationExecutors.
    ExecutorList execList_;
    /// Input ports of the function unit.
    std::vector<PortState*> inputPorts_;
    /// Output ports of the function unit.
    std::vector<PortState*> outputPorts_;
    /// Count of active executors (to allow returning instantly
    /// from advanceClock())
    std::size_t activeExecutors_;
    /// Optional detailed operation simulation model. Assume there's one
    /// such model per FU or none at all for now (could be possible to
    /// be one model per Operation).
    DetailedOperationSimulator* detailedModel_;
};

#include "FUState.icc"

//////////////////////////////////////////////////////////////////////////////
// NullFUState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models non-existing FUState.
 */
class NullFUState : public FUState {
public:
    static NullFUState& instance();

    virtual ~NullFUState();

    virtual void endClock();
    virtual void advanceClock();

    virtual void addInputPortState(PortState& port);
    virtual void addOutputPortState(PortState& port);

    virtual void addOperationExecutor(
        OperationExecutor& opExec,
        Operation& op);

    virtual OperationExecutor* executor(Operation& op);
    
private:
    NullFUState();
    /// Copying not allowed.
    NullFUState(const NullFUState&);
    /// Assignment not allowed.
    NullFUState& operator=(const NullFUState&);
    /// Unique instance of NullFUState.
    static NullFUState instance_;
};

#endif
