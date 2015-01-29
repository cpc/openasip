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
 * @file FUState.cc
 *
 * Definition of FUState class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005,2010 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "FUState.hh"
#include "Operation.hh"
#include "OperationExecutor.hh"
#include "OneCycleOperationExecutor.hh"
#include "SimpleOperationExecutor.hh"
#include "InputPortState.hh"
#include "TriggeringInputPortState.hh"
#include "OpcodeSettingVirtualInputPortState.hh"
#include "PortState.hh"
#include "Application.hh"
#include "OperationContext.hh"
#include "OutputPortState.hh"
#include "OperationContext.hh"
#include "SimulatorToolbox.hh"
#include "OperationPool.hh"
#include "Application.hh"
#include "SequenceTools.hh"
#include "Conversion.hh"
#include "DetailedOperationSimulator.hh"
#include "MultiLatencyOperationExecutor.hh"

using std::vector;
using std::string;

//////////////////////////////////////////////////////////////////////////////
// FUState
//////////////////////////////////////////////////////////////////////////////

/**
 * Constructor (no explicitly given FU name).
 *
 * @param lock Global lock signal.
 */
FUState::FUState() : 
    ClockedState(), idle_(false), trigger_(false),
    nextOperation_(NULL), nextExecutor_(NULL), operationContext_(DEFAULT_FU_NAME), 
    activeExecutors_(0), detailedModel_(NULL) {
}

/**
 * Constructor (with explicitly given FU name).
 *
 * @param lock Global lock signal.
 */
FUState::FUState(const TCEString& name) : 
    ClockedState(), idle_(false), trigger_(false),
    nextOperation_(NULL), nextExecutor_(NULL), operationContext_(name), 
    activeExecutors_(0), detailedModel_(NULL) {
}

/**
 * Destructor.
 */
FUState::~FUState() {
    clearPorts();
    SequenceTools::deleteAllItems(execList_);
}

/**
 * Clears input and output ports.
 */
void
FUState::clearPorts() {
    inputPorts_.clear();
    outputPorts_.clear();
}

/**
 * Handles actions that take place in the end of the clock cycle.
 *
 * If operation is triggered, startOperation() of the executor of the
 * operation is called.
 */
void
FUState::endClock() {
    if (trigger_) {
        if (nextOperation_ == NULL) {
            throw Exception(
                __FILE__, __LINE__, __func__,
                "Tried to trigger FU without operation code.");
        }
        if (nextExecutor_ == NULL) {
            ExecutorContainer::iterator iter = executors_.find(nextOperation_);
            assert(iter != executors_.end());
            nextExecutor_ = (*iter).second;
        }
        // set this flag to false before actually handling the trigger
        // condition because startOperation() can cause a runtime error
        // exception
        trigger_ = false;
        nextExecutor_->startOperation(*nextOperation_);
        if (nextExecutor_->hasPendingOperations()) {
            ++activeExecutors_;
            idle_ = false;
        }
    }
}

/**
 * Handles actions that take place while clock cycle changes.
 *
 * advanceClock() of all active operation executors are called.
 */
void
FUState::advanceClock() {

    if (detailedModel_ != NULL)
        detailedModel_->simulateCycleStart();

    // in case there are no active operations and there are no states
    // which need clock advancing, the FU is in idle state
    if (activeExecutors_ == 0 && context().isEmpty()) {
        idle_ = true;
        return;
    } else if (activeExecutors_ == 1 && nextExecutor_ != NULL &&
               nextExecutor_->hasPendingOperations()) {
        // a special case for sequential simulation:
        // if there's only one active executor, and it's the one
        // triggered previously in endClock(), we can use 
        // nextExecutor_ and avoid traversing the whole list of executors
        nextExecutor_->advanceClock();
        if (nextExecutor_->hasPendingOperations()) {
            activeExecutors_ = 1;
            idle_ = false;
        } else {
            activeExecutors_ = 0;
            idle_ = true;
        }
        // advance clock of all operations with state
        /// @todo detect if there are *clocked* state objects
        if (!context().isEmpty()) {
            context().advanceClock();
        } 
        return;
    }


    activeExecutors_ = 0;
    for (size_t i = 0; i < execList_.size(); ++i) {
        OperationExecutor* opexec = execList_[i];
        if (opexec->hasPendingOperations()) {            
            opexec->advanceClock();
            // check if it's still active
            if (opexec->hasPendingOperations()) {
                ++activeExecutors_;
            } 
        }
    }
    // advance clock of all operations with state
    /// @todo detect if there are *clocked* state objects
    if (!context().isEmpty()) {
        context().advanceClock();
    } 

    idle_ = (activeExecutors_ == 0 && context().isEmpty());
}

/**
 * Adds new input port state.
 *
 * @param port New input port state.
 */
void
FUState::addInputPortState(PortState& port) {
    inputPorts_.push_back(&port);
}

/**
 * Adds new output port state.
 *
 * @param port New output port state.
 */
void
FUState::addOutputPortState(PortState& port) {
    outputPorts_.push_back(&port);
}

/**
 * Adds operation executor for a given operation.
 *
 * @param opExec Operation executor to be added.
 * @param op Operation which executor is added.
 */
void
FUState::addOperationExecutor(OperationExecutor& opExec, Operation& op) {

    op.createState(context());

    // do not use same operation executor even if the latency and operand
    // bindings are equal between the operations, because the executor
    // properties can be different (especially, one of the executors can
    // have conflict detection)
    OperationExecutor* newExecutor = opExec.copy();
    execList_.push_back(newExecutor);
    newExecutor->setContext(context());
    executors_[&op] = newExecutor;
}

/**
 * Replaces the operation executor model for an operation.
 *
 * Does not copy the operation executor but uses the given instance.
 * The method copies the I/O bindings and sets the parent FUState from 
 * the old model.
 *
 * @param op Operation of which executor is added.
 * @param newExecutor Operation executor to be set.
 */
void
FUState::replaceOperationExecutor(
    Operation& op, 
    OperationExecutor* newExecutor) {

    op.createState(context());
    newExecutor->setContext(context());

    OperationExecutor* oldExecutor = executors_[&op];
    newExecutor->setParent(oldExecutor->parent());

    /* Copy the operand-port bindings. */
    for (int ioId = 1; 
         ioId <= op.numberOfInputs() + op.numberOfOutputs(); ++ioId) {
        newExecutor->addBinding(ioId, oldExecutor->binding(ioId));
    }
    assert(sameBindings(*oldExecutor, *newExecutor, op));

    /* Replace the executor in the execution list. */
    for (std::size_t i = 0; i < execList_.size(); ++i) {
        if (execList_.at(i) == oldExecutor) {
            execList_[i] = newExecutor;
            break;
        }
    }
    executors_[&op] = newExecutor;
    delete oldExecutor;
}

/**
 * Sets a detailed operation simulation model for all operations in 
 * the FU.
 */
void
FUState::setOperationSimulator(
    Operation& op, 
    DetailedOperationSimulator& sim) {

    MultiLatencyOperationExecutor* oe = 
        dynamic_cast<MultiLatencyOperationExecutor*>(executor(op));  
    assert (oe != NULL && "Can only add details to a complex executor.");
    oe->setOperationSimulator(sim);
}

/**
 * Sets a detailed operation simulation model for all operations in the FU.
 */
void
FUState::setOperationSimulator(DetailedOperationSimulator& sim) {
    
    for (ExecutorContainer::iterator i = executors_.begin(); 
         i != executors_.end(); ++i) {
        Operation* op = (*i).first;
        setOperationSimulator(*op, sim);
    }
    detailedModel_ = &sim;
}


/**
 * Returns true if the two OperationExecutors have the same bindings.
 *
 * @param exec1 First OperationExecutor.
 * @param exec2 Second OperationExecutor.
 * @param op Operation to be executed with executors.
 * @return True if the two executors have same bindings.
 */
bool
FUState::sameBindings(
    OperationExecutor& exec1,
    OperationExecutor& exec2,
    Operation& op) {

    for (int i = 1; i <= op.numberOfInputs() + op.numberOfOutputs(); i++) {

        PortState* port1 = NULL;
        try {
            port1 = &exec1.binding(i);
        } catch (const OutOfRange& o) {
            return false;
        }

        PortState* port2 = NULL;
        try {
            port2 = &exec2.binding(i);
        } catch (const OutOfRange& o) {
            return false;
        }

        if (port1 != port2) {
            return false;
        }
    }
    return true;
}

/**
 * Returns OperationExecutor for the given operation.
 *
 * @param op The operation of the wanted OperationExecutor.
 */
OperationExecutor*
FUState::executor(Operation& op) {
    ExecutorContainer::iterator iter = executors_.find(&op);
    if (iter == executors_.end()) {
        return NULL;
    } else {
        return (*iter).second;
    }
}

/**
 * Returns the operation context.
 *
 * This is a "template method" to allow differently initialized
 * OperationContext-classes in FUState subclasses.
 *
 * @return The operation context for the FU.
 */
OperationContext&
FUState::context() {
    return operationContext_;
}

//////////////////////////////////////////////////////////////////////////////
// NullFUState
//////////////////////////////////////////////////////////////////////////////

NullFUState NullFUState::instance_;

/**
 * Returns the instance of NullFUState.
 *
 * @return Instance of NullFUState.
 */
NullFUState&
NullFUState::instance() {
    return instance_;
}

/**
 * Constructor.
 */
NullFUState::NullFUState() {
}

/**
 * Destructor.
 */
NullFUState::~NullFUState() {
}

/**
 * Aborts the program with error message.
 */
void
NullFUState::endClock() {
    Application::abortWithError("endClock()");
}

/**
 * Aborts the program with error message.
 */
void
NullFUState::advanceClock() {
    Application::abortWithError("advanceClock()");
}

/**
 * Aborts the program with error message.
 */
void
NullFUState::addInputPortState(PortState&) {
    Application::abortWithError("addInputPortState()");
}

/**
 * Aborts the program with error message.
 */
void
NullFUState::addOutputPortState(PortState&) {
    Application::abortWithError("addOutputPortState()");
}

/**
 * Aborts the program with error message.
 */
void
NullFUState::addOperationExecutor(OperationExecutor&, Operation&) {
    Application::abortWithError("addOperationExecutor()");
}

/**
 * Aborts the program with error message.
 *
 * @return Never returns.
 */
OperationExecutor*
NullFUState::executor(Operation&) {
    Application::abortWithError("executor()");
    return NULL;
}

