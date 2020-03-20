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
 * @file MoveNode.cc
 *
 * Implementation of MoveNode class.
 *
 * Nodes are the minimum independent unit of information in a
 * minimally-ordered program representation. Typically, but not necessarily,
 * the nodes in a program representation are linked together by dependences
 * and thus form a graph.
 *
 * @author Ari Mets�halme 2006 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <climits>
#include <boost/format.hpp>

#include "MoveNode.hh"
#include "Port.hh"
#include "SpecialRegisterPort.hh"
#include "Bus.hh"
#include "UniversalMachine.hh"
#include "ProgramOperation.hh"
#include "POMDisassembler.hh"
#include "SetTools.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "TCEString.hh"
#include "Guard.hh"
#include "ControlUnit.hh"
#include "Terminal.hh"
#include "MoveGuard.hh"
#include "Operation.hh"
#include "Move.hh"
#include "Conversion.hh"

using namespace TTAMachine;

/**
 * Constructor.
 *
 * Creates a new node with Move.
 *
 * @param newmove the Move this node contains.
 */
MoveNode::MoveNode(std::shared_ptr<TTAProgram::Move> newmove) :
    move_(newmove), cycle_(0),  placed_(false), finalized_(false),
    isInFrontier_(false) {
}


/**
 * Constructor.
 *
 * Creates a new node to be entry node.
 *
 */

MoveNode::MoveNode() :
    move_(NULL), cycle_(0), placed_(false), finalized_(false),
    isInFrontier_(false) {
}

/**
 * Destructor.
 *
 * Does not unregister this movenode from ProgramOperations.
 */
MoveNode::~MoveNode() {

    if (isSourceOperation()) {
        sourceOperation().removeOutputNode(*this);
    }
    if (isDestinationOperation()) {
        for (unsigned int i = 0; i < destinationOperationCount(); i++) {
            destinationOperation(i).removeInputNode(*this);
        }
    }
}

/**
 * Creates a deep copy of MoveNode.
 *
 * Sets the source and destination operation of the copy to the same as
 * original. Does not copy the cycle.
 *
 * @return return copy of MoveNode.
 */
MoveNode*
MoveNode::copy() {

    MoveNode* newNode = NULL;
    if (move_ != NULL) {
        newNode = new MoveNode(move_->copy());
    } else {
        newNode = new MoveNode;
    }

    if (isSourceOperation()) {
        sourceOperation().addOutputNode(*newNode);
        newNode->setSourceOperationPtr(sourceOperationPtr());
    }

    if (isDestinationOperation()) {
        destinationOperation().addInputNode(*newNode);
        newNode->addDestinationOperationPtr(destinationOperationPtr());
    }
    return newNode;
}

/**
 * Tells whether the source of the MoveNode (move) belongs to an operation.
 *
 * @return True if the source of the MoveNode is an operation output.
 */
bool
MoveNode::isSourceOperation() const {
    if (move_ == NULL) {
        return false;
    }
    return srcOp_.get() != NULL;
}


/**
 * Tells whether the node (move) reads a program variable or, if assigned, a
 * GPR.
 *
 * @return True if the source of the node is a variable or GPR.
 */
bool
MoveNode::isSourceVariable() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isGPR();
}

/**
 * Tells whether the node (move) reads the return address port.
 * GPR.
 *
 * @return True if the source of the node is the return address port.
 */
bool
MoveNode::isSourceRA() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isRA();
}

/**
 * Tells whether the node (move) reads a Immediate Register
 *
 * @return True if the source of the node is Immediate register.
 */
bool
MoveNode::isSourceImmediateRegister() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isImmediateRegister();
}


/**
 * Tells whether the source of the node (move) is a program constant. If
 * assigned, the constant is an in-line immediate.
 *
 * @return True if the source of the node is a constant.
 */
bool
MoveNode::isSourceConstant() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->source().isImmediate();
}



/**
 * Tells whether the move belongs to an operation execution.
 *
 * @return True if the the node belongs to an operation.
 */
bool
MoveNode::isOperationMove() const {
    return isSourceOperation() || isDestinationOperation();
}

/**
 * Tells whether the node (move) writes a program variable or, if assigned,
 * a GPR.
 *
 * @return True if the destination of the node is a variable or GPR.
 */
bool
MoveNode::isDestinationVariable() const {
    if (move_ == NULL) {
        return false;
    }
    return move_->destination().isGPR();
}


/**
 * Tells whether the node is a ``software bypass'' - both its source and its
 * destination belong to operations.
 *
 * @return True if both source and destination of the node are operation
 *     terminals.
 */
bool
MoveNode::isBypass() const {
    return isSourceOperation() && isDestinationOperation();
}

/**
 * Tells whether the node is a register move, thus not belong to
 * any particular ProgramOperation.
 *
 * It's either an immediate move to an register or a register to register
 * copy.
 *
 * @return True if the move is a register to register copy.
 */
bool
MoveNode::isRegisterMove() const {
    return !isSourceOperation() && !isDestinationOperation();
}


/**
 * Returns true if this MoveNode is in the same ProgramOperation as the
 * given MoveNode.
 *
 * @return True if both MoveNodes belong to the same ProgramOperation.
 */
bool
MoveNode::inSameOperation(const MoveNode& other) const {

    if (other.isRegisterMove() || this->isRegisterMove())
        return false;

    // due to bypass moves we have to consider both ends of each moves
    // separately.. there has to be quicker way to check this ;-)
    std::set<ProgramOperation*> operationsA;
    std::set<ProgramOperation*> operationsB;

    if (this->isSourceOperation())
        operationsA.insert(&this->sourceOperation());

    if (this->isDestinationOperation())
        operationsA.insert(&this->destinationOperation());

    if (other.isSourceOperation())
        operationsB.insert(&other.sourceOperation());

    if (other.isDestinationOperation())
        operationsB.insert(&other.destinationOperation());

    std::set<ProgramOperation*> commonOperations;

    SetTools::intersection(operationsA, operationsB, commonOperations);
    return commonOperations.size() > 0;
}


/**
 * Tells whether is placed in the program representation, that is, has a
 * cycle assigned to it.
 *
 * @return True if a cycle is assigned to the node.
 */
bool
MoveNode::isPlaced() const {
    return placed_;
}


/**
 * Tells whether the node is fully assigned.
 *
 * A node is fully assigned when all the resources of the target processor
 * necessary to carry out the transport it specifies are assigned to it.
 *
 * @return True if all required scheduling resources of target machine are
 *      assigned to the node.
 */
bool
MoveNode::isAssigned() const {

    if (move_ == NULL) {
        // probably a dummy ENTRYNODE or something
        return false;
    }

    /// Machine found is NOT UniversalMachine - we are happy
    if (!move_->bus().machine()->isUniversalMachine()) {
        if ((isSourceOperation() ||
            isSourceVariable() ||
            isSourceImmediateRegister()) &&
            (move_->source().port().parentUnit()->machine() !=
             move_->bus().machine())) {
            return false;
        }
        if ((isDestinationOperation() || isDestinationVariable()) &&
            (move_->destination().port().parentUnit()->machine() !=
             move_->bus().machine())) {
            return false;
        }
        return true;
    }
    return false;
}

/**
 * Tells whether the node is completely scheduled.
 *
 * A node is completely scheduled only if it is assigned to a program cycle
 * and the necessary resources of the target processor are assigned to it.
 *
 * @return True if the node is placed and assigned.
 */
bool
MoveNode::isScheduled() const {
    return isPlaced() && isAssigned();
}


/**
 * Returns the cycle (index) assigned to the node.
 *
 * @return The cycle in which the node is placed.
 * @exception InvalidData if the node is not placed.
 */
int
MoveNode::cycle() const {
    if (!isPlaced()){
        std::string msg = "MoveNode was not placed yet: ";
        if (isMove()) {
            msg+= POMDisassembler::disassemble(*move_);
        } else {
            msg+= "Node does not contain a move.";
        }
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    } else {
        return cycle_;
    }
}

/**
 * Returns the enclosing scheduling scope of the node.
 *
 * @return A scheduling scope.
 */
Scope&
MoveNode::scope(){
    ///TODO: Intentionally falsified code
    /// Class Scope does not exists so far in real
    Scope* ns = new Scope;
    return *ns;
}


/**
 * Returns the instance of operation in the program whose output is the
 * source of this node.
 *
 * @return A program operation.
 * @exception InvalidData if the given node does not read an operation
 *     output.
 */
ProgramOperation&
MoveNode::sourceOperation() const {
    return *sourceOperationPtr().get();
}

ProgramOperationPtr
MoveNode::sourceOperationPtr() const {
    if (!isSourceOperation()){
        std::string msg =
            (boost::format(
                "MoveNode: '%s' source is not Operation.") % toString()).
            str();
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    } else {
        return srcOp_;
    }
}

/**
 * Set cycle for a node, also sets placed_
 * @param newcycle Cycle to which node is placed_
 * @throw InvalidData If node is already placed in cycle different from
 *                      newcycle
 */
void
MoveNode::setCycle(const int newcycle) {
    if (placed_ == true && cycle_ != newcycle) {
        std::string msg = "MoveNode is already placed in cycle ";
        msg += cycle_;
        msg += ".";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    cycle_ = newcycle;
    placed_ = true;
}

/**
 * Unset cycle from nodes
 * @throw InvalidData If node is not placed
 */
void
MoveNode::unsetCycle() {
    if (placed_ == false ) {
        std::string msg = "MoveNode is not placed.";
        throw InvalidData(__FILE__, __LINE__, __func__, msg);
    }
    cycle_ = 0;
    placed_ = false;
}

/**
 * Set a destination of MoveNode to ProgramOperation
 *
 * @param po Program operation that is destination of MoveNode
 */
void MoveNode::addDestinationOperationPtr(ProgramOperationPtr po) {
    dstOps_.push_back(po);
}
/**
 * Set a source of MoveNode to ProgramOperation
 *
 * @param po Program operation that is source of MoveNode
 */
void MoveNode::setSourceOperationPtr(ProgramOperationPtr po) {
    srcOp_ = po;
}

/**
 * Returns type of the MoveNode.
 *
 * Not yet used anywhere and types not decided so
 * current dummy implementation returns -1
 *
 * @return type of the node
 */
int
MoveNode::type() {
    return -1;
}

/**
 * Returns string with ID of the MoveNode.
 *
 * Not yet used anywhere except printing graph in .dot file. Returns the
 * disassembly of the move along with its id.
 *
 * @return The string with node ID.
 */
std::string
MoveNode::toString() const {
    if (move_ == NULL) {
        return "-1:\tENTRYNODE";
    }
    std::string content = 
        (isPlaced() ? Conversion::toString(cycle()) + " " : 
         std::string()) + POMDisassembler::disassemble(*move_);
    return content;
}

/**
 * Returns Dot representation of the node.
 *
 * Prints the disassembly of the move and sets the color of the node to red
 * in case it's scheduled.
 *
 * @return The string with node ID.
 */
std::string
MoveNode::dotString() const {

    std::string contents = GraphNode::dotString();
    if (isOperationMove()) {
        unsigned operationId = 0;

        // make the outline of the moves that belong to the same operation
        // of the same color to aid in schedule debugging
        if (isSourceOperation())
            operationId = sourceOperation().poId();
        else
            operationId = destinationOperation().poId();

        // hash the colors so that they are easy to separate/recognise.
        // srand();rand() pair only used as a hash function,
        // this needs to be deterministic.
        srand(operationId);
        int operationColor = rand() / (RAND_MAX>>24);

        contents +=
            (boost::format(
                ",color=\"#%.6x\"") % operationColor).str();
    }

    if (isScheduled()) {
        contents += ",shape=box";
    } else {
        contents += ",shape=ellipse";
    }
    return contents;
}

/**
 * Returns the cycle the given result move can be scheduled earliest,
 * taking in the account the latency of the operation.
 *
 * In case the trigger move has not been scheduled yet, returns INT_MAX.
 *
 * @exception IllegalObject if this MoveNode is not a result read.
 */
int
MoveNode::earliestResultReadCycle() const {

    if (!isSourceOperation())
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, "Not a result read move.");

    const ProgramOperation& po = sourceOperation();

    try {
        MoveNode* trigger = po.triggeringMove();
        if (trigger == NULL || !trigger->isScheduled()) {
            return INT_MAX;
        }

        // find the latency of the operation output we are reading
        const TTAMachine::HWOperation& hwop =
            *trigger->move().destination().functionUnit().operation(
                po.operation().name());

        // find the OSAL id of the operand of the output we are reading
        const int outputIndex = move_->source().operationIndex();
        return trigger->cycle() + hwop.latency(outputIndex);
    } catch (const InvalidData& id) {
        // triggeringMove() throws if the triggering move cannot be resolved
        // again ignore this. causd by either
        // incorrect scheduling order ( in RM tests) or
        // broken machine ( catched by machinecheck now)
    } catch (const Exception& e) {
        abortWithError(e.errorMessageStack());
    }
    return INT_MAX;
}
/**
 * Returns the lates cycle the given trigger move can be scheduled at,
 * taking in the account the latency of the operation results.
 *
 * In case the none of the result moves has been scheduled yet, returns INT_MAX.
 *
 * @exception IllegalObject if this MoveNode is not a result read.
 */
int
MoveNode::latestTriggerWriteCycle() const {

    if (!isDestinationOperation())
        throw IllegalParameters(
            __FILE__, __LINE__, __func__, "Not a result read move.");

    const ProgramOperation& po = destinationOperation();
    int latestTrigger = INT_MAX;
    for (int i = 0; i < po.outputMoveCount(); i++){ 
        MoveNode& result = po.outputMove(i);
        if (!result.isScheduled()) {
            continue;
        }
        // find the latency of the operation output we are testing
        const TTAMachine::HWOperation& hwop =
            *result.move().source().functionUnit().operation(
                po.operation().name());
        // find the OSAL id of the operand of the output we are testing
        const int outputIndex = result.move().source().operationIndex();
        int latency = hwop.latency(outputIndex);
        latestTrigger = std::min(latestTrigger, result.cycle() - latency);
    }
    return latestTrigger;
}

/**
 * Unsets destination operation.
 *
 * Does not ask the ProgramOperation to remove this MoveNode from
 * it's input moves.
 */
void
MoveNode::clearDestinationOperation() {
    dstOps_.clear();
}

/**
 * Unsets destination operation.
 *
 * Does not ask the ProgramOperation to remove this MoveNode from
 * it's input moves.
 */
void
MoveNode::removeDestinationOperation(const ProgramOperation* ptr) {
    for (std::vector<ProgramOperationPtr>::iterator i = dstOps_.begin();
         i != dstOps_.end(); i++) {
        if (((*i).get()) == ptr) {
            dstOps_.erase(i);
            return;
        }
    }
    std::string msg = "Removed destination op not found in MoveNode";
    throw InvalidData(__FILE__, __LINE__, __func__, msg);
}

/**
 * Unsets source operation.
 *
 * Does not ask the ProgramOperation to remove this MoveNode from
 * it's output moves.
 */
void
MoveNode::unsetSourceOperation() {
    srcOp_ = ProgramOperationPtr();
}

/**
 * Returns the total guard latency of the guard of given move,
 * or 0 if the move is unconditional.
 */
int MoveNode::guardLatency() const {
    if (!isMove()) {
        return 0;
    }
    return move_->guardLatency();
}

/**
 * Checks if the source of the movenode is the given reg.
 *
 * This method assumes incoming reg name is in correct
 * rf.number format, and does not check it, due performance reasons.
 *
 * @param reg register to check against source of the movenode
 *
 * @return true if the source of the movenode is the given reg.
 *         false if some other reg or not reg.
 */
bool
MoveNode::isSourceReg(const std::string& reg) const {
    if (!isMove()) {
        return false;
    }
    if (!move().source().isGPR()) {
        return false;
    }

    // try to do as quickly as possible,
    // compare the reg in string and reg in terminalregister.
    size_t dotPlace = reg.find('.');
    const std::string& rfName = move().source().registerFile().name();
    if (reg.compare(0, dotPlace, rfName) != 0) {
        return false;
    }

    return atoi(reg.c_str()+dotPlace+1) == move().source().index();
}

bool MoveNode::isLastUnscheduledMoveOfDstOp() const {
    for (unsigned int i = 0; i < destinationOperationCount(); i++) {
        const ProgramOperation& po = destinationOperation(i);
        // ignore ops with just one input
        if (po.inputMoveCount() == 1) {
            continue;
        }
        bool fail = false;
        for (int j = 0; j < po.inputMoveCount(); j++) {
            MoveNode& inputNode = po.inputMove(j);
            if (&inputNode != this && !inputNode.isScheduled()) {
                fail = true;
                break;
            }
        }
        if (!fail)
            return true;
    }
    return false;
}
