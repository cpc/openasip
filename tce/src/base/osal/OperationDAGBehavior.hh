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
