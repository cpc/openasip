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
 * @file ExecutionPipelineResource.cc
 *
 * Implementation of prototype of Resource Model:
 * implementation of the ExecutionPipelineResource.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @author Heikki Kultala 2009 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

//#define NO_OVERCOMMIT
 
#include <climits>

#include "ExecutionPipelineResource.hh"
#include "ExecutionPipelineResourceTable.hh"
#include "MapTools.hh"
#include "Move.hh"
#include "Operation.hh"
#include "Application.hh"
#include "Exception.hh"
#include "ProgramOperation.hh"
#include "MapTools.hh"
#include "MoveNode.hh"
#include "POMDisassembler.hh"
#include "OutputPSocketResource.hh"
#include "InputPSocketResource.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "HWOperation.hh"
#include "FUPort.hh"
#include "TCEString.hh"
#include "MoveGuard.hh"
#include "Guard.hh"
#include "DataDependenceGraph.hh"
#include "HWOperation.hh"
#include "FUPort.hh"

#include <fstream>
#include <sstream>
#include "ResourceManager.hh"
#include "MoveNode.hh"
#include "Terminal.hh"

/**
 * Constructor.
 *
 * Creates new resource with defined name
 *
 * @param name Name of resource
 * @param resNum Number of resources in FU
 * @param maxLatency Latency of longest operation FU supports
 */
ExecutionPipelineResource::ExecutionPipelineResource(
    const TTAMachine::FunctionUnit& fu,
    const unsigned int ii) :
    SchedulingResource("ep_" + fu.name(), ii), 
    resources(&ExecutionPipelineResourceTable::resourceTable(fu)),
    cachedSize_(INT_MIN), ddg_(NULL), fu_(fu) {
}

/**
 * Empty destructor
 */
ExecutionPipelineResource::~ExecutionPipelineResource() {}

/**
 * Not to be used. ExecutionPipelineResource needs to be
 * tested also with PSocket parameter to find if the desired
 * part of MoveNode is source or destination from type of PSocket.
 */
bool
ExecutionPipelineResource::canAssign(const int, const MoveNode&) const {
    abortWithError("Wrong use of canAssign, use also third parameter!");
    return false;
}

/**
 * Test if resource ExecutionPipelineResource is used in given cycle.
 *
 * If there is any of pipeline resources already used in given cycle.
 * @param cycle Cycle which to test
 * @return True if ExecutionPipelineResource is already used in cycle
 * @throw Internal error, the recorded resource usage for cycle is shorter
 * then the number of resources the FU has.
 */
bool
ExecutionPipelineResource::isInUse(const int cycle) const
    throw (Exception) {

    int modCycle = instructionIndex(cycle);

    int progOpCount = operandsWriten_.size();
    if (modCycle < progOpCount && operandsWriten_[modCycle].first != NULL) {
        /// Some operand is already written in tested cycle
        return true;
    }
    for (ResultMap::const_iterator rri = resultRead_.begin(); 
         rri != resultRead_.end(); rri++) {
        const ResultVector& resultRead = rri->second;
        int resultReadCount = resultRead.size();
        if (modCycle <  resultReadCount && 
            resultRead[modCycle].first.po != NULL) {
            /// Some result is already read in tested cycle
            return true;
        }
    }
    if (modCycle >= size()) {
        /// Cycle is beyond already scheduled scope, not in use therefore
        return false;
    }
    if (resources->numberOfResources() !=
            fuExecutionPipeline_.ref(modCycle).size()) {
        std::string msg = "Execution pipeline is missing resources!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    for (unsigned int i = 0; i < resources->numberOfResources(); i++) {
            if (fuExecutionPipeline_.ref(modCycle).size() == 0)
                return false;
                
            ResourceReservation& rr = 
                fuExecutionPipeline_.ref(modCycle)[i];

        if (rr.first != NULL) {
            /// Some pipeline resource is already in use in tested cycle
            return true;
        }
    }
    return false;
}

/**
 * Test if resource ExecutionPipelineResource is available for any of
 * the supported operations (at least one).
 *
 * @param cycle Cycle which to test
 * @return True if ExecutionPipelineResource is available in cycle
 */
bool
ExecutionPipelineResource::isAvailable(const int cycle) const {
    int progOpCount = operandsWriten_.size();

    int modCycle = instructionIndex(cycle);

    if (modCycle >= size()) {
        // Whole new operation will be in part of scope that is not
        // assigned to FU till now, so it is implicitely free
        if (modCycle >= progOpCount) {
            return true;
        }
    }
    if (modCycle < progOpCount) {
        // Cycle is in range between 2 moves of same operation
        // we can not overwrite it!
        ProgramOperation *po = operandsWriten_[modCycle].first;
        if (po != NULL) {
            // but if trigger conditional, may overwrite with 
            // operand with opposite guarded move.
            if (po->triggeringMove() != NULL && 
                po->triggeringMove()->move().isUnconditional()) {
                return false;
            }
        }
    }
    bool foundConflict = false;
    for (unsigned int pI = 0; pI < resources->pipelineSize(); pI++) {
        foundConflict = false;
        for (unsigned int i = 0; i < resources->maximalLatency(); i++) {
            if (int(instructionIndex(cycle + i)) >= size()) {
                if (!foundConflict) {
                    return true;
                }
            }
            for (unsigned int j = 0 ; j < resources->numberOfResources(); 
                 j++) {
                if (fuExecutionPipeline_.ref(instructionIndex(cycle+i)).size()
                    == 0)
                    continue;
                
                ResourceReservation& rr = 
                    fuExecutionPipeline_.ref(instructionIndex(cycle+i))[j];
                if (resources->operationPipeline(pI, i, j) &&
                    (rr.first != NULL && rr.second != NULL)) {
                    foundConflict = true;
                    // One of supported operations is conflicting, still
                    // needs to test other operations.
                    break;
                }
            }
        }
        if (!foundConflict) {
            // Last tested operation was not conflicting
            return true;
        }
    }
    if (!foundConflict) {
        return true;
    }
    return false;
}


/*
 * Record PO in cycle where result is produced into output register,
 *
 * increase number of results produced if same PO already producing
 * something in that cycle
 */
void 
ExecutionPipelineResource::setResultWriten(
    const TTAMachine::Port& port, unsigned int realCycle, 
    const ProgramOperation& po) {

    ResultVector& resultWriten = resultWriten_[&port];

    unsigned int modCycle = instructionIndex(realCycle);
    unsigned int resCount = resultWriten.size();

    // create bookkeepping if does not exist
    if (resCount <= modCycle) {
        resultWriten.resize(modCycle+1);
        resultWriten[modCycle] = 
            ResultHelperPair(
                ResultHelper(modCycle, NULL,0),
                ResultHelper(modCycle, NULL,0));
        
    }
#if 0    
    for (unsigned int i = resCount; i <= modCycle; i++) {
        // Increase size of the vector
        resultWriten.push_back(
            ResultHelperPair(
                ResultHelper(i, NULL,0),
                ResultHelper(i, NULL,0)));
    }
#endif
    if (resultWriten.ref(modCycle).first.po == NULL) {
        resultWriten.ref(modCycle).first =
            ResultHelper(realCycle, &po, 1);
    } else {
        if (resultWriten.ref(modCycle).first.realCycle == realCycle &&
            // add result to existing.
            resultWriten.ref(modCycle).first.po == &po) {
            resultWriten.ref(modCycle).first.count++;
        } else {
            if (resultWriten.ref(modCycle).second.po == NULL) {
                // another op? (with opposite guard?
                resultWriten.ref(modCycle).second =
                    ResultHelper(realCycle, &po, 1);
            } else {
                resultWriten.ref(modCycle).second.count++;
            }
        }
    }
}

void 
ExecutionPipelineResource::setResultWriten(
    const ProgramOperation& po, unsigned int triggerCycle) {

    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    // Loops over all output values produced by this 
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        int outIndex = op.numberOfInputs() + 1 + i;
        const TTAMachine::Port& port = *hwop.port(outIndex);
        int resultCycle = triggerCycle + hwop.latency(outIndex);
        
        setResultWriten(port, resultCycle, po);
    }
}

/*
 * Record PO in cycle where result is produced into output register,
 *
 * increase number of results produced if same PO already producing
 * something in that cycle
 */
void 
ExecutionPipelineResource::unsetResultWriten(
    const TTAMachine::Port& port, unsigned int realCycle, 
    const ProgramOperation& po) {
    unsigned int rrMod = instructionIndex(realCycle);

    ResultVector& resultWriten = resultWriten_[&port];
    ResultHelperPair& rhp = resultWriten.ref(rrMod);
    
    if (rhp.first.po == &po) {
        // Decrease counter of results written in
        rhp.first.count--;
        if (rhp.first.count == 0) {
            // move second to first.
            rhp.first = rhp.second;
            rhp.second = ResultHelper(); // empty it.
        }
    } else {
        assert(rhp.second.po == &po);
        // Decrease counter of results written in
        rhp.second.count--;
        if (rhp.second.count == 0) {
            rhp.second = ResultHelper(); // empty it.
        }
    }
}

void 
ExecutionPipelineResource::unsetResultWriten(
    const ProgramOperation& po, unsigned int triggerCycle) {

    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    // Loops over all output values produced by this 
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        int outIndex = op.numberOfInputs() + 1 + i;
        const TTAMachine::Port& port = *hwop.port(outIndex);
        int resultCycle = triggerCycle + hwop.latency(outIndex);
        
        unsetResultWriten(port, resultCycle, po);
    }
}


void
ExecutionPipelineResource::assign(const int, MoveNode&)
    throw (Exception) {
    abortWithError("Execution Pipeline Resource needs 3 arguments assign");
}

/**
 * Assign resource to given node for given cycle.
 *
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 * @param source Indicates if we want to unassing source part of move
 * in case move is bypassed
 */
void
ExecutionPipelineResource::assignSource(
    const int cycle,
    MoveNode& node) {
    unsigned int modCycle = instructionIndex(cycle);
    cachedSize_ = INT_MIN;

    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }

    assert(node.isSourceOperation());

    const TTAMachine::Port& port = resultPort(node);
    ResultVector& resultRead = resultRead_[&port];

    /// Assiging result read
    assignedSourceNodes_.insert(&node);
    ProgramOperation* pOp = &node.sourceOperation();
    
    /// Record Program Operation in cycle where the "result read"
    /// is scheduled
    unsigned int readCount = resultRead.size();
    if (readCount <= modCycle) {
        resultRead.resize(modCycle + 1);
        resultRead[modCycle] =
            ResultHelperPair(
                 ResultHelper(modCycle, NULL, 0),
                 ResultHelper(modCycle, NULL, 0));
#if 0        
        for (unsigned int i = readCount; i <= modCycle; i++) {
            // Increase the size of the vector
            resultRead.push_back(
                ResultHelperPair(
                    ResultHelper(i, NULL, 0),
                    ResultHelper(i, NULL, 0)));
        }
#endif        
    }
    // Record PO in cycle where result is read from output,
    // increase number of results read if same PO already reads something
    // in that cycle
    ResultHelperPair& rhp = resultRead.ref(modCycle);
    if (rhp.first.po == NULL) {
        rhp.first = ResultHelper(cycle, pOp, 1);
    } else {
        if (rhp.first.po == pOp) {
            rhp.first.count++;
        } else {
            if (rhp.second.po == NULL) {
                rhp.second = ResultHelper(cycle, pOp, 1);
            } else {
                if (rhp.second.po == pOp) {
                    rhp.second.count++;
                } else {
                    assert(0 && "result read of invalid op");
                }
            }
        }
    }
    
    // Record PO in cycle where result is available in result register
    // This should be always same or earlier then result read
    unsigned int resultReady = node.earliestResultReadCycle();
    
    if (resultReady == INT_MAX) {
        /// No trigger move assigned so far (bottom up assignment?)
        resultReady = cycle;
    }
    
    setResultWriten(port, resultReady, node.sourceOperation());
    
    // Record move and cycle in which the result of it is produced
    // This uses real cycles, not modulo cycles
    storedResultCycles_.insert(
        std::pair<MoveNode*, int>(&node,resultReady));
}


/**
 * Assign resource to given node for given cycle.
 *
 * @param cycle Cycle to assign
 * @param node MoveNode assigned
 * in case move is bypassed
 */
void
ExecutionPipelineResource::assignDestination(
    const int cycle,
    MoveNode& node) {
    unsigned int modCycle = instructionIndex(cycle);
    cachedSize_ = INT_MIN;

    if (!node.isDestinationOperation()) {
        return;
    }
    std::string opName = "";

    // If move is opcode setting, we set operation based on opcode.
    assignedDestinationNodes_.insert(&node);
    ProgramOperation& pOp = node.destinationOperation();

    int firstCycle = 0;
    int lastCycle = 0;
    int triggering = 0;
    unsigned int progOpCount = operandsWriten_.size();
    // If other operands of same PO were already written, returns
    // the range of cycles from first operand to last operand

    findRange(cycle, node, firstCycle, lastCycle, triggering);
    int modLastCycle = instructionIndex(lastCycle);
    if (progOpCount < modCycle) {
        operandsWriten_.resize(modCycle);
    }
    if (unsigned(modLastCycle) >= operandsWriten_.size()) {
        operandsWriten_.resize(
            modLastCycle + 1);
    }
    if (modLastCycle != lastCycle) {
        operandsWriten_.resize(initiationInterval_);
    }

    for (int i = firstCycle; i <= lastCycle; i++) {
        /// Test some other PO does not write operands in a requested range
        unsigned int modi = instructionIndex(i); 
        assert(operandsWriten_.size() >= modi);
        ProgramOperation* oldPO = operandsWriten_.ref(modi).first;
        if (oldPO != &pOp && oldPO != NULL) {
            MoveNode* oldTrigger = oldPO->triggeringMove();
            
            if (oldTrigger != NULL && !exclusiveMoves(oldTrigger, &node, i)) {
                std::string msg =  name() + " had previous operation ";
                msg += operandsWriten_.ref(modi).first->toString() + " in cycle " ;
                msg += Conversion::toString(modi);
                msg += " other trigger: ";
                msg += oldTrigger->toString();
                msg += "Node: " + node.toString();
                msg += "\n";

                if (node.isDestinationOperation()) {
                    msg += node.destinationOperation().toString();
                }
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            } else {

                // Marks all the cycles in range with PO
                // which is writing operands
                operandsWriten_.ref(modi).second = &pOp;
            } 
        } else {
            // Marks all the cycles in range with PO that is writing operands
            operandsWriten_.ref(modi).first = &pOp;
        }
    }

    if (!node.move().destination().isTriggering()) {
        /// New destination of FU is set before this method is called
        /// so we can trust that the port knowns if it is triggering
        return;
    }
    if (node.move().destination().isOpcodeSetting()) {
        opName = node.move().destination().operation().name();
    } else {
        std::string msg = "Using non opcodeSetting triggering move. ";
        msg += " Move: " + node.toString();
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    int pIndex = resources->operationIndex(opName);

    unsigned int fuEpSize = fuExecutionPipeline_.size();
    // add empty lines until one BEFORE cycle we want to assign
    if (modCycle >= fuEpSize) {
        fuExecutionPipeline_.resize(modCycle);
    }
    fuEpSize = fuExecutionPipeline_.size();

    for (unsigned int i = 0; i < resources->maximalLatency(); i++) {
        if (instructionIndex(cycle + i) >= fuEpSize) {
            // cycle is beyond size() so we add line to it.
            fuExecutionPipeline_.resize(instructionIndex(cycle + i) + 1);
            fuEpSize = instructionIndex(cycle+i) + 1;
        }

        // then we can insert the resource usage.
        for (unsigned int j = 0 ; j < resources->numberOfResources(); j++) {
            if (fuExecutionPipeline_.ref(instructionIndex(cycle + i)).size()
                == 0) {
                fuExecutionPipeline_.ref(instructionIndex(cycle+i)) =
                    ResourceReservationVector(resources->numberOfResources());
            }
            ResourceReservation& rr = 
                fuExecutionPipeline_.ref(instructionIndex(cycle+i))[j];
            if (resources->operationPipeline(pIndex,i,j)) {
                if (rr.first != NULL) {
                    assert(rr.second == NULL && "Resource already in use?");
                    rr.second = &node;
                } else { // rr.first == NULL
                    rr.first = &node;
                }
            }
        }
    }
    setResultWriten(pOp, cycle);
}

void
ExecutionPipelineResource::unassign(const int, MoveNode&)
    throw (Exception) {
    abortWithError("Execution Pipeline Resource needs 3 arguments unassign");
}

/**
 * Unassign resource from given node for given cycle.
 *
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * @param source Indicates if we want to unassign source part of move
 * in case move is bypassed
 * @throw In case there was no previous assignment or wrong operation
 * is unassigned
 */
void
ExecutionPipelineResource::unassignSource(
    const int cycle,
    MoveNode& node) {
    cachedSize_ = INT_MIN;

    int modCycle = instructionIndex(cycle);
    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }

    if (node.cycle() != cycle) {
        throw InvalidData(__FILE__, __LINE__, __func__,
            "Trying to unassign node from different cycle "
            "then it was assigned to!");
    }
    assert(node.isSourceOperation());

    const TTAMachine::Port& port = resultPort(node);
    ResultVector& resultRead = resultRead_[&port];
    ProgramOperation& po = node.sourceOperation();

    /// Unscheduling result read
    if (assignedSourceNodes_.find(&node) != assignedSourceNodes_.end()) {
        assignedSourceNodes_.erase(&node);
        
        if (MapTools::containsKey(storedResultCycles_, &node)) {
            /// Remove record of result beeing written to result register
            /// or decrease count in case there are more results
            unsigned int resultReady =
                MapTools::valueForKey<int>(storedResultCycles_, &node);
            
            unsetResultWriten(
                port, resultReady, node.sourceOperation());
            
            storedResultCycles_.erase(&node);
            
            // assert fail is much nicer than unknown exception.
            assert(modCycle < (int)resultRead.size());
            ResultHelperPair& resultReadPair = resultRead.ref(modCycle);
                
            if (resultReadPair.first.po == &po) {
                /// Remove record or decrease count of result read moves
                resultReadPair.first.count--;
                if (resultReadPair.first.count == 0) {
                    resultReadPair.first = resultReadPair.second;
                    
                    resultReadPair.second.count = 0;
                    resultReadPair.second.realCycle = modCycle;
                    resultReadPair.second.po = NULL;
                }
            } else {
                if (resultReadPair.second.po == &po) {
                    ///Remove record or decrease count of result read moves
                    resultReadPair.second.count--;
                    if (resultReadPair.second.count == 0) {
                        resultReadPair.second.realCycle = modCycle;
                        resultReadPair.second.po = NULL;
                    }
                }
            } 
        }
    }
}

#pragma GCC diagnostic ignored "-Wunused-variable"
/**
 * Unassign resource from given node for given cycle.
 *
 * @param cycle Cycle to remove assignment from
 * @param node MoveNode to remove assignment from
 * in case move is bypassed
 * @throw In case there was no previous assignment or wrong operation
 * is unassigned
 */
void
ExecutionPipelineResource::unassignDestination(
    const int cycle,
    MoveNode& node) {

    if (!node.isDestinationOperation()) {
        return;
    }

    int modCycle = instructionIndex(cycle);
    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }

    if (assignedDestinationNodes_.find(&node) ==
        assignedDestinationNodes_.end()) {
        return;
    }
    /// Now unassing destination part of move
    assignedDestinationNodes_.erase(&node);

    unsigned int progOpCount = operandsWriten_.size();
    if ((unsigned(modCycle) >= fuExecutionPipeline_.size() &&
        instructionIndex(cycle) >= progOpCount) ||
        cycle < 0) {
        std::string msg = "Trying to unassign cycle from out of scope - ";
        msg += Conversion::toString(cycle);
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    ProgramOperation* pOp = &node.destinationOperation();
    int firstCycle = 0;
    int lastCycle = 0;
    int triggering = 0;
    findRange(cycle, node, firstCycle, lastCycle, triggering);

    int modFirstCycle = instructionIndex(firstCycle);
    for (int j = 0; j < pOp->inputMoveCount(); j++) {
        if (pOp->inputMove(j).isScheduled() &&
            &pOp->inputMove(j) != &node) {
            if (pOp->inputMove(j).cycle() == firstCycle) {
                // found second operand in same cycle, keep PO for that cycle
                firstCycle++;
            }
            if (pOp->inputMove(j).cycle() == lastCycle) {
                // found second operand in same cycle
                lastCycle--;
            }
        }
    }

    if (modFirstCycle < int(progOpCount) &&
        (firstCycle == cycle || lastCycle == cycle)) {
        // only remove PO if node was first or last of all inputs
        // do not make hole in between other operands of same PO!

        // no overlap.
        if (lastCycle >= firstCycle) {
            for (int i = lastCycle; i >= firstCycle; i--) {
                unsigned int modi = instructionIndex(i);
                if (operandsWriten_.ref(modi).first == pOp) {
                    if (operandsWriten_.ref(modi).second == NULL) {
                        operandsWriten_.ref(modi).first = NULL;
                        if (ii == INT_MAX && 
                            modi == operandsWriten_.size() - 1) {
                            operandsWriten_.erase_element(modi);
                        }
                    } else {
                        operandsWriten_.ref(modi).first = 
                            operandsWriten_.ref(modi).second;
                        operandsWriten_.ref(modi).second = NULL;
                    }
                } else {
                    if (operandsWriten_.ref(modi).second == pOp) {
                        operandsWriten_.ref(modi).second = NULL;
                    }
                }
            }
        }
    }

    // Cleanup the operands vector from the end of the scope
    int maxOperandsWrite = operandsWriten_.size() -1;
    if (ii == INT_MAX) {
        while (maxOperandsWrite >= 0 &&
               operandsWriten_.ref(maxOperandsWrite).first == NULL) {
            maxOperandsWrite--;
        }
        operandsWriten_.resize(maxOperandsWrite + 1);
    }

    if (!node.move().destination().isTriggering()) {
        return;
    }
    std::string opName = "";
    if (node.move().destination().isOpcodeSetting()) {
        opName = node.move().destination().operation().name();
    } else {
        opName = node.move().destination().hintOperation().name();
    }
    if (!resources->hasOperation(opName)) {
        std::string msg = "Trying to unassign operation \'";
        msg += opName ;
        msg += "\' not supported on FU!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }

    // can not trust size() since that one ignores empty pipeline
    // and here we need to go up to the maximalLatency
    unsigned int fuEpSize = fuExecutionPipeline_.size();
    if ((instructionIndex(cycle + resources->maximalLatency() - 1)) 
        >= fuEpSize) {
        std::string msg = "Unassigning operation longer then scope!";
        msg += " - cycle:";
        msg += Conversion::toString(cycle);
        msg += " - scope:";
        msg += Conversion::toString(fuEpSize);
        msg += " - ii:";
        msg += Conversion::toString(initiationInterval_);
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    for (unsigned int i = 0; i < resources->maximalLatency(); i++) {
        for (unsigned int j = 0 ; j < resources->numberOfResources(); j++) {
            assert(
                fuExecutionPipeline_.ref(instructionIndex(cycle+i)).size() != 0);
            ResourceReservation& rr = 
                fuExecutionPipeline_.ref(instructionIndex(cycle+i))[j];
            if (rr.first == &node) {
                assert(resources->operationPipeline(
                    resources->operationIndex(opName),i,j) &&
                       "unassigning pipeline res not used by this op");
                
                rr.first = rr.second;
                rr.second = NULL;
            } else {
                if (rr.second == &node) {
                    assert(resources->operationPipeline(
                        resources->operationIndex(opName),i,j) &&
                           "unassigning pipeline res not used by this op");
                    
                    rr.second = NULL;
                }
            }
        }
    }

    unsetResultWriten(*pOp, cycle);
}

#pragma GCC diagnostic warning "-Wunused-variable"

/**
 * Return true if resource can be assigned for given node in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param pSocket Socket which was assigned to move by previous broker
 * @param triggers Indicates if move is triggering
 * @return true if node can be assigned to cycle
 */
bool
ExecutionPipelineResource::canAssignSource(
    const int cycle,
    const MoveNode& node,
    const TTAMachine::Port& resultPort) const {
    
    assert(node.isSourceOperation());
    const ProgramOperation& po = node.sourceOperation();
    
    const TTAMachine::HWOperation& hwop =
        *fu_.operation(po.operation().name());
    const int outputIndex = node.move().source().operationIndex();
    if (initiationInterval_ != 0 && 
        hwop.latency(outputIndex) >= (int)initiationInterval_) {
        return false;
    }

    /// Testing the result read move
    /// Find the cycle first of the possible results of PO will be produced
    int resultReady = node.earliestResultReadCycle();

    /// Check if the port has a register. If not result read must be
    /// in same cycle as result ready.
    const TTAMachine::FUPort& port = *hwop.port(outputIndex);            
    if (port.noRegister() && resultReady != cycle) {
        return false;
    }
    if (resultReady != INT_MAX) {
        if (cycle < resultReady) {
            // resultReady is INT_MAX if trigger was not scheduled yet
            // also tested cycle can not be before result is in output
            // register
            return false;
        }
    
        const MoveNode* trigger = po.triggeringMove();
        int triggerCycle = (trigger != NULL && trigger->isScheduled()) ? 
            trigger->cycle() : -1;
        return resultNotOverWritten(
                cycle, resultReady, node, resultPort, 
                trigger, triggerCycle) && 
            resultAllowedAtCycle(
                resultReady, po, resultPort, *trigger, triggerCycle);
    } else {
        // limit result cycle to latency of operation, so that 
        // trigger does nto have to be scheduled to negative cycle.
        // find the OSAL id of the operand of the output we are reading
        if (hwop.latency(outputIndex) > cycle) {
            return false;
        }

        /// Check if the port has a register. If not result read must be
        /// in same cycle as result ready.
        const TTAMachine::HWOperation& hwop = 
            *fu_.operation(po.operation().name());
        const TTAMachine::FUPort& port = 
            *hwop.port(node.move().source().operationIndex());    
        if (port.noRegister() && resultReady != cycle) {
            return false;
        }        
        const MoveNode* trigger = po.triggeringMove();
        int triggerCycle = (trigger != NULL && trigger->isScheduled()) ? 
        trigger->cycle() : -1;
        
        // We need to test if the write in given cycle is possible
        // even if we do not yet have trigger scheduled and
        // node.earliestResultReadCycle() returns INT_MAX.
        // This allows for comparison of result moves in Bottom-Up schedule
        return resultAllowedAtCycle(
            cycle, po, resultPort, *trigger, triggerCycle);
    }
    return true;
}

/**
 * Return true if resource can be assigned for given node in given cycle.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param pSocket Socket which was assigned to move by previous broker
 * @param triggers Indicates if move is triggering
 * @return true if node can be assigned to cycle
 */
bool
ExecutionPipelineResource::canAssignDestination(
    const int cycle,
    const MoveNode& node,
    bool triggers) const {

    if (!node.isDestinationOperation()) {
        return true;
    }

    unsigned int modCycle = instructionIndex(cycle);
    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }
    
    // then handle operation inputs.

    MoveNode* newNode = const_cast<MoveNode*>(&node);
    ProgramOperation* pOp = NULL;
    try {
        pOp = &newNode->destinationOperation();
    } catch (const InvalidData& e) {
        abortWithError(e.errorMessage());
    }
    int progOpCount = operandsWriten_.size();
    int firstCycle = 0;
    int lastCycle = 0;
    int triggering = -1;
    findRange(cycle, node, firstCycle, lastCycle, triggering);

    /// Check if operand port has register. If not all operands must be 
    /// written in same cycle!
    const TTAMachine::HWOperation& hwop = 
        *fu_.operation(pOp->operation().name());
    TTAMachine::FUPort& port =
        *hwop.port(newNode->move().destination().operationIndex());                   
    if (port.noRegister() && firstCycle != cycle) {
        debugLogRM("port.noRegister() && firstCycle != cycle for: " + node.toString());
        return false;
    }

    for (int i = firstCycle; i <= lastCycle; i++) {
        int modi = instructionIndex(i);
        if (modi < 0 ) {
            break;
        }

        if (modi < progOpCount) {
            // check if some other operation writes operand this cycle.
            ProgramOperation* oldPO = operandsWriten_[modi].first;
            // if already 2 ops do it, can never fit 3rd
            if (oldPO != pOp && oldPO != NULL) {
                if (operandsWriten_[modi].second != NULL &&
                    operandsWriten_[modi].second != pOp) {
                    return false;
                }

                // check operands of other op if all exclusive.
                for (int j = 0; j < oldPO->inputMoveCount(); j++) {
                    MoveNode& otherOpMove = oldPO->inputMove(j);
                    if (!exclusiveMoves(&otherOpMove, &node, i)) {
                        return false;
                    }
                }
            }
        }
    }

    // triggering move may be different on a target machine then on
    // universal machine, test triggering using data from FUBrokers
    if (!triggers) {
        // if operand later than trigger, fail.
        if (triggering != -1 && cycle > triggering) {
            return false;
        }

        // make sure operand not so early that it conflicts with same
        // operation on previous iteration.
        if (triggering != -1 && 
            ii != INT_MAX && cycle + (int)ii <= triggering) {
            return false;
        }
        return true;
    }

    // we have a trigger. Cannot be later than operand.
    if (lastCycle > cycle) {
        return false;
    }
    
    // Too late to schedule trigger, results would not be ready in time.
    if (cycle > node.latestTriggerWriteCycle()) {
        return false;
    }
    // make sure operand not so early that it conflicts with same
    // operation on previous iteration.
    if (ii != INT_MAX && firstCycle + (int)ii <= cycle) {
        return false;
    }

    std::string opName = "";
    if (newNode->move().destination().isOpcodeSetting()) {
        opName = newNode->move().destination().operation().name();
        debugLogRM(opName);
    } else {
        // If target architecture has different opcode setting port
        // as universal machine, pick  a name of operation from a hint
        if (triggers) {
            opName = newNode->move().destination().hintOperation().name();
        }
        if (opName == "") {
            std::string msg = "Using non opcodeSetting triggering move. ";
            msg += "Move: " + newNode->toString();
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }
    }

    if (!resources->hasOperation(opName)) {
        // Operation no supported by FU
        debugLogRM(opName + " not supported by the FU!");
        return false;
    }
// conflicts started here

    int pIndex = resources->operationIndex(opName);
    
    bool canAssign = true;
    std::vector<std::vector<bool> > assigned;

    std::size_t maxSize = resources->maximalLatency() + modCycle;
    if (maxSize > ii) {
        maxSize = ii;
    }

    assigned.resize(resources->maximalLatency());

    for (unsigned int i = 0; i < resources->maximalLatency(); i++) {
        assigned[i].resize(resources->numberOfResources(), false);
    }

    for (unsigned int i = 0; i < resources->maximalLatency() 
             && canAssign; i++) {
        unsigned int modci = instructionIndex(cycle+i); 
        
        if (ii == INT_MAX) {
            if (modci >= static_cast<unsigned int>(size())) {
                break;
            }
        } else {
            // may still fail on bigger value of i if overlaps,
            // so continue instead of break.
            if (fuExecutionPipeline_.size() <= modci) {
                continue;
            }
        }

        for (unsigned int j = 0 ; j < resources->numberOfResources(); j++) {
            if (fuExecutionPipeline_.ref(modci).size() == 0)
                continue;
                
            ResourceReservation& rr = 
                fuExecutionPipeline_.ref(modci)[j];

            // is this resource needed by this operation?
            if (resources->operationPipeline(pIndex,i,j)) {
                // is the resource free?
                if (rr.first != NULL) {
                    // can still assign this with opposite guard?
                    if (rr.second == NULL &&
                        exclusiveMoves(rr.first, &node, modCycle)) {
                        assigned[i][j] = true;
                        rr.second = &node;
                    } else { // fail.
                        canAssign = false;
                        break;
                    }
                } else { // mark it used for this operation.
                    assigned[i][j] = true;
                    rr.first = &node;
                }
            }
        }
    }
    
    // reverts usage of this op to resource used table
    for (unsigned int i = 0; i < resources->maximalLatency(); i++) {
        for (unsigned int j = 0; j < resources->numberOfResources(); j++) {
            if (assigned[i][j]) {
                ResourceReservation& rr = 
                    fuExecutionPipeline_.ref(instructionIndex(cycle+i))[j];
                // clear the usage.
                if (rr.first == &node) {
                    assert(rr.second == NULL);
                    rr.first = rr.second;
                    rr.second = NULL;
                } else {
                    if (rr.second == &node) {
                        rr.second = NULL;
                    } else {
                        assert(0&& "assignment to undo not found");
                    }
                }
            }
        }

    }
    if (!canAssign) {
        return false;
    }
    
    // Test for result read WaW already when scheduling trigger.
    return testTriggerResult(node, cycle);
}

/**
 * Always return true.
 *
 * @return true
 */
bool
ExecutionPipelineResource::isExecutionPipelineResource() const {
    return true;
}

/**
 * Tests if all referred resources in dependent groups are of proper types.
 *
 * @return true Allways true, pipelines are internal to object.
 */
bool
ExecutionPipelineResource::validateDependentGroups() {
    return true;
}

/**
 * Tests if all resources in related resource groups are of proper types.
 *
 * @return true If all resources in related resource groups are
 *              Triggering PSockets - for now InputPSockets
 */
bool
ExecutionPipelineResource::validateRelatedGroups() {
    for (int i = 0; i < relatedResourceGroupCount(); i++) {
        for (int j = 0, count = relatedResourceCount(i); j < count; j++) {
            if (!relatedResource(i, j).isInputPSocketResource())
                return false;
        }
    }
    return true;
}

/**
 * Return number of cycles current execution pipeline for FU contains.
 * Effectively, highest cycle in which any of the resources of an
 * FU is occupied plus 1.
 *
 * @return Number of cycles in pipeline.
 */
int
ExecutionPipelineResource::size() const {

#if 0
    // Breaks for load and store units with internal pipeline resources!!!
    if (cachedSize_ != INT_MIN ) {
        return cachedSize_;
    }
#endif
    int length = fuExecutionPipeline_.size() - 1;
    if (length == -1) {
        cachedSize_ = 0;
        return 0;
    }
    for (int i = length; i >= 0; i--) {
        for (unsigned int j = 0; j < resources->numberOfResources(); j++) {
            if (fuExecutionPipeline_.ref(i).size() == 0) {                
                continue;
            }
            ResourceReservation& rr = 
                fuExecutionPipeline_.ref(i)[j];
            if (rr.first != NULL) {
                cachedSize_ = i + 1;
                return i + 1;
            }
        }
    }
    cachedSize_ = 0;
    return 0;
}

/**
 * For given MoveNode and cycle find cycle in which other MoveNodes
 * of same program operation are already scheduled. The closest ones.
 * This returns real cycles, not modulo cycles.
 *
 * @param cycle Cycle to test
 * @param node MoveNode to test
 * @param first Will return address of closest previous cycle already
 * used by MoveNode of same program operation
 * @param last Will return address of closest later cycle already
 * used by MoveNode of same program operation
 * @param triggering returns the cycle of the trigger of the 
 *        destination operation. -1 if trigger not scheduled.
 */
void
ExecutionPipelineResource::findRange(
    const int cycle,
    const MoveNode& node,
    int& first,
    int& last,
    int& triggering) const {

    ProgramOperation* pOp = NULL;
    try {
        pOp = &node.destinationOperation();
    } catch (const InvalidData& e) {
        abortWithError(e.errorMessage());
    }

    last = INT_MAX;
    first = -1;
    triggering = -1;
    for (int k = 0; k < pOp->inputMoveCount(); k++) {
        // find closest cycle from PO to given node, before and after
        if (pOp->inputMove(k).isScheduled()) {
            if (&node != &pOp->inputMove(k)) {
                /// Check if the current port has register.
                /// If not all the operands must be written in same cycle.
                const TTAMachine::HWOperation& hwop = 
                    *fu_.operation(pOp->operation().name());
                TTAMachine::FUPort& port =
                    *hwop.port(node.move().destination().operationIndex());
                if (port.noRegister()) {
                    triggering = pOp->inputMove(k).cycle();
                    last = triggering;
                    first = triggering;
                    return;
                }
            }
            if (pOp->inputMove(k).move().isTriggering()) {
                triggering = pOp->inputMove(k).cycle();
            }
            if (pOp->inputMove(k).cycle() > cycle &&
                pOp->inputMove(k).cycle() < last) {
                last = pOp->inputMove(k).cycle();
            }
            if (pOp->inputMove(k).cycle() < cycle &&
                pOp->inputMove(k).cycle() > first) {
                first = pOp->inputMove(k).cycle();
            }
        }
    }
    if (first == -1) {
        first = cycle;
    }
    if (last == INT_MAX) {
        last = cycle;
    }
}

/**
 * Returns the highest cycle known to Execution Pipeline to be used by either
 * pipeline resources or some operands, trigger or result read/write
 *
 *
 * TODO: module thingies
 *
 * @return Highest cycle in which the pipeline is known to be used.
 */
int
ExecutionPipelineResource::highestKnownCycle() const {
    if (initiationInterval_ == 0 || initiationInterval_ == INT_MAX) {
        int maximum = operandsWriten_.size() - 1;
        while (maximum >= 0 && operandsWriten_[maximum].first == NULL) {
            maximum--;
        }
        
        for (ResultMap::const_iterator rwi = resultWriten_.begin(); 
             rwi != resultWriten_.end(); rwi++) {
            const ResultVector& resultWriten = rwi->second;
            int maxResults = resultWriten.size() - 1;
            while (maxResults >= 0 && 
                   resultWriten[maxResults].first.po == NULL) {
                maxResults--;
            }
            if (maxResults> maximum) {
                maximum = maxResults;
            }
        }

        for (ResultMap::const_iterator rri = resultRead_.begin(); 
             rri != resultRead_.end(); rri++) {
            const ResultVector& resultRead = rri->second;

            int maxResultReads = resultRead.size() -1;
            while (maxResultReads >= 0 &&
                   resultRead[maxResultReads].first.po == NULL) {
                maxResultReads--;
            }
            if (maxResultReads > maximum) {
                maximum = maxResultReads;
            }
        }
        // size returns count of cycle, max cycle address needs -1
        return std::max(maximum, size() - 1);
    } else {
        int highest = -1;
        for (ResultMap::const_iterator rwi = resultWriten_.begin(); 
             rwi != resultWriten_.end(); rwi++) {
            const ResultVector& resultWriten = rwi->second;

            for (unsigned int i = 0; i < resultWriten.size(); i++) {
                const ResultHelperPair& rhp = resultWriten[i];
                if (rhp.first.po != NULL) {
                    if (int(rhp.first.realCycle) > highest) {
                        highest = rhp.first.realCycle;
                    }
                    if (rhp.second.po != NULL) {
                        if (int(rhp.second.realCycle) > highest) {
                            highest = rhp.second.realCycle;
                        }
                    }
                } 
            }
        }
        for (ResultMap::const_iterator rri = resultRead_.begin(); 
             rri != resultRead_.end(); rri++) {
            const ResultVector& resultRead = rri->second;
            for (unsigned int i = 0; i < resultRead.size(); i++) {
                const ResultHelperPair& rrp = resultRead[i];
                if (rrp.first.po != NULL) {
                    if (int(rrp.first.realCycle) > highest) {
                        highest = rrp.first.realCycle;
                    }
                    if (rrp.second.po != NULL) {
                        if (int(rrp.second.realCycle) > highest) {
                            highest = rrp.second.realCycle;
                        }
                    }
                }
            }
        }
        // TODO: operand writes not yet handled for this.
        return highest;
    }
}

/**
 * Returns a cycle in which result of next program operation will be
 * writen to result. This method results the next one of any iteration,
 * not just the current iteration.
 *
 * @param cycle Cycle from which to start testing.
 * @param node Node for which to test
 * @return Cycle in which next result will be writen, overwriting curent one.
 */
int
ExecutionPipelineResource::nextResultCycle(
    const TTAMachine::Port& port,
    int cycle, const MoveNode& node, const MoveNode* trigger, int triggerCycle)
    const {

    ResultMap::const_iterator rwi = resultWriten_.find(&port);
    if (rwi == resultWriten_.end()) {
        return INT_MAX;
    }
    const ResultVector& resultWriten = rwi->second;
    
    if (!node.isSourceOperation()) {
        throw InvalidData(__FILE__, __LINE__, __func__,
            "Trying to get next result for move that is not "
            "in ProgramOperation");
    }
    ProgramOperation* sourcePo = NULL;
    if (node.isSourceOperation()) {
        sourcePo = &node.sourceOperation();
        if (trigger == NULL) {
            trigger = sourcePo->triggeringMove();
        }
    }

    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }

    unsigned int rwSize = resultWriten.size();
    for (unsigned int i = cycle; i < cycle + ii; i++) {
        unsigned int modi = instructionIndex(i);
        
        if (rwSize <= modi) {
            if (ii == INT_MAX) {
                return INT_MAX;
            } else {
                continue;
            }
        }
        
        const ResultHelperPair& rhp = resultWriten[modi];
        if (rhp.first.count != 0) {
            assert(rhp.first.po != NULL);
            if (rhp.first.po != sourcePo &&
                !exclusiveMoves(
                    rhp.first.po->triggeringMove(), trigger,
                    triggerCycle)) {
                return rhp.first.realCycle;
            }
            if (rhp.second.po != sourcePo && rhp.second.count != 0) {
                assert(rhp.second.po != NULL);
                if (!exclusiveMoves(
                        rhp.second.po->triggeringMove(), trigger,
                        triggerCycle)) {
                    return rhp.second.realCycle;
                }
            }
        }
    }
    return INT_MAX;
}

/** 
 * Returns cycle when result of some PO is ready.
 * 
 * @param po programoperation
 * int resultReadCycle cycle when the result is read
 *
 * @TODO: multiple out values still not supported correctly.
 */
int ExecutionPipelineResource::resultReadyCycle(
    const ProgramOperation& po, const TTAMachine::Port& resultPort) const {

    MoveNode* trigger = po.triggeringMove();
    if (trigger == NULL || !trigger->isScheduled()) {
        return -1;
    }

    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(po.operation().name());
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        int outIndex = op.numberOfInputs() + 1 + i;
        const TTAMachine::Port *p = hwop.port(outIndex);
        if (p == &resultPort) {
            return trigger->cycle() + hwop.latency(outIndex);
        }
    }
    return -1;
}

/**
 * Checks whether both of two moves have exclusive guards so that 
 * both moves are never executed, only either of those.
 * Those can then be scheduled to use same resources.
 *
 * This checks that the guards are exclusive, and that the moves are
 * to be scheduled in same cycle (one already scheduled, on is going to
 * be scheudled to given cycle, which has to be the same.
 * the same cycle requirements makes sure the value of the guard cannot be
 * changed between the moves.
 *
 * @param mn1 movenode which has already been scheduled
 * @param mn2 move which we are going to schedule
 * @param cycle cycle where we are going to scheudle mn2.
 */
bool ExecutionPipelineResource::exclusiveMoves(
    const MoveNode* mn1, const MoveNode* mn2, int cycle) const {
#ifdef NO_OVERCOMMIT
    return false;
#else
    if (mn1 == NULL || mn2 == NULL || !mn1->isMove() || !mn2->isMove()) {
        return false;
    }
    
    if (mn1->move().isUnconditional() || mn2->move().isUnconditional()) {
        return false;
    }

    if (ddg_ != NULL) {
        return ddg_->exclusingGuards(*mn1, *mn2);
    }

    if (!mn1->move().guard().guard().isOpposite(mn2->move().guard().guard())) {
        return false;
    }

    if (!mn1->isScheduled()) {
        return false;
    }

    if ((mn2->isScheduled() && mn1->cycle() == mn2->cycle()) ||
        (!mn2->isScheduled() && (mn1->cycle() == cycle || cycle == INT_MAX))) {
        return true;
    }
    return false;
#endif
}

/**
 * Clears bookkeeping of the scheduling resource. 
 * 
 * After this call the state of the resource should be identical to a 
 * newly-created and initialized resource.
 */
void
ExecutionPipelineResource::clear() {
    SchedulingResource::clear();
    fuExecutionPipeline_.clear();
    resultWriten_.clear();
    resultRead_.clear();
    operandsWriten_.clear();
    storedResultCycles_.clear();
    assignedSourceNodes_.clear();
    assignedDestinationNodes_.clear();
    cachedSize_ = 0;
    ddg_ = NULL;
}

void
ExecutionPipelineResource::setDDG(const DataDependenceGraph* ddg) {
    ddg_ = ddg;
}

/**
 * Tests the conflicts caused by results if a trigger
 * is scheduled to given cycle.
 */
bool
ExecutionPipelineResource::testTriggerResult(
    const MoveNode& trigger, int cycle) const {
    ProgramOperation& po = trigger.destinationOperation();
    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    // Loops over all output values produced by this 
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        int outIndex = op.numberOfInputs() + 1 + i;
        const TTAMachine::Port& port = *hwop.port(outIndex);
        int resultCycle = cycle + hwop.latency(outIndex);
        
        if (!resultAllowedAtCycle(
                resultCycle, po, port, trigger, cycle)) {
            return false;
        }

        if (initiationInterval_ != 0 && 
            hwop.latency(outIndex) >= (int)initiationInterval_) {
            return false;
        }
    }

    // If we have alreayd scheduled some result, we have to make sure
    // nobody overwrites it.
    for (int i = 0; i < po.outputMoveCount(); i++) {
        MoveNode& mn = po.outputMove(i);
        if (mn.isScheduled()) {
            assert(mn.move().source().isFUPort());
            int outIndex = mn.move().source().operationIndex();
            const TTAMachine::Port& port = *hwop.port(outIndex);
            int resultCycle = cycle + hwop.latency(outIndex);
            if (!resultNotOverWritten(
                    mn.cycle(), resultCycle, mn, port, &trigger, cycle)) {
                return false;
            }
        }
    }
    return true;

}

/**
 * Tests that a new result at given cycle does not mess up result of 
 * some other operation
 *
 * @param resultCycle cycle when the nw result appears
 * @po Programoperation which the new result belongs to
 * @resultPort port where the result is written to
 * @trigger trigger movenode of the operation
 * @triggercycle cycle of the trigger
 */
bool
ExecutionPipelineResource::resultAllowedAtCycle(
    int resultCycle, const ProgramOperation& po, 
    const TTAMachine::Port& resultPort, const MoveNode& trigger, int triggerCycle) 
    const {

    // if none found from the map, this used.
    ResultVector empty;
    ResultMap::const_iterator rri = resultRead_.find(&resultPort);

    const ResultVector& resultRead = (rri != resultRead_.end()) ?
        rri->second : empty;

    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }
    
    unsigned int rrSize = resultRead.size();
    unsigned int rrMod = instructionIndex(resultCycle);
    for (unsigned int i = rrMod; i < rrMod + ii; i++) {
        unsigned int modi = instructionIndex(i);
        if (modi >= rrSize) {
            if (ii == INT_MAX) {
                break;
            } else {
                // may be read at small instr index of next iteration.
                // so may not abort, but can skip this cycle.
                continue;
            }
        }
        const ResultHelperPair& resultReadPair = resultRead[modi];
        if (resultReadPair.first.count > 0) {
            // same operation reading result again. cannot fail.
            if (resultReadPair.first.po == &po) {
                break;
            } 
            assert (resultReadPair.first.po != NULL);
            bool modiLooped = modi < rrMod;
            
            // first check conflicts to first of po.
            MoveNode* otherTrigger = resultReadPair.first.po->triggeringMove();
            if (!exclusiveMoves(otherTrigger, &trigger, triggerCycle)) {
                if (resultReadPair.first.po == &po) {
                    break;
                }
                // here check conflicts against first.
                
                int otherReady = resultReadyCycle(
                    *resultReadPair.first.po, resultPort);
                
                int or2Mod = instructionIndex(otherReady);
                bool orLooped = or2Mod > (int)modi; // FAIL HERE?
                
                // neither looped or both looped.                       
                if (modiLooped == orLooped) {
                    if (or2Mod <= (int)rrMod) {
                        return false;
                    } else {
                        if (otherTrigger->move().isUnconditional()) {
                            break;
                        }
                    }
                } else {
                    // either one looped, order has to be reverse.
                    if (or2Mod >= (int)rrMod) {
                        return false;
                    } else {
                        if (otherTrigger->move().isUnconditional()) {
                            break;
                        }
                    }
                }
            }

            // then check conflicts to second po
            if (resultReadPair.second.count > 0) {
                MoveNode* otherTrigger = resultReadPair.second.po->triggeringMove();
                if (!exclusiveMoves(otherTrigger, &trigger, triggerCycle)) {
                    if (resultReadPair.second.po == &po) {
                        break;
                    }
                    int otherReady = resultReadyCycle(
                        *resultReadPair.second.po, resultPort);
                    
                    unsigned int or2Mod = instructionIndex(otherReady);
                    bool orLooped = or2Mod > modi;
                    
                    // neither looped or both looped.
                    if (modiLooped == orLooped) {
                        if (or2Mod <= rrMod) {                           
                            return false;
                        } else {
                            if (otherTrigger->move().isUnconditional()) {
                                break;
                            }
                        }
                    } else {
                        // either looped, order has to be reverse.
                        if (or2Mod >= rrMod) {
                            return false;
                        } else {
                            if (otherTrigger->move().isUnconditional()) {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

/**
 * Gives the port where from the movenode reads a result.
 */
const TTAMachine::Port&
ExecutionPipelineResource::resultPort(const MoveNode& mn) const {
    assert(mn.isSourceOperation());
    const ProgramOperation& po = mn.sourceOperation();
    const TTAMachine::HWOperation& hwop = 
        *fu_.operation(po.operation().name());
    return *hwop.port(mn.move().source().operationIndex());
}

/**
 * Returns if the result can be scheduled to given cycle
 * so that the results of other operations do not overwrite it.
 *
 * @param resultReadCycle cycle when the result is read
 * @param resultReadyCycle cycle when the result becomes available
 * @param po ProgramOperation where the result move belongs
 * @param node the result read node being scheduled
 * @param resultPort the port which is being read by the result
 * @param trigger Trigger of the program operation
 * @param triggercycle cycle of the trigger of the PO
 */
bool 
ExecutionPipelineResource::resultNotOverWritten(
    int resultReadCycle, int resultReadyCycle,
    const MoveNode& node, const TTAMachine::Port& resultPort, 
    const MoveNode* trigger, int triggerCycle) const {
    unsigned int rrMod = instructionIndex(resultReadyCycle);

    unsigned int modCycle = instructionIndex(resultReadCycle);
    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        // no loop scheduling.
        ii = INT_MAX;
    } else {
        // loop scheudling.
        // make sure opeation does not cause conflict
        // with itself on next loop iteration.
        if (resultReadyCycle + (int)ii <= resultReadCycle) {
            return false;
        }
    }
    // Test when some other PO will write result to result register
    // starting from cycle when result could be ready
    int otherResult = nextResultCycle(
        resultPort, resultReadyCycle, node, trigger, triggerCycle);
    int orMod = instructionIndex(otherResult);
    
    if (otherResult == INT_MAX && ii != INT_MAX) {
        orMod = INT_MAX;
    }
    
    // overlaps between these.
    // TODO: these checks fail. true when no ii.
    if (orMod < (int)rrMod) {
        // overlaps between these. both overlap, oridinary comparison.
        if (modCycle < rrMod && orMod <= (int)modCycle) {
            // Result will be overwritten before we will read it
            return false;
        } 
        // is cycle does not overlap, it's earlier, so does not fail.
    } else {
        // overlaps between these. it's later, fails.
        // neither overlaps. ordinary comparison.
        if ((rrMod != INT_MAX && orMod != INT_MAX && modCycle < rrMod) || 
            orMod <= (int)modCycle) {
            // Result will be overwritten before we will read it
            return false;
        }
    }
    return true;
}
