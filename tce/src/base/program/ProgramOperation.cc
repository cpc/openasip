/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file ProgramOperation.cc
 *
 * Implementation of ProgramOperation class.
 *
 * ProgramOperation represents an instance of operation execution in
 * the program and should hold the reference to each MoveNode that belongs
 * to the operation execution instance.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "ProgramOperation.hh"
#include "MoveNode.hh"
#include "MoveNodeSet.hh"
#include "Operation.hh"
#include "MapTools.hh"
#include "TerminalFUPort.hh"
#include "AssocTools.hh"
#include "ContainerTools.hh"
#include "Move.hh"
#include "Terminal.hh"
#include "BaseFUPort.hh"
#include "FunctionUnit.hh"
#include "TCEString.hh"
#include "FUPort.hh"
#include "HWOperation.hh"
#include "Guard.hh"
#include "MoveGuard.hh"

namespace TTAProgram{
    class NullOperation;
}
/**
 * Constructor.
 *
 * Creates a ProgramOperation from operation
 * @param operation Operation
 */
ProgramOperation::ProgramOperation(
	const Operation &operation,
    const llvm::MachineInstr* instr) :
    operation_(operation), poId_(idCounter++), mInstr_(instr) {
}

/**
 * Constructor.
 *
 * Creates a ProgramOperation from NullOperation
 */
 ///TODO: this better go, just for testing with empty operation...
ProgramOperation::ProgramOperation() :
    operation_(NullOperation::instance()), poId_(idCounter++) {
    inputMoves_.clear();
    outputMoves_.clear();
}
/**
 * Destructor.
 *
 * Deletes MoveNodeSets.
 * Does not unregister these program operations from movenodes.
 */
ProgramOperation::~ProgramOperation() {
    // If MoveNodes of PO are not destroyed before PO they need to unset
    // their source or destination operation respectively
    for (int i = 0; i < inputMoveCount(); i++) {
        inputMove(i).removeDestinationOperation(this);
    }
    for (int i = 0; i < outputMoveCount(); i++) {
        outputMove(i).unsetSourceOperation();
    }       
    AssocTools::deleteAllValues(inputMoves_);
    AssocTools::deleteAllValues(outputMoves_);
}

/**
 * Add given node to the set of nodes that belong to this program
 * operation.
 *
 * Warning: this should not be used with bypassed moves!
 *
 * Add node to nodes of program operation
 * @param node MoveNode to add to operation
 */
void
ProgramOperation::addNode(MoveNode& node) {
    if (node.move().source().isFUPort()) {
        if (node.move().source().hintOperation().name() ==
            operation().name()) {
            addOutputNode(node);
        }
    } else {
        if (node.move().destination().isFUPort()) {
            if (node.move().destination().hintOperation().name() ==
                operation().name()) {
                addInputNode(node);
            }
        }
    }
}

/**
 * Add given node to the set of input nodes that belong to this program
 * operation.
 *
 * Input node is a move that writes an operand of the operation.
 *
 * @param node MoveNode to add to operation
 */
void
ProgramOperation::addInputNode(MoveNode& node) {
    int inputIndex = node.move().destination().operationIndex();
    if (MapTools::containsKey(inputMoves_, inputIndex)) {
        MoveNodeSet* nodeSet =
            MapTools::valueForKey<MoveNodeSet*>(inputMoves_, inputIndex);
        nodeSet->addMoveNode(node);
        allInputMoves_.push_back(&node);
    } else {
        MoveNodeSet* nodeSet = new MoveNodeSet;
        nodeSet->addMoveNode(node);
        inputMoves_[inputIndex] = nodeSet;
        allInputMoves_.push_back(&node);
    }
}

/**
 * Add given node to the set of nodes that belong to this program operation.
 *
 * Add node to nodes of program operation
 * @param node MoveNode to add to operation
 */
void
ProgramOperation::addOutputNode(MoveNode& node) {
    int outputIndex = node.move().source().operationIndex();
    if (MapTools::containsKey(outputMoves_, outputIndex)) {
        MoveNodeSet* nodeSet =
            MapTools::valueForKey<MoveNodeSet*>(outputMoves_,outputIndex);
        nodeSet->addMoveNode(node);
        allOutputMoves_.push_back(&node);
    } else {
        MoveNodeSet* nodeSet = new MoveNodeSet;
        nodeSet->addMoveNode(node);
        allOutputMoves_.push_back(&node);
        outputMoves_[outputIndex] = nodeSet;
    }
}

/**
 * Removes output node from set of nodes that belong to this program operation.
 *
 * @param node MoveNode being removed from this program operation
 */
void
ProgramOperation::removeOutputNode(MoveNode& node) {
    int outputIndex = node.move().source().operationIndex();
    if (MapTools::containsKey(outputMoves_,outputIndex)) {
        MoveNodeSet* nodeSet =
            MapTools::valueForKey<MoveNodeSet*>(outputMoves_,outputIndex);
        nodeSet->removeMoveNode(node);
        for (std::vector<MoveNode*>::iterator i = allOutputMoves_.begin();
             i != allOutputMoves_.end(); i++) {
            if (*i == &node) {
                allOutputMoves_.erase(i);
                break;
            }
        }
    } else {
        throw IllegalRegistration(
            __FILE__,__LINE__,__func__,"MoveNode not part of PO");
    }
}

/**
 * Removes output node from set of nodes that belong to this program operation.
 *
 * @param node MoveNode being removed from this program operation
 */
void
ProgramOperation::removeInputNode(MoveNode& node) {
    int inputIndex = node.move().destination().operationIndex();
    if (MapTools::containsKey(inputMoves_,inputIndex)) {
        MoveNodeSet* nodeSet =
            MapTools::valueForKey<MoveNodeSet*>(inputMoves_,inputIndex);
        nodeSet->removeMoveNode(node);
        for (std::vector<MoveNode*>::iterator i = allInputMoves_.begin();
             i != allInputMoves_.end(); i++) {
            if (*i == &node) {
                allInputMoves_.erase(i);
                break;
            }
        }
    } else {
        throw IllegalRegistration(
            __FILE__,__LINE__,__func__,"MoveNode not part of PO");
    }
}

/**
 * Return true if each input and each output of the operation has a
 * corresponding node reference in this program operation
 *
 * @return True if each input and output of operation has node reference
 */
bool
ProgramOperation::isComplete() {
    if (!isReady()) {
        return false;
    }
    // numbering of output does not start from 1 but input count +1
    int begin = operation_.numberOfInputs() + 1;
    int end =  operation_.numberOfInputs() + operation_.numberOfOutputs();
    for (int i = begin; i <= end; i++) {
        if (!MapTools::containsKey(outputMoves_,i)) {
            return false;
        }
    }
    return true;
}

/**
 * Return true if each input node has corresponding node reference
 *
 * @return True if each input of operation has corresponding node reference
 */
bool
ProgramOperation::isReady() {
    for (int i = 1; i <= operation_.numberOfInputs(); i++) {
        if (!MapTools::containsKey(inputMoves_,i)) {
            return false;
        }
    }
    return true;
}

/**
 * Return true if this program operation has more then one reference for some
 * input or output of the operation.
 *
 * @return True if some of inputs or outputs has more then one reference
 */
bool
ProgramOperation::isMultinode() {
    std::map<int,MoveNodeSet*>::iterator moveIt = inputMoves_.begin();
    while (moveIt != inputMoves_.end()) {
        if ((*moveIt).second->count() > 1) {
            return true;
        }
        moveIt++;
    }
    moveIt = outputMoves_.begin();
    while (moveIt != outputMoves_.end()) {
        if ((*moveIt).second->count() > 1) {
            return true;
        }
        moveIt++;
    }
    return false;
}

/**
 * Return true if nodes of this operation have function unit assigned to
 * their source or destination respectively
 *
 * @return True if nodes have source or destination function unit assigned
 */
bool
ProgramOperation::isAssigned() {
    for (int i = 0; i < inputMoveCount(); i++ ) {
        if (!inputMove(i).isAssigned()) {
            return false;
        }
    }
    for (int i = 0; i < outputMoveCount(); i++ ) {
        if (!outputMove(i).isAssigned()) {
            return false;
        }
    }
    return true;
}

/**
 * Return true if any node of this operation have function unit assigned to
 * their source or destination respectively
 *
 * @return True if any node has source or destination function unit assigned
 */
bool
ProgramOperation::isAnyNodeAssigned() {
    for (int i = 0; i < inputMoveCount(); i++ ) {
        if (inputMove(i).isAssigned()) {
            return true;
        }
    }
    for (int i = 0; i < outputMoveCount(); i++ ) {
        if (outputMove(i).isAssigned()) {
            return true;
        }
    }
    return false;
}

/**
 * Return true if any node of this operation have function unit assigned to
 * their source or destination respectively
 *
 * @return True if any node has source or destination function unit assigned
 */
bool
ProgramOperation::isAnyInputAssigned() {
    for (int i = 0; i < inputMoveCount(); i++ ) {
        if (inputMove(i).isAssigned()) {
            return true;
        }
    }
    return false;
}

/**
 * Return true if nodes of this operation have function unit assigned to
 * their source or destination respectively
 *
 * @return True if nodes have source or destination function unit assigned
 */
bool
ProgramOperation::areInputsAssigned() {
    for (int i = 0; i < inputMoveCount(); i++ ) {
        if (!inputMove(i).isAssigned()) {
            return false;
        }
    }
    return true;
}

/**
 * Return true if any output node of this operation have function unit assigned to
 * their source or destination respectively
 *
 * @return True if any node has source or destination function unit assigned
 */
bool
ProgramOperation::isAnyOutputAssigned() {
    for (int i = 0; i < outputMoveCount(); i++ ) {
        if (outputMove(i).isAssigned()) {
            return true;
        }
    }
    return false;
}

/**
 * Return true if nodes of this operation have function unit assigned to
 * their source or destination respectively
 *
 * @return True if nodes have source or destination function unit assigned
 */
bool
ProgramOperation::areOutputsAssigned() {
    for (int i = 0; i < outputMoveCount(); i++ ) {
        if (!outputMove(i).isAssigned()) {
            return false;
        }
    }
    return true;
}

/**
 * Return the MoveNode of program operation that sets the opcode and triggers
 * operation execution.
 *
 * Such a MoveNode is usually one of input nodes and can not have multiple
 * copies.
 * @return MoveNode that sets opcode and triggers execution
 * @throw InvalidData In case there is no opcode setting node register in
 * operation
 */
MoveNode&
ProgramOperation::opcodeSettingNode() {
    std::map<int,MoveNodeSet*>::iterator moveIt = inputMoves_.begin();
    while (moveIt != inputMoves_.end()) {
        if ((*moveIt).second->at(0).move().destination().isOpcodeSetting()) {
            if ((*moveIt).second->count() > 1) {
                std::string msg = "ProgramOperation has more then one opcode\
                    setting node.";
                throw InvalidData(__FILE__, __LINE__, __func__, msg);
            }
            return (*moveIt).second->at(0);
        }
        moveIt++;
    }
    std::string msg = "ProgramOperation is missing opcode setting node.";
    throw InvalidData(__FILE__, __LINE__, __func__, msg);
}

/**
 * Return the nodes that writes input Index of operation
 *
 * @param index Index of a operation to test
 * @return Set of nodes that writes particular input of operation
 * @throw OutOfRange In case the index is higher then number of operation
 * inputs
 */
MoveNodeSet&
ProgramOperation::inputNode(int index) const {
    if (index < 1 || index > operation_.numberOfInputs()) {
        std::string msg = "InputNode index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }
    
    std::map<int, MoveNodeSet*>::const_iterator i = inputMoves_.find(index);
    if (i != inputMoves_.end()) {
        return *(i->second);
    } else {
        std::string msg = "InputNode index not found.";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
}
/**
 * Return the nodes that writes output Index of operation
 *
 * @param index Index of a operation to test
 * @return Set of nodes that writes particular output of operation
 * @throw OutOfRange If index is not found
 */
MoveNodeSet&
ProgramOperation::outputNode(int index) const {
    if (index <= operation_.numberOfInputs() ||
        index > operation_.numberOfInputs()+
        operation_.numberOfOutputs()) {
        std::string msg = "OutputNode index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    if (MapTools::containsKey(outputMoves_, index)) {
        MoveNodeSet* nodeSet =
                MapTools::valueForKey<MoveNodeSet*>(outputMoves_, index);
        return *nodeSet;
    } else {
        std::string msg = "OutputNode index not found.";
        throw KeyNotFound(__FILE__, __LINE__, __func__, msg);
    }
}
/**
 * Return true if program operation has node registered for input identified
 * by index.
 *
 * @param Index Operation input to test
 * @return True if the operation input Index has node registered
 */
bool
ProgramOperation::hasInputNode(int index) const {
    if (MapTools::containsKey(inputMoves_,index)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Return true if program operation has node registered for output identified
 * by index.
 *
 * @param Index Operation output to test
 * @return True if the operation output Index has node registered
 */
bool
ProgramOperation::hasOutputNode(int index) const {
    if (MapTools::containsKey(outputMoves_,index)) {
        return true;
    } else {
        return false;
    }
}

/**
 * Return the operation from OSAL
 *
 * @return Return the operation object handle from OSAL
 */
const Operation&
ProgramOperation::operation() const {
    return operation_;
}

/**
 * Count of input moves to this operation.
 *
 * @return The count of input moves.
 */
int
ProgramOperation::inputMoveCount() const {
    return allInputMoves_.size();
}

/**
 * Count of output moves to this operation.
 *
 * @return The count of output moves.
 */
int
ProgramOperation::outputMoveCount() const {
    return allOutputMoves_.size();
}

/**
 * Returns the given input move.
 *
 * @param index Index of the move.
 * @return The input move.
 */
MoveNode&
ProgramOperation::inputMove(int index) const {
    return *allInputMoves_.at(index);
}

/**
 * Returns the given output move.
 *
 * @param index Index of the move.
 * @return The output move.
 */
MoveNode&
ProgramOperation::outputMove(int index) const {
    return *allOutputMoves_.at(index);
}

/**
 * Returns the triggering move of the operation.
 *
 * @return The triggering move. NULL in case the operation is not yet 
 * assigned to an FU, or the triggering move has not been assigned yet.
 */
MoveNode*
ProgramOperation::triggeringMove() const {

    for (std::size_t i = 0; i < allInputMoves_.size(); ++i) {
        MoveNode& moveNode = *allInputMoves_.at(i);
        if (moveNode.isScheduled()) {
            const TTAProgram::Move& move = moveNode.move();
            if (move.destination().isFUPort() &&
                (dynamic_cast<const TTAMachine::BaseFUPort&>(
                    move.destination().port())).isTriggering())
                return &moveNode;
        }
    }
    return NULL;
}

MoveNode*
ProgramOperation::findTriggerFromUnit(
    const TTAMachine::Unit& unit) const {
    const TTAMachine::FunctionUnit* fu =
    dynamic_cast<const TTAMachine::FunctionUnit*>(&unit);
    int ioIndex = -1;
    assert(fu);

    int portC = fu->portCount();
    for (int i = 0; i < portC; i++) {
        auto p = fu->port(i);
        const TTAMachine::FUPort* port = dynamic_cast<const TTAMachine::FUPort*>(p);
        if (port != nullptr && port->isTriggering()) {
            const TTAMachine::HWOperation* hwop =
            fu->operation(operation().name());
            ioIndex = hwop->io(*port);
            auto ngi = inputMoves_.find(ioIndex);
            if (ngi == inputMoves_.end()) {
                return NULL;
            }
            auto mng = *ngi->second;
            if (mng.count() < 1)
                return NULL;
            return &mng.at(0);
        }
    }
    return NULL;
}

MoveNode&
ProgramOperation::moveNode(const TTAProgram::Move& move) const {

    for (std::size_t i = 0; i < allInputMoves_.size(); ++i) {
        MoveNode& moveNode = *allInputMoves_.at(i);
        if (&moveNode.move() == &move)
            return moveNode;
    }
    for (std::size_t i = 0; i < allOutputMoves_.size(); ++i) {
        MoveNode& moveNode = *allOutputMoves_.at(i);
        if (&moveNode.move() == &move)
            return moveNode;
    }
    throw KeyNotFound(__FILE__, __LINE__, __func__, 
                      "No MoveNode for the Move found in the ProgramOperation.");
}

bool
ProgramOperation::hasMoveNodeForMove(const TTAProgram::Move& move) 
    const {

    for (std::size_t i = 0; i < allInputMoves_.size(); ++i) {
        MoveNode& moveNode = *allInputMoves_.at(i);
        if (&moveNode.move() == &move)
            return true;
    }
    for (std::size_t i = 0; i < allOutputMoves_.size(); ++i) {
        MoveNode& moveNode = *allOutputMoves_.at(i);
        if (&moveNode.move() == &move)
            return true;
    }
    return false;
}


/**
 * The moves of the program operation in human readable format.
 *
 * @return The program operation as a string.
 */
std::string
ProgramOperation::toString() const {
    std::string output = "";
    for (std::size_t i = 0; i < allInputMoves_.size(); ++i) {
        output += allInputMoves_.at(i)->toString() + " ";
    }
    for (std::size_t i = 0; i < allOutputMoves_.size(); ++i) {
        output += allOutputMoves_.at(i)->toString() + " ";
    }
    return output;
}

/**
 * Returns an unique id of this program operation.
 *
 * These can be used instead of pointers for comparison,
 * and are more deterministic as they are given in order.
 */
unsigned int ProgramOperation::poId() const {
    return poId_;
}

/**
 * Comparison based on ID's for maps and sets.
 */
bool 
ProgramOperation::Comparator::operator()(
    const ProgramOperation* po1, const ProgramOperation* po2) const {
    if (po1 == NULL) {
        return false;
    } 
    if (po2 == NULL) {
        return true;
    }
    return po1->poId() < po2->poId();
}

bool
ProgramOperation::Comparator::operator()(
    const ProgramOperationPtr &po1, const ProgramOperationPtr &po2) const {
    return po1.get()->poId() < po2.get()->poId();
}


/**
 * Switches inputs of 2-operand commutative operations
 */
void
ProgramOperation::switchInputs(int idx1, int idx2) {

    const Operation& op = operation();
    assert (op.numberOfInputs() >= 2);
    
    // switch input nodes.
    MoveNodeSet tmpSet = *inputMoves_[idx1];
    *inputMoves_[idx1] = *inputMoves_[idx2];
    *inputMoves_[idx2] = tmpSet;

    for (int i = 0; i < inputMoveCount(); i++) {
        MoveNode& node = inputMove(i);
        TTAProgram::Move& move = node.move();
        TTAProgram::Terminal& oldDest = move.destination();
        
        // then update the moves.
        if (move.destination().operationIndex() == idx1) {
            move.setDestination(
                new TTAProgram::TerminalFUPort(
                    *static_cast<TTAProgram::TerminalFUPort&>(oldDest).
                    hwOperation(), idx2));
            
        } else if (move.destination().operationIndex() == idx2) {
            move.setDestination(
                new TTAProgram::TerminalFUPort(
                    *static_cast<TTAProgram::TerminalFUPort&>(oldDest).
                    hwOperation(), idx1));
        }
    }
}

/**
 * Returns true in case the operation has at least one operand that
 * is a contant/immediate.
 */
bool
ProgramOperation::hasConstantOperand() const {
    for (MoveVector::const_iterator i = allInputMoves_.begin(); 
         i != allInputMoves_.end(); ++i) {
        MoveNode* mn = *i;
        if (mn->isSourceConstant())
            return true;
    }
    return false;
}

unsigned int ProgramOperation::idCounter = 0;

const TTAMachine::FunctionUnit*
ProgramOperation::scheduledFU() const {

    for (int i = 0; i < inputMoveCount(); i++ ) {
        MoveNode& inputNode = inputMove(i);
        if (inputNode.isAssigned()) {
            return static_cast<const TTAMachine::FunctionUnit*>(
                inputNode.move().destination().port().parentUnit());
        }
    }
    for (int i = 0; i < outputMoveCount(); i++ ) {
        MoveNode& outputNode = outputMove(i);
        if (outputNode.isAssigned()) {
            auto& source = outputNode.move().source();
            assert(source.isFUPort());
            return &source.functionUnit();
        }
    }
    return nullptr;
}

const TTAMachine::HWOperation* ProgramOperation::hwopFromOutMove(
    const MoveNode& outputNode) const {
    if (outputNode.isSourceOperation() &&
        &outputNode.sourceOperation() == this) {
        return outputNode.move().source().functionUnit().operation(
            operation().name());
    } else {
        const TTAMachine::Guard& guard =
            outputNode.move().guard().guard();
        const TTAMachine::PortGuard* pg =
            dynamic_cast<const TTAMachine::PortGuard*>(&guard);
        assert(pg); // todo: throw?
        const TTAMachine::Port* p = pg->port();
        const TTAMachine::Unit* u = p->parentUnit();
        const TTAMachine::FunctionUnit* fu =
            static_cast<const TTAMachine::FunctionUnit*>(u);
        return fu->operation(operation().name());
    }
}

bool ProgramOperation::isLegalFU(const TTAMachine::FunctionUnit& fu) const {

    if (!fu.hasOperation(operation_.name())) {
        return false;
    }
    for (int i = 0; i < inputMoveCount(); i++) {
        MoveNode& mn = inputMove(i);
        TTAProgram::Move& move = mn.move();
        if (move.hasAnnotation(
                TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_DST,
                fu.name())) {
            return false;
        }
    }
    for (int i = 0; i < outputMoveCount(); i++) {
        MoveNode& mn = outputMove(i);
        TTAProgram::Move& move = mn.move();
        if (move.hasAnnotation(
                TTAProgram::ProgramAnnotation::ANN_REJECTED_UNIT_SRC,
                fu.name())) {
            return false;
        }
    }
    return true;
}
