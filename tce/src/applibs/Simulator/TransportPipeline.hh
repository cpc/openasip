/**
 * @file TransportPipeline.hh
 *
 * Declaration of TransportPipeline class.
 * 
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
   
private:
    /// Assignment not allowed.
    TransportPipeline& operator=(const TransportPipeline&);
    
    /// Operation to be triggered next.
    Operation* operation_;
    /// Operation context used to fetch the values for PC and RA.
    OperationContext* context_;
    /// Operation context seen by the operation.
    OperationContext tempContext_;
    /// Storage for PC value in the context.
    InstructionAddress PC_;
    /// Storage for RA value in the context.
    SimValue RA_;
    /// The owner GCUState.
    GCUState& parent_;

};

#endif
