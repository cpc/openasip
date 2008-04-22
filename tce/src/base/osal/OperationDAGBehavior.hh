/**
 * @file OperationDAGBehavior.hh
 *
 * Declaration of OperationDAGBehavior class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */


#ifndef TTA_OPERATION_DAG_BEHAVIOR_HH
#define TTA_OPERATION_DAG_BEHAVIOR_HH

#include <vector>
#include <string>
#include <iostream>

#include "Exception.hh"
#include "OperationBehavior.hh"

class SimValue;
class OperationContext;
class OperationDAG;
class Operation;

/**
 * Implementation of OperationBehavior which uses OperationDAG for execution
 * of operation.
 */
class OperationDAGBehavior : public OperationBehavior {
public:
    OperationDAGBehavior(OperationDAG& dag, int operandCount);
    virtual ~OperationDAGBehavior();

    virtual bool simulateTrigger(
        SimValue** io, OperationContext& context) const;

    virtual bool lateResult(
        SimValue** io, OperationContext& context) const;

    virtual void createState(OperationContext& context) const;
    virtual void deleteState(OperationContext& context) const;

    virtual const char* stateName() const;

    virtual bool canBeSimulated() const;

private:
    struct SimulationStep {
        Operation* op;
        SimValue** params;
    };
  
    OperationDAG& dag_;

    /// Number of operands of this operation.
    int operandCount_;
  
    /// Table of parameters for simulate trigger.
    SimValue* ios_;
    
    // Script for simulation
    std::vector<SimulationStep> simulationSteps_;
    
    /// Contain list of pointers to delete in destructor
    std::vector<SimValue*> cleanUpTable_;

    /// For checking if there is cyclic dependency in DAG.
    mutable bool cycleFound_;
};

#endif
