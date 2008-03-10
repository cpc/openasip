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
