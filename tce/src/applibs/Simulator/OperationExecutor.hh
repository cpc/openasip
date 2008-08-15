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
 * @file OperationExecutor.hh
 *
 * Declaration of OperationExecutor class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel@cs.tut.fi)
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
    explicit OperationExecutor(FUState& parent);
    virtual ~OperationExecutor();

    FUState& parent() const;

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
