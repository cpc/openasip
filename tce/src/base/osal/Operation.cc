/**
 * @file Operation.cc
 *
 * Definition of Operation class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#include <string>

#include "Operation.hh"
#include "OperationDAG.hh"
#include "OperationDAGConverter.hh"
#include "SequenceTools.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "OperationPool.hh"

using std::string;
using std::vector;
using std::set;

const string Operation::OPRN_OPERATION = "operation";
const string Operation::OPRN_NAME = "name";
const string Operation::OPRN_DESCRIPTION = "description";
const string Operation::OPRN_INPUTS = "inputs";
const string Operation::OPRN_OUTPUTS = "outputs";
const string Operation::OPRN_TRAP = "trap";
const string Operation::OPRN_SIDE_EFFECTS = "side-effects";
const string Operation::OPRN_CONTROL_FLOW = "control-flow";
const string Operation::OPRN_READS_MEMORY = "reads-memory";
const string Operation::OPRN_WRITES_MEMORY = "writes-memory";
const string Operation::OPRN_AFFECTS = "affects";
const string Operation::OPRN_AFFECTED_BY = "affected-by";
const string Operation::OPRN_IN = "in";
const string Operation::OPRN_OUT = "out";
const string Operation::OPRN_TRIGGER = "trigger-semantics";

/**
 * Constructor.
 *
 * @param name The name of the Operation.
 * @param behavior The behavior of the Operation.
 */
Operation::Operation(const std::string& name, OperationBehavior& behavior) : 
    behavior_(&behavior), name_(name), description_(""),
    inputs_(0), outputs_(0), readsMemory_(false), writesMemory_(false), 
    canTrap_(false), hasSideEffects_(false), controlFlowOperation_(false) {
}

/**
 * Destructor.
 *
 * Operands are destroyed.
 */ 
Operation::~Operation() {
    
	clear();
}

/**
 * Clears the operation.
 */
void
Operation::clear() {
    
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
string
Operation::name() const {
    return name_;
}

/**
 * Returns the description of the Operation.
 *
 * @return The description of the Operation.
 */
string
Operation::description() const {
    return description_;
}

/**
 * Creates new DAG and adds it's code for operation.
 *
 * @param code Source code written in DAG language.
 */
void Operation::addDag(std::string code) {
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
Operation::removeDag(int index) {
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
Operation::dagCount() const {
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
Operation::dag(int index) const {       

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

//     std::cerr << "Returning DAG of operation: " << name() 
//               << " DAG address:" << (unsigned int)(dags_[index].dag) 
//               << " code: " << dags_[index].code 
//               << " error: " << dags_[index].error 
//               << " compilationFailed: " << dags_[index].compilationFailed
//               << " is null dag: " << dags_[index].dag->isNull() 
//               << std::endl;

    return *dags_[index].dag;
}

/**
 * Returns source code of DAG.
 *
 * @param index Index of DAG whose source code is requested.
 * @return The source code set for DAG.
 */
std::string 
Operation::dagCode(int index) const {
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
Operation::setDagCode(int index, std::string& code) {    
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
std::string 
Operation::dagError(int index) const {
    return dags_[index].error;
}

/**
 * Returns the number of the inputs of the Operation.
 *
 * @return The number of inputs of the Operation.
 */
int 
Operation::numberOfInputs() const {
    return inputs_;
}

/**
 * Returns the number of outputs of the Operation.
 *
 * @return The number of outputs of the Operation.
 */
int
Operation::numberOfOutputs() const {
    return outputs_;
}

/**
 * Returns true if Operation uses memory.
 *
 * @return True if Operation uses memory, false otherwise.
 */
bool
Operation::usesMemory() const {
    return readsMemory_ || writesMemory_;
}

/**
 * Returns true if Operation reads from memory.
 *
 * @return True if Operation reads from memory, false otherwise.
 */
bool
Operation::readsMemory() const {
    return readsMemory_;
}

/**
 * Returns true if Operation writes to memory.
 *
 * @return True if Operation writes to memory, false otherwise.
 */
bool
Operation::writesMemory() const {
    return writesMemory_;
}

/**
 * Returns true if Operation can trap.
 *
 * @return True if Operation can trap, false otherwise.
 */
bool
Operation::canTrap() const {
    return canTrap_;
}

/**
 * Return true if operation has side effects.
 *
 * @return True if Operation has side effects, false otherwise.
 */
bool
Operation::hasSideEffects() const {
    return hasSideEffects_;
}

/**
 * Return true if the operation can change control flow.
 *
 * Branches and calls of different type have this property set.
 *
 * @return True if Operation is a control flow operation.
 */
bool
Operation::isControlFlowOperation() const {
    return controlFlowOperation_;
}

/**
 * Sets the behavior for operation.
 *
 * @param behavior Behavior for an operation.
 */
void
Operation::setBehavior(OperationBehavior& behavior) {
    behavior_ = &behavior;
}

/**
 * Returns the behavior of Operation.
 *
 * @return The behavior of Operation.
 */
OperationBehavior&
Operation::behavior() const {
    return *behavior_;
}

/**
 * Returns the number of operations that affect this operation.
 *
 * @return The number of operations that affect this operation.
 */
int
Operation::affectsCount() const {
    return affects_.size();
}

/**
 * Returns the number of operations affected by this operation.
 *
 * @return The number of operations affected by this operation.
 */
int
Operation::affectedByCount() const {
    return affectedBy_.size();
}

/**
 * Returns the name of the operation this operation affects.
 *
 * @param i The index of the operation.
 * @return The name of the operation.
 */
string
Operation::affects(unsigned int i) const 
    throw (OutOfRange) {
    
    if (i >= affects_.size()) {
        string method = "Operation::affects()";
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
string
Operation::affectedBy(unsigned int i) const 
    throw (OutOfRange) {

    if (i >= affectedBy_.size()) {
        string method = "Operation::affectedBy()";
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
Operation::dependsOn(const Operation& op) const {
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
Operation::canSwap(int id1, int id2) const {

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
Operation::loadState(const ObjectState* state) 
    throw (ObjectStateLoadingException) {

	clear();

    string method = "Operation::loadState()";
    
    try {
        name_ = StringTools::stringToUpper(state->stringAttribute(OPRN_NAME));

        description_ = state->stringAttribute(OPRN_DESCRIPTION);

        inputs_ = state->intAttribute(OPRN_INPUTS);
        outputs_ = state->intAttribute(OPRN_OUTPUTS);
        
        canTrap_ = state->boolAttribute(OPRN_TRAP);
        hasSideEffects_ = state->boolAttribute(OPRN_SIDE_EFFECTS);
        controlFlowOperation_ = state->boolAttribute(OPRN_CONTROL_FLOW);
        readsMemory_ = state->boolAttribute(OPRN_READS_MEMORY);
        writesMemory_ = state->boolAttribute(OPRN_WRITES_MEMORY);

        for (int i = 0; i < state->childCount(); i++) {
            ObjectState* child = state->child(i);
           
            if (child->name() == OPRN_IN) {
                Operand* operand = new Operand(true);
                operand->loadState(child);
                if (operand->index() < 1 || operand->index() > inputs_) {
                    string msg = "Input operand index illegal";
                    throw Exception(__FILE__, __LINE__, method, msg);
                }
                insertOperand(operand, inputOperands_);

            } else if (child->name() == OPRN_OUT) {
                Operand* operand = new Operand(false);
                operand->loadState(child);
                if (operand->index() <= inputs_ || 
                    operand->index() > inputs_ + outputs_) {
                    string msg = "Output operand index illegal";
                    throw Exception(__FILE__, __LINE__, method, msg);
                }
                insertOperand(operand, outputOperands_);

            } else if (child->name() == OPRN_AFFECTS) {
                for (int j = 0; j < child->childCount(); j++) {
                    ObjectState* affects = child->child(j);
                    affects_.insert(
                        StringTools::stringToUpper(
                            affects->stringAttribute(OPRN_NAME)));
                }

            } else if (child->name() == OPRN_AFFECTED_BY) {            
                for (int j = 0; j < child->childCount(); j++) {
                    ObjectState* affectedBy = child->child(j);
                    affectedBy_.insert(
                        StringTools::stringToUpper(
                            affectedBy->stringAttribute(OPRN_NAME)));
                }

            } else if (child->name() == OPRN_TRIGGER) {                
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
		unsigned int inputs = inputOperands_.size();
        unsigned int outputs = outputOperands_.size();
		int opIndex = 1;

        for (unsigned int i = 0; 
             i < static_cast<unsigned>(inputs_) - inputs; i++) {
            
            while (&operand(opIndex) != &NullOperand::instance()) {
                opIndex++;
            }
                
            Operand* operand = new Operand(true, opIndex, Operand::SINT_WORD);
            insertOperand(operand, inputOperands_);
        }

        for (unsigned int i = 0; 
             i < static_cast<unsigned>(outputs_) - outputs; i++) {
            
            while (&operand(opIndex) != &NullOperand::instance()) {
                opIndex++;
            }
            
            Operand* operand = new Operand(true, opIndex, Operand::SINT_WORD);
            insertOperand(operand, outputOperands_);
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
Operation::saveState() const {
    
    ObjectState* root = new ObjectState(OPRN_OPERATION);
    root->setAttribute(OPRN_NAME, name_);
    root->setAttribute(OPRN_DESCRIPTION, description_);
    root->setAttribute(OPRN_INPUTS, inputs_);
    root->setAttribute(OPRN_OUTPUTS, outputs_);
    
    root->setAttribute(OPRN_TRAP, canTrap_);
    root->setAttribute(OPRN_SIDE_EFFECTS, hasSideEffects_);
    root->setAttribute(OPRN_CONTROL_FLOW, controlFlowOperation_);
    root->setAttribute(OPRN_READS_MEMORY, readsMemory_);
    root->setAttribute(OPRN_WRITES_MEMORY, writesMemory_);

    if (affectedBy_.size() > 0) {
            ObjectState* affectedBy = new ObjectState(OPRN_AFFECTED_BY);
            set<string>::const_iterator it = affectedBy_.begin();
            while (it != affectedBy_.end()) {
                ObjectState* affectedByChild = new ObjectState(OPRN_OPERATION);
                affectedByChild->setAttribute(OPRN_NAME, *it);
                affectedBy->addChild(affectedByChild);
                it++;
            }
        root->addChild(affectedBy);
    }
   
    if (affects_.size() > 0) {
        ObjectState* affects = new ObjectState(OPRN_AFFECTS);
        set<string>::const_iterator it = affects_.begin();
        while (it != affects_.end()) {
            ObjectState* affectsChild = new ObjectState(OPRN_OPERATION);
            affectsChild->setAttribute(OPRN_NAME, *it);
            affects->addChild(affectsChild);
            it++;
        }
        root->addChild(affects);
    }

    for (unsigned int i = 0; i < inputOperands_.size(); i++) {
        ObjectState* operand = inputOperands_[i]->saveState();
        operand->setName(OPRN_IN);
        root->addChild(operand);
    }
    
    for (unsigned int i = 0; i < outputOperands_.size(); i++) {
        ObjectState* operand = outputOperands_[i]->saveState();
        operand->setName(OPRN_OUT);
        root->addChild(operand);
    }

    for (int i = 0; i < dagCount(); i++) {     
        ObjectState* trigger = new ObjectState(OPRN_TRIGGER);
        trigger->setValue(dagCode(i));
        root->addChild(trigger);
    }

    return root;
}

/**
 * Returns the input Operand if found, otherwise null Operand.
 *
 * @todo Remove method or fix the function to take index of output or input
 *       instead id. Now functionality is basically same than operand() 
 *
 * @param id The id of Operand.
 * @return Operand if it is found, null Operand otherwise.
 */
Operand&
Operation::input(int id) const {
    return fetchOperand(id, inputOperands_);
}

/**
 * Returns the output Operand if found, otherwise null Operand.
 *
 * @todo Remove method or fix the function to take index of output or input
 *       instead id. Now functionality is basically same than operand() 
 *
 * @param id The id of Operand.
 * @return Operand if it is found, null Operand otherwise.
 */
Operand&
Operation::output(int id) const {
    return fetchOperand(id, outputOperands_);
}

/**
 * Returns Operand if found, otherwise null Operand.
 *
 * @param id The id of Operand.
 * @return Operand if found, null Operand otherwise.
 */
Operand&
Operation::operand(int id) const {
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
Operation::fetchOperand(int id, const std::vector<Operand*>& ops) const {
    assert(id != 0);
    for (unsigned int i = 0; i < ops.size(); i++) {
        if (ops[i]->index() == id) {
            return *ops[i];
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
Operation::fetchOperand(int id) const {
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
Operation::insertOperand(Operand* operand, std::vector<Operand*>& ops) {
	
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
Operation::simulateTrigger(
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
Operation::createState(OperationContext& context) const {
    behavior_->createState(context);
}

/**
 * Deletes an instance of operation state for this operation from the
 * operation context.
 *
 * @param context The operation context to delete the state from.
 */
void
Operation::deleteState(OperationContext& context) const {
    behavior_->deleteState(context);
}

/**
 * Returns true if this operation has behavior, or dag which is
 * simulateable (doesn't contain infinite recursion loop).
 *
 * @return True if this operation has behavior, or dag which is
 */
bool
Operation::canBeSimulated() const {    
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
std::string 
Operation::emulationFunctionName() const {
    std::string functionName; 

    functionName = "__emulate_" + name() + 
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

//////////////////////////////////////////////////////////////////////////////
// NullOperation
//////////////////////////////////////////////////////////////////////////////

NullOperation NullOperation::instance_;

/**
 * Constructor.
 */ 
NullOperation::NullOperation() : 
    Operation("<NULL>", NullOperationBehavior::instance()) {
}

/**
 * Destructor.
 */ 
NullOperation::~NullOperation() {
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool 
NullOperation::dependsOn(const Operation&) const {
    abortWithError("dependsOn()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return 0.
 */
int
NullOperation::affectsCount() const {
    abortWithError("affectsCount()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @return 0.
 */
int
NullOperation::affectedByCount() const {
    abortWithError("affectedByCount()");
    return 0;
}

/**
 * Aborts program with error log message.
 *
 * @exception Nothing.
 * @return Empty string.
 */
string
NullOperation::affects(unsigned int) const 
    throw (OutOfRange) {
    
    abortWithError("affects()");
    return "";
}

/**
 * Aborts program with error log message.
 *
 * @exception Nothing.
 * @return Empty string.
 */
string
NullOperation::affectedBy(unsigned int) const 
    throw (OutOfRange) {
    
    abortWithError("affectedBy()");
    return "";
}

/**
 * Aborts program with error log message.
 *
 * @return False
 */
bool
NullOperation::canSwap(int, int) const {
    abortWithError("canSwap()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return NullOperand.
 */
Operand&
NullOperation::input(int) const {
    abortWithError("input()");
    return NullOperand::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return NullOperand.
 */
Operand&
NullOperation::output(int) const {
    abortWithError("output()");
    return NullOperand::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return NullOperand.
 */
Operand&
NullOperation::operand(int) const {
    abortWithError("operand()");
    return NullOperand::instance();
}

/**
 * Aborts program with error log message.
 *
 * @return NullOperationBehavior.
 */
OperationBehavior&
NullOperation::behavior() const {
    abortWithError("behavior()");
    return NullOperationBehavior::instance();
}

/**
 * Aborts program with error log message.
 */
void
NullOperation::setBehavior(OperationBehavior&) {
    abortWithError("setBehavior()");
}

/**
 * Aborts program with error log message.
 *
 * @return An empty string.
 */
std::string
NullOperation::name() const {
    abortWithError("name()");
    return "";
}

/**
 * Aborts program with error log message.
 *
 * @return An empty string.
 */
std::string
NullOperation::description() const {
    abortWithError("description()");
    return "";
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int 
NullOperation::numberOfInputs() const {
    abortWithError("numberOfInputs()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return -1.
 */
int
NullOperation::numberOfOutputs() const {
    abortWithError("numberOfOutputs()");
    return -1;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::usesMemory() const {
    abortWithError("usesMemory()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::readsMemory() const {
    abortWithError("readsMemory()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::writesMemory() const {
    abortWithError("writesMemory()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::canTrap() const {
    abortWithError("canTrap()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::hasSideEffects() const {
    abortWithError("hasSideEffects()");
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::isControlFlowOperation() const {
    abortWithError(__func__);
    return false;
}

/**
 * Aborts program with error log message.
 *
 * @return False.
 * @exception Exception never.
 */
bool
NullOperation::simulateTrigger(
    SimValue**,
    OperationContext&) const 
    throw (Exception) {
    
    abortWithError("simulateTrigger()");
    return false;
}

/**
 * Aborts program with error log message.
 */
void
NullOperation::createState(OperationContext&) const {
    abortWithError("createState()");
}

/**
 * Aborts program with error log message.
 */
void
NullOperation::deleteState(OperationContext&) const {
    abortWithError("deleteState()");
}
