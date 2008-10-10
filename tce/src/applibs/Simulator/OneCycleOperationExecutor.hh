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
