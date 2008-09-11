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
 * @file OperationPimpl.cc
 *
 * Definition of OperationPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen-no.spam-tut.fi)
 * @note rating: red
 */

#include "Operation.hh"
#include "OperationDAG.hh"
#include "OperationDAGConverter.hh"
#include "SequenceTools.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "OperationPool.hh"
#include "TCEString.hh"
#include "OperationPimpl.hh"

using std::set;
using std::vector;
using std::string;


/**
 * Constructor.
 *
 * @param name The name of the Operation.
 * @param behavior The behavior of the Operation.
 */
OperationPimpl::OperationPimpl(
    const TCEString& name, 
    OperationBehavior& behavior) : 
    behavior_(&behavior), name_(name), description_(""),
    inputs_(0), outputs_(0), readsMemory_(false), writesMemory_(false), 
    canTrap_(false), hasSideEffects_(false), isClocked_(false),
    controlFlowOperation_(false) {
}

/**
 * Destructor.
 *
 * Operands are destroyed.
 */ 
OperationPimpl::~OperationPimpl() {
    clear();
}

/**
 * Clears the operation.
 */
void
OperationPimpl::clear() {
    
    for (int i = 0; i < dagCount(); i++) {
        if (!dags_[i].dag->isNull()) {
            delete dags_[i].dag;
        }
    }   
    dags_.clear();
    
    SequenceTools::deleteAllItems(inputOperands_);
    SequenceTools::deleteAllItems(outputOperands_);
    inputOperands_.clear();
    outputOperands_.clear();
    affects_.clear();
    affectedBy_.clear();
    name_ = "";
    inputs_ = 0;
    outputs_ = 0;
    readsMemory_ = false;
    writesMemory_ = false;
    canTrap_ = false;
    hasSideEffects_ = false;
}

/**
 * Returns the name of the Operation.
 *
 * @return The name of the Operation.
 */
TCEString
OperationPimpl::name() const {
    return name_;
}

/**
 * Returns the description of the Operation.
 *
 * @return The description of the Operation.
 */
TCEString
OperationPimpl::description() const {
    return description_;
}

/**
 * Creates new DAG and adds it's code for operation.
 *
 * @param code Source code written in DAG language.
 */
void OperationPimpl::addDag(const TCEString& code) {
    OperationDAGInfo newDag;
    newDag.code = code;
    newDag.dag = &OperationDAG::null;
    newDag.compilationFailed = false;
    dags_.push_back(newDag);
}

/**
 * Removes DAG of given index from operation.
 *
 * @param index Index of dag to delete.
 */
void 
OperationPimpl::removeDag(int index) {
    DAGContainer::iterator i = dags_.begin() + index;
    
    if (!i->dag->isNull()) {
        delete i->dag;
        i->dag = &OperationDAG::null;
    }
    
    dags_.erase(i);
}


/**
 * Returns number of DAGs stored for operation.
 *
 * @return Number of DAGs stored for operation.
 */
int 
OperationPimpl::dagCount() const {
    return dags_.size();
}

/**
 * Returns an operation DAG for operation. 
 *
 * Compiles DAG from code to object if necessary.
 *
 * @param index Index of returned DAG.
 * @return Requested operation DAG or OperationDAG::null
 *         if DAG is not valid.
 */
OperationDAG& 
OperationPimpl::dag(int index) const {       

    // if dag is not up to date, try to compile it, if compilation failed and
    // dag code has not been changed don't try to compile again
    if (dags_[index].dag->isNull() && !dags_[index].compilationFailed) {
        
        try {
            dags_[index].dag = 
                OperationDAGConverter::createDAG(dags_[index].code);
            dags_[index].compilationFailed = false;
            
        } catch (const IllegalParameters &e) {
            dags_[index].dag = &OperationDAG::null;
            dags_[index].error = e.errorMessage();
            dags_[index].compilationFailed = true;

        } catch (const Exception &e) {
            dags_[index].dag = &OperationDAG::null;
            dags_[index].error = "UNEXPECTED ERROR: " + e.errorMessage();
            dags_[index].compilationFailed = true;
        }
    }

    return *dags_[index].dag;
}

/**
 * Returns source code of DAG.
 *
 * @param index Index of DAG whose source code is requested.
 * @return The source code set for DAG.
 */
TCEString
OperationPimpl::dagCode(int index) const {
    return dags_[index].code;
}

/**
 * Set new source code for DAG and automatically tries to compile
 * latest version to object.
 *
 * @param index Index of DAG whose source code is updated.
 * @param code New source code in DAG Osal Language.
 */ 
void 
OperationPimpl::setDagCode(int index, const TCEString& code) {    
    dags_[index].code = code;
    if (!dags_[index].dag->isNull()) {
        delete dags_[index].dag;
        dags_[index].dag = &OperationDAG::null;
    }
    dags_[index].compilationFailed = false;
}

/**
 * Error message if DAG source code could not be compiled.
 *
 * @param index Index of DAG whose error is returned.
 * @return Error message, empty string if DAG was compiles successfully.
 */
TCEString 
OperationPimpl::dagError(int index) const {
    return dags_[index].error;
}

/**
 * Returns the number of the inputs of the Operation.
 *
 * @return The number of inputs of the Operation.
 */
int 
OperationPimpl::numberOfInputs() const {
    return inputs_;
}

/**
 * Returns the number of outputs of the Operation.
 *
 * @return The number of outputs of the Operation.
 */
int
OperationPimpl::numberOfOutputs() const {
    return outputs_;
}

/**
 * Returns true if Operation uses memory.
 *
 * @return True if Operation uses memory, false otherwise.
 */
bool
OperationPimpl::usesMemory() const {
    return readsMemory_ || writesMemory_;
}

/**
 * Returns true if Operation reads from memory.
 *
 * @return True if Operation reads from memory, false otherwise.
 */
bool
OperationPimpl::readsMemory() const {
    return readsMemory_;
}

/**
 * Returns true if Operation writes to memory.
 *
 * @return True if Operation writes to memory, false otherwise.
 */
bool
OperationPimpl::writesMemory() const {
    return writesMemory_;
}

/**
 * Returns true if Operation can trap.
 *
 * @return True if Operation can trap, false otherwise.
 */
bool
OperationPimpl::canTrap() const {
    return canTrap_;
}

/**
 * Return true if operation has side effects.
 *
 * @return True if Operation has side effects, false otherwise.
 */
bool
OperationPimpl::hasSideEffects() const {
    return hasSideEffects_;
}

/**
 * Returns true if the operation is clocked.
 * 
 * @return True if the operation is clocked.
 */
bool
OperationPimpl::isClocked() const {
    return isClocked_;
}

/**
 * Return true if the operation can change control flow.
 *
 * Branches and calls of different type have this property set.
 *
 * @return True if Operation is a control flow operation.
 */
bool
OperationPimpl::isControlFlowOperation() const {
    return controlFlowOperation_;
}

/**
 * Sets the behavior for operation.
 *
 * @param behavior Behavior for an operation.
 */
void
OperationPimpl::setBehavior(OperationBehavior& behavior) {
    behavior_ = &behavior;
}

/**
 * Returns the behavior of Operation.
 *
 * @return The behavior of Operation.
 */
OperationBehavior&
OperationPimpl::behavior() const {
    return *behavior_;
}

/**
 * Returns the number of operations that affect this operation.
 *
 * @return The number of operations that affect this operation.
 */
int
OperationPimpl::affectsCount() const {
    return affects_.size();
}

/**
 * Returns the number of operations affected by this operation.
 *
 * @return The number of operations affected by this operation.
 */
int
OperationPimpl::affectedByCount() const {
    return affectedBy_.size();
}

/**
 * Returns the name of the operation this operation affects.
 *
 * @param i The index of the operation.
 * @return The name of the operation.
 */
TCEString
OperationPimpl::affects(unsigned int i) const {
    
    if (i >= affects_.size()) {
        string method = "OperationPimpl::affects()";
        string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, method, msg);
    }
    
    set<string>::const_iterator it = affects_.begin();
    unsigned int count = 0;
    while (count < i) {
        it++;
        count++;
    }
    return *it;
}

/**
 * Returns the name of the operation that is affected by this operation.
 *
 * @param i The index of the operation.
 * @exception OutOfRange If index is illegal.
 * @return The name of the operation. 
 */
TCEString
OperationPimpl::affectedBy(unsigned int i) const {

    if (i >= affectedBy_.size()) {
        string method = "OperationPimpl::affectedBy()";
        string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, method, msg);
    }

    set<string>::const_iterator it = affectedBy_.begin();
    unsigned int count = 0;
    while (count < i) {
        it++;
        count++;
    }
    return *it;
}

/**
 * Returns true if Operation depends on the given operation.
 *
 * @param op The Operation being investigated.
 * @return True if Operation depends on the given operation, false otherwise.
 */
bool 
OperationPimpl::dependsOn(const Operation& op) const {
    set<string>::const_iterator it = affects_.find(op.name());
    if (it != affects_.end()) {
        return true;
    } else {
        it = affectedBy_.find(op.name());
        if (it != affectedBy_.end()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Returns true if Operands can be swapped.
 *
 * @param id1 Id of the first Operand.
 * @param id2 Id of the second Operand.
 * @return True, if Operands can be swapped, false otherwise.
 */
bool
OperationPimpl::canSwap(int id1, int id2) const {

    Operand& op1 = fetchOperand(id1);
    Operand& op2 = fetchOperand(id2);
    
    if (&op1 == &NullOperand::instance() || 
        &op2 == &NullOperand::instance()) {
        return false;
    }
    return op1.canSwap(op2);
}

/**
 * Loads the Operation from ObjectState object.
 *
 * @param state The state of the Operation.
 */
void
OperationPimpl::loadState(const ObjectState* state) {

    clear();

    string method = "OperationPimpl::loadState()";
    
    try {
        name_ = StringTools::stringToUpper(state->stringAttribute(
            Operation::OPRN_NAME));

        description_ = state->stringAttribute(Operation::OPRN_DESCRIPTION);

        inputs_ = state->intAttribute(Operation::OPRN_INPUTS);
        outputs_ = state->intAttribute(Operation::OPRN_OUTPUTS);

        canTrap_ = state->boolAttribute(Operation::OPRN_TRAP);
        hasSideEffects_ = state->boolAttribute(Operation::OPRN_SIDE_EFFECTS);
        isClocked_ = state->boolAttribute(Operation::OPRN_CLOCKED);
        controlFlowOperation_ = state->boolAttribute(Operation::OPRN_CONTROL_FLOW);
        readsMemory_ = state->boolAttribute(Operation::OPRN_READS_MEMORY);
        writesMemory_ = state->boolAttribute(Operation::OPRN_WRITES_MEMORY);

        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
           
            if (child->name() == Operation::OPRN_IN) {
                Operand* operand = new Operand(true);
                operand->loadState(child);
                if (operand->index() < 1 || operand->index() > inputs_) {
                    string msg = "Input operand index illegal";
                    throw Exception(__FILE__, __LINE__, method, msg);
                }
                insertOperand(operand, inputOperands_);

            } else if (child->name() == Operation::OPRN_OUT) {
                Operand* operand = new Operand(false);
                operand->loadState(child);
                if (operand->index() <= inputs_ || 
                    operand->index() > inputs_ + outputs_) {
                    string msg = "Output operand index illegal";
                    throw Exception(__FILE__, __LINE__, method, msg);
                }
                insertOperand(operand, outputOperands_);

            } else if (child->name() == Operation::OPRN_AFFECTS) {
                for (int j = 0; j < child->childCount(); j++) {
                    ObjectState* affects = child->child(j);
                    affects_.insert(
                        StringTools::stringToUpper(
                            affects->stringAttribute(Operation::OPRN_NAME)));
                }

            } else if (child->name() == Operation::OPRN_AFFECTED_BY) {            
                for (int j = 0; j < child->childCount(); j++) {
                    ObjectState* affectedBy = child->child(j);
                    affectedBy_.insert(
                        StringTools::stringToUpper(
                            affectedBy->stringAttribute(Operation::OPRN_NAME)));
                }

            } else if (child->name() == Operation::OPRN_TRIGGER) {                
                addDag(child->stringValue());
                
            } else {
                // no other childs should be possible
                string msg = "Unknown child: " + child->name();
                throw Exception(__FILE__, __LINE__, method, msg);
            }
        }
        
        // TODO: check can-swap operands and verify that they are created
        //       properly
        
        // add operands that are not defined in XML nor can-swap..
        for (int i = 1; i <= inputs_; ++i) {
            if (&operand(i) == &NullOperand::instance()) {
                Operand* operand = new Operand(true, i, Operand::SINT_WORD);
                insertOperand(operand, inputOperands_);
            }
        }

        for (int i = 1; i <= outputs_; ++i) {
            if (&operand(i) == &NullOperand::instance()) {
                Operand* operand = new Operand(true, i, Operand::SINT_WORD);
                insertOperand(operand, outputOperands_);
            }
        }

    } catch (const Exception& e) {
        string msg = "Problems loading Operation: " + e.errorMessage();
        ObjectStateLoadingException error(__FILE__, __LINE__, method, msg);
        error.setCause(e);
        throw error;
    }
}

/**
 * Saves the state of the Operation in ObjectState object.
 *
 * @return The state of the Operation.
 */
ObjectState*
OperationPimpl::saveState() const {
    
    ObjectState* root = new ObjectState(Operation::OPRN_OPERATION);
    root->setAttribute(Operation::OPRN_NAME, name_);
    root->setAttribute(Operation::OPRN_DESCRIPTION, description_);
    root->setAttribute(Operation::OPRN_INPUTS, inputs_);
    root->setAttribute(Operation::OPRN_OUTPUTS, outputs_);
    
    root->setAttribute(Operation::OPRN_TRAP, canTrap_);
    root->setAttribute(Operation::OPRN_SIDE_EFFECTS, hasSideEffects_);
    root->setAttribute(Operation::OPRN_CLOCKED, isClocked_);
    root->setAttribute(Operation::OPRN_CONTROL_FLOW, controlFlowOperation_);
    root->setAttribute(Operation::OPRN_READS_MEMORY, readsMemory_);
    root->setAttribute(Operation::OPRN_WRITES_MEMORY, writesMemory_);

    if (affectedBy_.size() > 0) {
            ObjectState* affectedBy = new ObjectState(Operation::OPRN_AFFECTED_BY);
            set<string>::const_iterator it = affectedBy_.begin();
            while (it != affectedBy_.end()) {
                ObjectState* affectedByChild = new ObjectState(Operation::OPRN_OPERATION);
                affectedByChild->setAttribute(Operation::OPRN_NAME, *it);
                affectedBy->addChild(affectedByChild);
                it++;
            }
        root->addChild(affectedBy);
    }
   
    if (affects_.size() > 0) {
        ObjectState* affects = new ObjectState(Operation::OPRN_AFFECTS);
        set<string>::const_iterator it = affects_.begin();
        while (it != affects_.end()) {
            ObjectState* affectsChild = new ObjectState(Operation::OPRN_OPERATION);
            affectsChild->setAttribute(Operation::OPRN_NAME, *it);
            affects->addChild(affectsChild);
            it++;
        }
        root->addChild(affects);
    }

    for (unsigned int i = 0; i < inputOperands_.size(); i++) {
        ObjectState* operand = inputOperands_[i]->saveState();
        operand->setName(Operation::OPRN_IN);
        root->addChild(operand);
    }
    
    for (unsigned int i = 0; i < outputOperands_.size(); i++) {
        ObjectState* operand = outputOperands_[i]->saveState();
        operand->setName(Operation::OPRN_OUT);
        root->addChild(operand);
    }

    for (int i = 0; i < dagCount(); i++) {     
        ObjectState* trigger = new ObjectState(Operation::OPRN_TRIGGER);
        trigger->setValue(dagCode(i));
        root->addChild(trigger);
    }

    return root;
}

/**
 * Returns the input Operand with the given index.
 *
 * This method can be used to traverse the list of output operands
 * (the max index is numberOfOutput() - 1).
 *
 * @param index The id of Operand.
 */
Operand&
OperationPimpl::input(int index) const {
    return *inputOperands_.at(index);
}

/**
 * Returns the output Operand with the given index.
 *
 * This method can be used to traverse the list of output operands
 * (the max index is numberOfOutput() - 1).
 *
 * @param index The index of Operand.
 */
Operand&
OperationPimpl::output(int index) const {
    return *outputOperands_.at(index);
}

/**
 * Returns the Operand with the given id if found, otherwise null Operand.
 *
 * @note This method is used to fetch operands with their 'id', the number
 * which identifies it to the programmer. That is, output ids start from
 * the last input id + 1, etc.
 *
 * @param id The id of Operand.
 * @return Operand if found, null Operand otherwise.
 */
Operand&
OperationPimpl::operand(int id) const {
    assert(id != 0);
    Operand& op = fetchOperand(id, inputOperands_);
    if (&op == &NullOperand::instance()) {
        return fetchOperand(id, outputOperands_);
    }
    return op;
}

/**
 * Returns an operand with a certain id if it exists.
 *
 * If operand is not found, a null operand is returned.
 *
 * @param id The id of an operand.
 * @param ops Vector where operand is searched.
 * @return Operand with a certain id.
 */
Operand&
OperationPimpl::fetchOperand(int id, const std::vector<Operand*>& ops) const {
    assert(id != 0);

    for (std::vector<Operand*>::const_iterator i = ops.begin(); i != ops.end();
        ++i) {
        if ((*i)->index() == id) {
            return **i;
        }
    }

    return NullOperand::instance();
}

/**
 * Returns an operand with a certain id if it exists.
 *
 * If operand is not found, NullOperand is returned.
 *
 * @param id Id of the operand.
 * @return Operand with a certain id.
 */
Operand&
OperationPimpl::fetchOperand(int id) const {
    assert(id != 0);

    for (unsigned int i = 0; i < inputOperands_.size(); i++) {
        if (inputOperands_[i]->index() == id) {
            return *inputOperands_[i];
        }
    }

    for (unsigned int i = 0; i < outputOperands_.size(); i++) {
        if (outputOperands_[i]->index() == id) {
            return *outputOperands_[i];
        }
    }

    return NullOperand::instance();
}

/**
 * Inserts operand to the right place.
 *
 * Operands are inserted according to their indexes.
 *
 * @param operand Operand to be inserted.
 * @param ops Vector in which operand is inserted.
 */
void
OperationPimpl::insertOperand(Operand* operand, std::vector<Operand*>& ops) {
    
    vector<Operand*>::iterator it = ops.begin();
    bool inserted = false;
    while (it != ops.end()) {
        if ((*it)->index() > operand->index()) {
            inserted = true;
            ops.insert(it, operand);
            break;
        }
        it++;
    }
    if (!inserted) {
        ops.push_back(operand);
    }
}

/**
 * Simulates the process of starting execution of an operation.
 *
 * @param io The input and output operands.
 * @param context The operation context.
 * @return True, if all values could be computed, false otherwise.
 * @exception Exception Depends on the operation behavior.
 *                     
 */
bool
OperationPimpl::simulateTrigger(
    SimValue** io,
    OperationContext& context) const {
   
    return behavior_->simulateTrigger(io, context);
}

/**
 * Creates an instance of operation state for this operation and adds it to
 * the operation context.
 *
 * @param context The operation context to add the state in.
 */
void
OperationPimpl::createState(OperationContext& context) const {
    behavior_->createState(context);
}

/**
 * Deletes an instance of operation state for this operation from the
 * operation context.
 *
 * @param context The operation context to delete the state from.
 */
void
OperationPimpl::deleteState(OperationContext& context) const {
    behavior_->deleteState(context);
}

/**
 * Returns true if this operation has behavior, or dag which is
 * simulateable (doesn't contain infinite recursion loop).
 *
 * @return True if this operation has behavior, or dag which is
 */
bool
OperationPimpl::canBeSimulated() const {    
    return behavior_->canBeSimulated();
}


std::string 
llvmOperandType(Operand::OperandType type) {
    switch (type) {
    case Operand::SINT_WORD:
        return "i32";
    case Operand::UINT_WORD:
        return "i32";
    case Operand::FLOAT_WORD:
        return "f32";
    case Operand::DOUBLE_WORD:
        return "f64";
    default:
        return "Unknown";
    }
}

/**
 * Name of emulation function which is called if the operation is emulated
 * in program.
 *
 * @return Name of emulation function of the instruction.
 */
TCEString 
OperationPimpl::emulationFunctionName() const {
    std::string functionName; 

    functionName = "__emulate_" + std::string(name()) + 
        "_" + Conversion::toString(numberOfInputs()) +
        "_" + Conversion::toString(numberOfOutputs());
    
    for (int i = 1; i <= numberOfInputs(); i++) {
        Operand& oper = operand(i);
        functionName += "_" + llvmOperandType(oper.type());
    }
    
    for (int i = 1; i <= numberOfOutputs(); i++) {
        Operand& oper = operand(numberOfInputs() + i);
        functionName += "_" + llvmOperandType(oper.type());        
    }    
    
    return functionName;
}
