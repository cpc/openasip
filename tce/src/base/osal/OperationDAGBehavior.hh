/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OperationDAGBehavior.hh
 *
 * Declaration of OperationDAGBehavior class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto-no.spam-tut.fi)
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
