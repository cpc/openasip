/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file FUState.hh
 *
 * Declaration of FUState class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
class GlobalLock;

//////////////////////////////////////////////////////////////////////////////
// FUState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models the state of the function unit of TTA processor.
 */
class FUState : public ClockedState {
public:
    FUState(GlobalLock& lock);
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

    virtual OperationExecutor* executor(Operation& op);

    virtual OperationContext& context();

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

    /// Maps operations to executors.
    typedef std::map<Operation*, OperationExecutor*> ExecutorContainer;
    /// Contains all the different instances of executors.
    typedef std::vector<OperationExecutor*> ExecutorList;

    /// True if operation is triggered in current clock cycle.
    bool trigger_;
    /// The global lock to use.
    GlobalLock* lock_;
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
    NullFUState(GlobalLock& lock);
    /// Copying not allowed.
    NullFUState(const NullFUState&);
    /// Assignment not allowed.
    NullFUState& operator=(const NullFUState&);
    
    static GlobalLock lock_;
    /// Unique instance of NullFUState.
    static NullFUState instance_;
};

#endif
