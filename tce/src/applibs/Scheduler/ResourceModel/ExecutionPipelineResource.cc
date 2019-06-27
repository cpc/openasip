/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @author Heikki Kultala 2013 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */

//#define DEBUG_RM

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
    cachedSize_(INT_MIN), maxCycle_(INT_MAX), ddg_(NULL), fu_(fu) {
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
ExecutionPipelineResource::isInUse(const int cycle) const {
    // check if any operand port is used
    int modCycle = instructionIndex(cycle);
    for (OperandWriteMap::const_iterator i = operandsWriten_.begin();
         i != operandsWriten_.end(); i++) {
        
        const OperandWriteVector& operandWrites = i->second;
        OperandWriteVector::const_iterator j = operandWrites.find(modCycle);
        if (j != operandWrites.end()) {
            return true;
        }
    }

    for (ResultMap::const_iterator rri = resultRead_.begin(); 
         rri != resultRead_.end(); rri++) {
        const ResultVector& resultRead = rri->second;
        unsigned int resultReadCount = resultRead.rbegin()->first;//.size();
        if (modCycle <  (int)resultReadCount && 
            (MapTools::valueForKeyNoThrow<ResultHelperPair>(
                 resultRead, modCycle)).first.po != NULL) {
            /// Some result is already read in tested cycle
            return true;
        }
    }
    if (modCycle >= (int)size()) {
        /// Cycle is beyond already scheduled scope, not in use therefore
        return false;
    }
    if (resources->numberOfResources() !=
        (MapTools::valueForKeyNoThrow<ResourceReservationVector>(
             fuExecutionPipeline_, modCycle)).size()) {
        std::string msg = "Execution pipeline is missing resources!";
        throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
    }
    for (unsigned int i = 0; i < resources->numberOfResources(); i++) {
        const ResourceReservationVector& rrv = 
            MapTools::valueForKeyNoThrow<ResourceReservationVector>(
                fuExecutionPipeline_, modCycle);
        if (rrv.size() == 0) {
            return false;
        }
                
        const ResourceReservation& rr = rrv[i];

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
    // check if all operand ports are used
    int modCycle = instructionIndex(cycle);
    for (int i = 0; i < fu_.portCount(); i++) {
        const TTAMachine::BaseFUPort* port = fu_.port(i);
        OperandWriteMap::const_iterator it = operandsWriten_.find(port);
        if (it == operandsWriten_.end()) {
            return true;
        }
        const OperandWriteVector& operandWrites = it->second;
        OperandWriteVector::const_iterator j = operandWrites.find(modCycle);
        if (j == operandWrites.end()) {
            return true;
        }

        MoveNodePtrPair mnpp = j->second;
        if (mnpp.first == NULL || 
            (!mnpp.first->move().isUnconditional() &&
             mnpp.second == NULL)) {
            return true;
        }
    }
    return true;
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

    ResultHelperPair& rhp = resultWriten[modCycle];
    if (rhp.first.po == NULL) {
        rhp.first = ResultHelper(realCycle, &po, 1);
    } else {
        if (rhp.first.realCycle == realCycle &&
            rhp.first.po == &po) {
            rhp.first.count++;
        } else {
            if (rhp.second.po == NULL) {
                rhp.second = ResultHelper(realCycle, &po, 1);
            } else {
                rhp.second.count++;
            }
        }
    }
}

/*
 * Record PO in cycle where operand is used by the pipeline.
 *
 */
void 
ExecutionPipelineResource::setOperandUsed(
    const TTAMachine::Port& port, unsigned int realCycle, 
    const ProgramOperation& po) {

    OperandUseVector& operandUsed = operandsUsed_[&port];
    unsigned int modCycle = instructionIndex(realCycle);

    OperandUsePair& oup = operandUsed[modCycle];
    if (oup.first.po == NULL) {
        oup.first.po = &po;
    } else {
        if (oup.first.realCycle == realCycle &&
            oup.first.po == &po) {
            assert(false);
        } else {
            if (oup.second.po == NULL) {
                oup.second.po = &po;
            } else {
                assert(false);
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

void 
ExecutionPipelineResource::setOperandsUsed(
    const ProgramOperation& po, unsigned int triggerCycle) {

    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    // Loops over all output values produced by this 
    for (int i = 0; i < op.numberOfInputs(); i++) {
        const TTAMachine::Port& port = *hwop.port(i+1);
        int operandUseCycle = triggerCycle + hwop.slack(i+1);
        
        setOperandUsed(port, operandUseCycle, po);
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
    ResultHelperPair& rhp = resultWriten[rrMod];
    
    if (rhp.first.po == &po) {
        // Decrease counter of results written in
        rhp.first.count--;
        if (rhp.first.count == 0) {
            if (rhp.second.count == 0) {
                resultWriten.erase(rrMod);
            } else {
                // move second to first.
                rhp.first = rhp.second;
                rhp.second = ResultHelper(); // empty it.
            }
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
ExecutionPipelineResource::unsetOperandUsed(
    const TTAMachine::Port& port, unsigned int realCycle, 
    const ProgramOperation& po) {
    unsigned int modCycle = instructionIndex(realCycle);

    OperandUseVector& operandUsed = operandsUsed_[&port];
    OperandUseVector::iterator i = operandUsed.find(modCycle);
    assert(i != operandUsed.end());
    OperandUsePair& mnpp = i->second;
    if (mnpp.first.po == &po) {
        if (mnpp.second.po == NULL) {
            operandUsed.erase(i);
        } else {
            mnpp.first = mnpp.second;
            mnpp.second.po = NULL;
        }
    } else {
        if (mnpp.second.po == &po) {
            mnpp.second.po = NULL;
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
ExecutionPipelineResource::unsetOperandsUsed(
    const ProgramOperation& po, unsigned int triggerCycle) {

    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    // Loops over all output values produced by this 
    for (int i = 0; i < op.numberOfInputs(); i++) {
        const TTAMachine::Port& port = *hwop.port(i+1);
        int resultCycle = triggerCycle + hwop.slack(i+1);
        unsetOperandUsed(port, resultCycle, po);
    }
}

void
ExecutionPipelineResource::assign(const int, MoveNode&) {
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
    assignedSourceNodes_.insert(std::pair<int, MoveNode*>(cycle, &node));
    ProgramOperation* pOp = &node.sourceOperation();
    
    /// Record Program Operation in cycle where the "result read"
    /// is scheduled
    unsigned int readCount = resultRead.size();
    if (readCount <= modCycle) {
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
    ResultHelperPair& rhp = resultRead[modCycle];
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
    cachedSize_ = INT_MIN;

#ifdef DEBUG_RM
    std::cerr << "\tAssigning destination: " << node.toString() << std::endl;
#endif
    if (!node.isDestinationOperation()) {
        return;
    }

    assignedDestinationNodes_.insert(std::pair<int, MoveNode*>(cycle, &node));

    int modCycle = instructionIndex(cycle);

    std::string opName = "";

    //TODO: is this correct trigger or UM trigger?
    if (node.move().destination().isTriggering()) {
#ifdef DEBUG_RM
        std::cerr << "\t\tis trigger!" << std::endl;
#endif
        assert(node.destinationOperationCount() == 1);

        ProgramOperation& pOp = node.destinationOperation();
        if (node.move().destination().isOpcodeSetting()) {
            opName = node.move().destination().operation().name();
        } else {
            std::string msg = "Using non opcodeSetting triggering move. ";
            msg += " Move: " + node.toString();
            throw ModuleRunTimeError(__FILE__, __LINE__, __func__, msg);
        }
        int pIndex = resources->operationIndex(opName);
        for (unsigned int i = 0; i < resources->maximalLatency(); i++) {
            int modic = instructionIndex(cycle+i);
            // then we can insert the resource usage.
            for (unsigned int j = 0 ; 
                 j < resources->numberOfResources(); j++) {
                if (fuExecutionPipeline_[modic].size()
                    == 0) {
                    fuExecutionPipeline_[modic] =
                        ResourceReservationVector(
                            resources->numberOfResources());
                }
                ResourceReservation& rr = 
                    fuExecutionPipeline_[modic][j];
                if (resources->operationPipeline(pIndex,i,j)) {
                    if (rr.first != NULL) {
                        assert(rr.second == NULL&&"Resource already in use?");
                        rr.second = &node;
                    } else { // rr.first == NULL
                        rr.first = &node;
                    }
                }
            }
        }
        setResultWriten(pOp, cycle);
        setOperandsUsed(pOp, cycle);
    }

    const TTAMachine::Port& opPort = operandPort(node);
    MoveNodePtrPair& mnpp = operandsWriten_[&opPort][modCycle];
    if (mnpp.first == NULL) {
        mnpp.first = &node;
    } else {
        if (mnpp.second != NULL || !exclusiveMoves(mnpp.first, &node, cycle)) {
            std::string msg =  name() + " had previous operation ";
            msg += mnpp.first->destinationOperation().toString() + "\n ";
            msg += mnpp.first->toString() + " in inst.index " ;
            msg += Conversion::toString(modCycle);
            msg += " other trigger: ";
            msg += mnpp.first->toString();
            msg += " Node: " + node.toString();
            msg += "\nThis op: " + node.destinationOperation().toString();
            msg += "\n";
            
            if (node.isDestinationOperation()) {
                msg += node.destinationOperation().toString();
            }
            throw InvalidData(__FILE__, __LINE__, __func__, msg);
        } else {
            
            // Marks all the cycles in range with PO
            // which is writing operands
            mnpp.second = &node;
        } 
    }
}

void
ExecutionPipelineResource::unassign(const int, MoveNode&) {
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
    if (MapTools::containsValue(assignedSourceNodes_, &node)) {
        MapTools::removeItemsByValue(assignedSourceNodes_, &node);

        
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
            ResultHelperPair& resultReadPair = resultRead[modCycle];
                
            if (resultReadPair.first.po == &po) {
                /// Remove record or decrease count of result read moves
                resultReadPair.first.count--;
                if (resultReadPair.first.count == 0) {
                    if (resultReadPair.second.count == 0) {
                        // erase from the bookkeeping, this is empty.
                        resultRead.erase(modCycle);
                    } else {
                        resultReadPair.first = resultReadPair.second;
                        resultReadPair.second.count = 0;
                        resultReadPair.second.realCycle = modCycle;
                        resultReadPair.second.po = NULL;
                    }
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

#ifdef DEBUG_RM
    std::cerr << "\tUnassigning destination: " << node.toString() << std::endl;
#endif

    if (!node.isDestinationOperation()) {
#ifdef DEBUG_RM
        std::cerr << "\t\tNot dest operatioN!" << std::endl;
#endif
        return;
    }

    int modCycle = instructionIndex(cycle);
    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }

    if (!MapTools::containsValue(assignedDestinationNodes_, &node)) {
#ifdef DEBUG_RM
        std::cerr << "\t\t assigned destinations not contain!" << std::endl;
#endif
        return;
    }
    /// Now unassing destination part of move
    MapTools::removeItemsByValue(assignedDestinationNodes_, &node);

    const TTAMachine::Port& opPort = operandPort(node);
    MoveNodePtrPair& mnpp = operandsWriten_[&opPort][modCycle];
    assert (mnpp.first != NULL);
    if (mnpp.second == &node) {
        mnpp.second = NULL;
    } else {
        assert(mnpp.first == &node);
        mnpp.first = mnpp.second;
        mnpp.second = NULL;
    }

    // if not trigger, we are done
    // TODO: correct trigger or UM trigger?
    if (!node.move().destination().isTriggering()) {
#ifdef DEBUG_RM
        std::cerr << "\t\t not trigger!" << std::endl;
#endif
        return;
    }

    assert(node.destinationOperationCount() == 1);
    
    unsetOperandsUsed(node.destinationOperation(), cycle);        
    unsetResultWriten(node.destinationOperation(), cycle);
    
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
        int modic = instructionIndex(cycle+i);
        for (unsigned int j = 0 ; j < resources->numberOfResources(); j++) {
            assert(
                fuExecutionPipeline_[modic].size() != 0);
            ResourceReservation& rr = 
                fuExecutionPipeline_[modic][j];
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

#ifdef DEBUG_RM
    std::cerr << "CanAssignDestination called for: " << node.toString() <<
        " Cycle: " << cycle << " PO: " 
              << node.destinationOperation().toString() << std::endl;
    if (triggers) {
        std::cerr << "\tTriggers." << std::endl;
    }
#endif
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

    const TTAMachine::HWOperation& hwop = 
        *fu_.operation(pOp->operation().name());
    TTAMachine::FUPort& port =
        *hwop.port(newNode->move().destination().operationIndex());                   

    if (!operandAllowedAtCycle(port, node, cycle)) {
        return false;
    }
    
    if (operandOverwritten(node, cycle)) {
        return false;
    }

    if (!triggers) {
        if (operandTooLate(node, cycle)) {
#ifdef DEBUG_RM 
        std::cerr << "\t\tOperand too late" << std::endl;
#endif

            return false;
        }
        return true;
    }
   
    if (triggerTooEarly(node, cycle)) {
#ifdef DEBUG_RM 
        std::cerr << "\t\tTrigger too early" << std::endl;
#endif

        return false;
    }

#ifdef DEBUG_RM
    std::cerr << "\tCanAssignDestination is trigger: " << node.toString() <<
        " Cycle: " << cycle << std::endl;
#endif

    // Too late to schedule trigger, results would not be ready in time.
    if (cycle > node.latestTriggerWriteCycle()) {
#ifdef DEBUG_RM 
        std::cerr << "\t\tTrigger too late for results" << std::endl;
#endif
        return false;
    }

    // now we know we have a trigger.
    if (operandsOverwritten(cycle, node)) {
#ifdef DEBUG_RM 
        std::cerr << "\t\tOperands overwritten" << std::endl;
#endif

        return false;
    }

    if (!resourcesAllowTrigger(cycle, node)) {
#ifdef DEBUG_RM 
        std::cerr << "\t\tResources prevent trigger" << std::endl;
#endif

        return false;
    }

    // TODO: if ports have no regs..

    // Test for result read WaW already when scheduling trigger.
    return testTriggerResult(node, cycle);
}

bool ExecutionPipelineResource::operandOverwritten(
    const MoveNode& mn, int cycle) const {
#ifdef DEBUG_RM
    std::cerr << "\t\tTesting operand not overwritten by other ops: " <<
        mn.toString() << " cycle: " << cycle << std::endl;
#endif
    for (unsigned int i = 0; i < mn.destinationOperationCount(); i++) {
        ProgramOperation& po = mn.destinationOperation(i);
        MoveNode* trigger = po.triggeringMove();
        if (trigger == &mn) {
#ifdef DEBUG_RM
            std::cerr << "\t\t\tmn is trigger, no need to check overwrite" << std::endl;
#endif
            return false;
        }
        if (trigger == NULL || !trigger->isScheduled()) {
#ifdef DEBUG_RM
            std::cerr << "\t\t\ttrigger null or not scheduled" << std::endl;
#endif
            continue;
        }

        int triggerCycle = trigger->cycle();

        if (operandOverwritten(cycle, triggerCycle, po, mn, *trigger)) {
            return true;
        }
    }
    return false;
}

bool ExecutionPipelineResource::operandOverwritten(
    int operandWriteCycle,
    int triggerCycle,
    const ProgramOperation& po,
    const MoveNode& operand,
    const MoveNode& trigger) const {

    unsigned int ii = initiationInterval_;
    if (ii < 1) {
        ii = INT_MAX;
    }

    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    int opIndex = operand.move().destination().operationIndex();
    const TTAMachine::Port& port = *hwop.port(opIndex);
    int operandUseCycle = triggerCycle + hwop.slack(opIndex);

    // same op on next loop iteration overwrites?
    if (operandUseCycle - operandWriteCycle >= (int)ii) {
#ifdef DEBUG_RM 
        std::cerr << "\t\tOperand LR over loop iteration(2): " << ii
                  << std::endl;
#endif
        return true;
    }
    
#ifdef DEBUG_RM 
    std::cerr << "\t\t\tTesting port: " << port.name() << std::endl;
#endif
    OperandWriteMap::const_iterator iter = operandsWriten_.find(&port);
    if (iter == operandsWriten_.end()) {
        return false;
    }
    const OperandWriteVector& operandWritten = iter->second;
#ifdef DEBUG_RM 
    
    std::cerr << "\t\t\tOperandWriteCycle: " << operandWriteCycle << std::endl
              << "\t\t\tOperandUseCycle: " << operandUseCycle << std::endl;
#endif

    for (int j = operandWriteCycle; j <= operandUseCycle; j++) {
#ifdef DEBUG_RM
        std::cerr << "\t\t\tTesting if overwritten in cycle: " << j << std::endl;
#endif
        unsigned int modCycle = instructionIndex(j);
        OperandWriteVector::const_iterator owi = operandWritten.find(modCycle);
        if (owi == operandWritten.end()) {
            continue;
        }
        const MoveNodePtrPair& mnpp = owi->second;
        if (mnpp.first != NULL && mnpp.first != &operand &&
            !exclusiveMoves(mnpp.first, &trigger, triggerCycle)) {
            return true;
        }
        if (mnpp.second != NULL && mnpp.second != &operand &&
            !exclusiveMoves(mnpp.second, &trigger, triggerCycle)) {
            return true;
        }
    }
    return false;
}

bool ExecutionPipelineResource::operandsOverwritten(
    int triggerCycle, const MoveNode& trigger) const {
    ProgramOperation &po = trigger.destinationOperation();
#ifdef DEBUG_RM 
    std::cerr << "\t\tTesting op overwrite for: " << po.toString() << " cycle: " << triggerCycle << std::endl;
#endif
    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& inputMove = po.inputMove(i);
        if (&inputMove == &trigger) {
            continue;
        }
        if (!inputMove.isScheduled()) {
            continue;
        }
        int operandWriteCycle = inputMove.cycle();
        if (operandOverwritten(
                operandWriteCycle, triggerCycle, po, inputMove, trigger)) {
            return true;
        }
    }
    return false;
}


bool ExecutionPipelineResource::resourcesAllowTrigger(
    int cycle, const MoveNode& node) const {

    unsigned int ii = initiationInterval();
    if (ii < 1) {
        ii = INT_MAX;
    }

    int modCycle = instructionIndex(cycle);
    std::string opName = "";
    if (node.move().destination().isOpcodeSetting()) {
        opName = node.move().destination().operation().name();
        debugLogRM(opName);
    } else {
        // If target architecture has different opcode setting port
        // as universal machine, pick  a name of operation from a hint
        opName = node.move().destination().hintOperation().name();
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
    
    if (maxCycle_ != INT_MAX) {
        for (unsigned int i = 0; i < resources->maximalLatency() 
                 && canAssign; i++) {
            
            for (unsigned int j = 0 ; j < resources->numberOfResources(); j++) {
                // is this resource needed by this operation?
                if (resources->operationPipeline(pIndex,i,j)) {
                // is the resource free?
                    if (((unsigned int)(cycle + i)) > 
                        (unsigned int)(maxCycle_)) {
                        canAssign = false;
                        break;
                    }
                }
            }
        }    
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
      
        ResourceReservationVector& rrv =
            fuExecutionPipeline_[modci];
        
        if (rrv.size() == 0) {
            continue;
        }
        
        for (unsigned int j = 0 ; j < resources->numberOfResources(); j++) {
            
            ResourceReservation& rr = rrv[j];
            
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
                    fuExecutionPipeline_[instructionIndex(cycle+i)][j];
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
    return canAssign;
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
unsigned int
ExecutionPipelineResource::size() const {

#if 0
    // Breaks for load and store units with internal pipeline resources!!!
    if (cachedSize_ != INT_MIN ) {
        return cachedSize_;
    }
#endif
    int length = fuExecutionPipeline_.size() - 1;
    int dstCount = assignedDestinationNodes_.size();
    int srcCount = assignedSourceNodes_.size();
    // If there are no sources or destinations
    // assigned then the pipeline has to be empty.
    // No point searching whole empty range
    if (length == -1 || (dstCount == 0 && srcCount ==0)) {
        cachedSize_ = 0;
        return 0;
    }
    int stoppingCycle = length;    
    if (dstCount > 0) {
        int dstMin = (*assignedDestinationNodes_.begin()).first;
        stoppingCycle = std::min(stoppingCycle, dstMin);
    }
    if (srcCount > 0) {
        int srcMin = (*assignedSourceNodes_.begin()).first;
        stoppingCycle = std::min(stoppingCycle, srcMin);
    }
    // Don't go bellow smallest known assigned node
    for (int i = length; i >= stoppingCycle; i--) {
	ResourceReservationTable::const_iterator iter =
	    fuExecutionPipeline_.find(i);
	if (iter == fuExecutionPipeline_.end()) {
	    continue;
	} else {
	    const ResourceReservationVector& rrv = iter->second;
	    if (rrv.size() == 0) {
		continue;
	    }

	    for (unsigned int j = 0; j < resources->numberOfResources(); j++) {
		const ResourceReservation& rr = rrv[j];
		if (rr.first != NULL) {
		    cachedSize_ = i + 1;
		    return i + 1;
		}
	    }
        }
    }
    cachedSize_ = 0;
    return 0;
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

        // Find largest cycle where any operand or result was previously
        // scheduled.
        int maximum = 0;
        if (assignedDestinationNodes_.size() > 0) {
            maximum = (*assignedDestinationNodes_.rbegin()).first;
        } else {
            maximum = -1;
        }
    
        int maxResults = 0;
        if (assignedSourceNodes_.size() > 0) {
            maxResults = (*assignedSourceNodes_.rbegin()).first;
        } else {
            maxResults = 0;
        }
        if (maxResults> maximum) {
            maximum = maxResults;
        }
        // size returns count of cycle, max cycle address needs -1
        return std::max(maximum, (int)(size()) - 1);
    } else {
        int highest = -1;
        int min = INT_MAX;        
        if (assignedSourceNodes_.size() > 0) {
            int srcMin = (*assignedSourceNodes_.begin()).first;
            min = std::min(min, srcMin);
        }
        if (assignedDestinationNodes_.size() > 0) {
            int dstMin = (*assignedDestinationNodes_.begin()).first;
            min = std::min(min, dstMin);
        }
        
        for (ResultMap::const_iterator rwi = resultWriten_.begin(); 
             rwi != resultWriten_.end(); rwi++) {
            const ResultVector& resultWriten = rwi->second;
                
            for (int i = resultWriten.size() -1; i >= min; i--) {
                const ResultHelperPair& rhp = 
                    MapTools::valueForKeyNoThrow<ResultHelperPair>(
                        resultWriten,i);
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
            for (int i = resultRead.size() -1; i >= min ; i--) {
                const ResultHelperPair& rrp = 
                    MapTools::valueForKeyNoThrow<ResultHelperPair>(
                        resultRead,i);
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
        
        const ResultHelperPair& rhp = 
            MapTools::valueForKeyNoThrow<ResultHelperPair>(resultWriten,modi);
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
    operandsUsed_.clear();
    operandsWriten_.clear();
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
    const TTAMachine::Port& resultPort,
    const MoveNode& trigger, int triggerCycle) 
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
        const ResultHelperPair& resultReadPair = 
            MapTools::valueForKeyNoThrow<ResultHelperPair>(resultRead, modi);
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
                MoveNode* otherTrigger = 
                    resultReadPair.second.po->triggeringMove();
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
 * Gives the port where from the movenode writes an operand.
 */
const TTAMachine::Port&
ExecutionPipelineResource::operandPort(const MoveNode& mn) const {
    assert(mn.isDestinationOperation());
    const ProgramOperation& po = mn.destinationOperation();
    const TTAMachine::HWOperation& hwop = 
        *fu_.operation(po.operation().name());
    return *hwop.port(mn.move().destination().operationIndex());
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

bool ExecutionPipelineResource::operandAllowedAtCycle(
    const TTAMachine::Port& port, const MoveNode& mn, int cycle) const {
#ifdef DEBUG_RM
    std::cerr << "\t\tTesting " << mn.toString() << " that operand at port: " << port.name() << " allowed at cycle: " << cycle << std::endl;
#endif
    int ii = initiationInterval();
    if (ii < 1) {
        ii = INT_MAX;
    }

    OperandUseMap::const_iterator rui = operandsUsed_.find(&port);
    if (rui == operandsUsed_.end()) {
        return INT_MAX;
    }
    
    const OperandUseVector& operandUsed = rui->second;
    size_t operandUsedSize = operandUsed.size();

    if (!mn.isDestinationOperation()) {
        throw InvalidData(__FILE__, __LINE__, __func__,
            "Trying to get next result for move that is not "
            "in ProgramOperation");
    }
    
    // TODO: this may be very slow if big BB with not much code
    int nextOperandUseCycle = cycle;
    int nextOperandUseModCycle = instructionIndex(nextOperandUseCycle);
    OperandUseVector::const_iterator i = operandUsed.find(nextOperandUseModCycle);

    while(true) {
#ifdef DEBUG_RM
        std::cerr << "\t\t\tTesting use from cycle: " << nextOperandUseCycle << std::endl;
#endif
        if (i != operandUsed.end()) {
#ifdef DEBUG_RM
            std::cerr << "\t\t\t\tcycle " << nextOperandUseCycle << " not empty." << std::endl;
#endif
            const OperandUseHelper& operandUse = i->second.first;
            if (operandUse.po != NULL) {
                const MoveNode& opUseTrigger = 
                    *operandUse.po->triggeringMove();
                if (!exclusiveMoves(&opUseTrigger, &mn, cycle)) {
                    if (!checkOperandAllowed(
                            port, cycle, operandUse, nextOperandUseModCycle)) {
                        return false;
                    }
                    if (opUseTrigger.move().isUnconditional()) {
                        bool allScheduled = true;
                        for (int i = 0; 
                             i < operandUse.po->inputMoveCount(); i++) {
                            if (!operandUse.po->inputMove(i).isScheduled()) {
                                allScheduled = false;
                                break;
                            }
                        }
                        if (allScheduled) {
                            return true;
                        }
                    }
                }
                // conditional moves, second exclusive?
                const OperandUseHelper& operandUse2 = i->second.second;
                if (operandUse2.po != NULL) {
                    const MoveNode& opUseTrigger2 =
                        *operandUse2.po->triggeringMove();
                    if (!exclusiveMoves(&opUseTrigger2, &mn, cycle)) {
                        if (!checkOperandAllowed(
                                port, cycle, operandUse2, nextOperandUseModCycle)) {
                            return false;
                        }
                    }
                }
            }
        }
        nextOperandUseCycle++;

        if (ii == INT_MAX && nextOperandUseCycle >= (int)operandUsedSize) {
            return true;
        }
        
        if (nextOperandUseCycle - cycle >= (int)ii) {
            return true; 
        }
        nextOperandUseModCycle = instructionIndex(nextOperandUseCycle);
        i = operandUsed.find(nextOperandUseModCycle);
    }

    return true;
}

bool ExecutionPipelineResource::checkOperandAllowed(
    const TTAMachine::Port& port, 
    int operandWriteCycle, 
    const OperandUseHelper &operandUse,
    unsigned int operandUseModCycle) const {
    for (int i = 0; i < operandUse.po->inputMoveCount(); i++) {
        MoveNode& mn = operandUse.po->inputMove(i);
        if (mn.isScheduled()) {
            if (&mn.move().destination().port() == &port) {
                // TODO: this is not loop scheduler-aware?

                // fail if the other operand happens eaelier than this (it has later usage).
                
                // loop scheudling, op overlaps
                // need to also check that is not written before the use.
                if (operandUseModCycle <
                    instructionIndex(mn.cycle())) {
                    if (instructionIndex(operandWriteCycle) <=
                        operandUseModCycle ||
                        instructionIndex(mn.cycle()) <= instructionIndex(operandWriteCycle)) {
                        return false;
                    }
                }
                // not overlapping.
                if (instructionIndex(mn.cycle()) <= instructionIndex(operandWriteCycle) &&
                    instructionIndex(operandWriteCycle) <= operandUseModCycle) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * Checks that operand is not scheduled too late(after trigger+slack)
 */
bool ExecutionPipelineResource::operandTooLate(const MoveNode& mn, int cycle) const {
    for (unsigned int i = 0; i < mn.destinationOperationCount(); i++) {
        ProgramOperation& po = mn.destinationOperation(i);
        MoveNode* trigger = po.triggeringMove();
        if (trigger == &mn) {
#ifdef DEBUG_RM
            std::cerr << "\t\t\tmn is trigger, no need to check overwrite" << std::endl;
#endif
            return false;
        }
        if (trigger == NULL || !trigger->isScheduled()) {
            continue;
        }

        const Operation& op = po.operation();
        TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

        int opIndex = mn.move().destination().operationIndex();
        int slack = hwop.slack(opIndex);
        const TTAMachine::FUPort& port = *hwop.port(opIndex);
        if (port.noRegister()) {
            if (cycle != trigger->cycle() + slack) {
                return true;
            }
        } else {
            if (cycle > trigger->cycle() + slack) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Checks that trigger is not scheduled too early(before operand-slack)
 */
bool ExecutionPipelineResource::triggerTooEarly(const MoveNode& trigger, int cycle) const {
    ProgramOperation& po = trigger.destinationOperation(0);
    const Operation& op = po.operation();
    TTAMachine::HWOperation& hwop = *fu_.operation(op.name());

    for (int i = 0; i < po.inputMoveCount(); i++) {
        MoveNode& mn = po.inputMove(i);
        if (mn.isScheduled()) {
            int slack = hwop.slack(mn.move().destination().operationIndex());
            const TTAMachine::Port& port = mn.move().destination().port();
            const TTAMachine::FUPort& fuPort = 
                dynamic_cast<const TTAMachine::FUPort&>(port);
            if (fuPort.noRegister()) {
                if (cycle != mn.cycle() - slack) {
                    return true;
                }
            } else {
                if (cycle < mn.cycle() - slack) {
                    return true;
                }
            }
        }
    }
    return false;
}
