/**
 * @file OneCycleOperationExecutor.hh
 *
 * Declaration of OneCycleOperationExecutor class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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
