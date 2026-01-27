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
 * @file Operation.cc
 *
 * Definition of Operation class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @author Mikael Lepistö 2007 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 17 August 2004 by jn, ll, tr, pj
 */

#include <algorithm>

#include "RandomNumberGenerator.hh"
#include "Operation.hh"
#include "OperationPimpl.hh"
#include "TCEString.hh"
#include "Application.hh"
#include "Exception.hh"
#include "Operand.hh"
#include "OperationBehavior.hh"
#include "SimValue.hh"
#include "OperationContext.hh"

const char* Operation::OPRN_OPERATION = "operation";
const char* Operation::OPRN_NAME = "name";
const char* Operation::OPRN_DESCRIPTION = "description";
const char* Operation::OPRN_INPUTS = "inputs";
const char* Operation::OPRN_OUTPUTS = "outputs";
const char* Operation::OPRN_TRAP = "trap";
const char* Operation::OPRN_SIDE_EFFECTS = "side-effects";
const char* Operation::OPRN_CLOCKED = "clocked";
const char* Operation::OPRN_CONTROL_FLOW = "control-flow";
const char* Operation::OPRN_READS_MEMORY = "reads-memory";
const char* Operation::OPRN_WRITES_MEMORY = "writes-memory";
const char* Operation::OPRN_AFFECTS = "affects";
const char* Operation::OPRN_AFFECTED_BY = "affected-by";
const char* Operation::OPRN_IN = "in";
const char* Operation::OPRN_OUT = "out";
const char* Operation::OPRN_TRIGGER = "trigger-semantics";
const char* Operation::OPRN_ISCALL = "is-call";
const char* Operation::OPRN_ISBRANCH = "is-branch";

/**
 * Constructor.
 *
 * @param name The name of the Operation.
 * @param behavior The behavior of the Operation.
 */
Operation::Operation(const TCEString& name, OperationBehavior& behavior) : 
    pimpl_(new OperationPimpl(name, behavior)) {
}

/**
 * Destructor.
 *
 * Operands are destroyed.
 */ 
Operation::~Operation() {
	delete pimpl_;
    pimpl_ = NULL;
}

/**
 * Returns the name of the Operation.
 *
 * @return The name of the Operation.
 */
TCEString
Operation::name() const {
    return pimpl_->name();
}

/**
 * Returns the description of the Operation.
 *
 * @return The description of the Operation.
 */
TCEString
Operation::description() const {
    return pimpl_->description();
}

/**
 * Creates new DAG and adds it's code for operation.
 *
 * @param code Source code written in DAG language.
 */
void 
Operation::addDag(const TCEString& code) {
    pimpl_->addDag(code);
}

/**
 * Removes DAG of given index from operation.
 *
 * @param index Index of dag to delete.
 */
void 
Operation::removeDag(int index) {
    pimpl_->removeDag(index);
}


/**
 * Returns number of DAGs stored for operation.
 *
 * @return Number of DAGs stored for operation.
 */
int 
Operation::dagCount() const {
    return pimpl_->dagCount();
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
    return pimpl_->dag(index);
}

/**
 * Returns source code of DAG.
 *
 * @param index Index of DAG whose source code is requested.
 * @return The source code set for DAG.
 */
TCEString 
Operation::dagCode(int index) const {
    return pimpl_->dagCode(index);
}

/**
 * Set new source code for DAG and automatically tries to compile
 * latest version to object.
 *
 * @param index Index of DAG whose source code is updated.
 * @param code New source code in DAG Osal Language.
 */ 
void 
Operation::setDagCode(int index, const TCEString& code) {    
    pimpl_->setDagCode(index, code);
}

/**
 * Error message if DAG source code could not be compiled.
 *
 * @param index Index of DAG whose error is returned.
 * @return Error message, empty string if DAG was compiles successfully.
 */
TCEString 
Operation::dagError(int index) const {
    return pimpl_->dagError(index);
}

/**
 * Returns the number of the inputs of the Operation.
 *
 * @return The number of inputs of the Operation.
 */
int 
Operation::numberOfInputs() const {
    return pimpl_->numberOfInputs();
}

/**
 * Returns the number of outputs of the Operation.
 *
 * @return The number of outputs of the Operation.
 */
int
Operation::numberOfOutputs() const {
    return pimpl_->numberOfOutputs();
}

/**
 * Returns the number of all operands of the Operation.
 *
 * @return The number of all operands of the Operation.
 */
int
Operation::operandCount() const {
    return numberOfInputs() + numberOfOutputs();
}

/**
 * Returns true if any of the operands is a vector operand.
 *
 * @return True if the Operation has a vector operand, false otherwise.
 */
bool
Operation::isVectorOperation() const {
    return false; // WiP.
}

/**
 * Returns true if Operation uses memory.
 *
 * @return True if Operation uses memory, false otherwise.
 */
bool
Operation::usesMemory() const {
    return pimpl_->usesMemory();
}

/**
 * Returns true if Operation reads from memory.
 *
 * @return True if Operation reads from memory, false otherwise.
 */
bool
Operation::readsMemory() const {
    return pimpl_->readsMemory();
}

/**
 * Returns true if Operation writes to memory.
 *
 * @return True if Operation writes to memory, false otherwise.
 */
bool
Operation::writesMemory() const {
    return pimpl_->writesMemory();
}

/**
 * Returns true if Operation can trap.
 *
 * @return True if Operation can trap, false otherwise.
 */
bool
Operation::canTrap() const {
    return pimpl_->canTrap();
}

/**
 * Return true if operation has side effects.
 *
 * @return True if Operation has side effects, false otherwise.
 */
bool
Operation::hasSideEffects() const {
    return pimpl_->hasSideEffects();
}

/**
 * Returns true if the operation is clocked.
 * 
 * @return True if the operation is clocked.
 */
bool
Operation::isClocked() const {
    return pimpl_->isClocked();
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
    return pimpl_->isControlFlowOperation();
}

/**
 * Return true if the operation is branch.
 *
 * Branches of different type have this property set.
 *
 * @return True if Operation is a branch operation.
 */
bool
Operation::isBranch() const {
    return pimpl_->isBranch();
}

/**
 * Return true if the operation is call.
 *
 * Calls of different type have this property set.
 *
 * @return True if Operation is a call operation.
 */
bool
Operation::isCall() const {
    return pimpl_->isCall();
}

bool
Operation::isBaseOffsetMemOperation() const {
    std::string upperName = name().upper();
    return upperName == "ALDW" || upperName == "ALDHU" || 
        upperName == "ALDH" || upperName == "ALDQU" ||
        upperName == "ALDQ" || upperName == "ASTW" ||
            upperName == "ASTH"  || upperName == "ASTQ"     ||
            upperName == "ALD8"  || upperName == "ALDU8"    ||
            upperName == "ALD16" || upperName == "ALDU16"   ||
            upperName == "ALD32" || upperName == "AST32"    ||
            upperName == "AST16" || upperName == "AST8";
}

/**
 * Sets the property of operation indicating the operation is control flow.
 */
void
Operation::setControlFlowOperation(bool setting) {
    pimpl_->setControlFlowOperation(setting);
}

/**
 * Returns true if Operand does not have any kind of side effects program wide.
 *
 * In Addition of !hasSideEffects(), the operation does not access memory and
 * is not control flow operation.
 */
bool
Operation::isPure() const {
    return (!hasSideEffects()
        && !usesMemory()
        && !isControlFlowOperation()
        && !isCall()
        && !isBranch() && affectsCount() == 0
        && affectedByCount() == 0);
}

/**
 * Sets the property of operation indicating the operation is function call.
 */
void
Operation::setCall(bool setting) {
    pimpl_->setCall(setting);
}

/**
 * Sets the property of operation indicating the operation is branch changing 
 * control flow.
 */
void
Operation::setBranch(bool setting) {
    pimpl_->setBranch(setting);
}

/**
 * Sets the behavior for operation.
 *
 * @param behavior Behavior for an operation.
 */
void
Operation::setBehavior(OperationBehavior& behavior) {
    pimpl_->setBehavior(behavior);
}

/**
 * Returns the behavior of Operation.
 *
 * @return The behavior of Operation.
 */
OperationBehavior&
Operation::behavior() const {
    return pimpl_->behavior();
}

/**
 * Returns the number of operations that affect this operation.
 *
 * @note This is from the point of view of the single operation's
 * description. It doesn't know the affectedBy-properties globally
 * of all possible operations!
 *
 * @return The number of operations that affect this operation.
 */
int
Operation::affectsCount() const {
    return pimpl_->affectsCount();
}

/**
 * Returns the number of operations affected by this operation.
 *
 * @note This is from the point of view of the single operation's
 * description. It doesn't know the affects-properties globally
 * of all possible operations!
 *
 * @return The number of operations affected by this operation.
 */
int
Operation::affectedByCount() const {
    return pimpl_->affectedByCount();
}

/**
 * Returns the name of the operation this operation affects.
 *
 * @note This is from the point of view of the single operation's
 * description. It doesn't know the affects-properties globally
 * of all possible operations!
 *
 * @param i The index of the operation.
 * @return The name of the operation.
 */
TCEString
Operation::affects(unsigned int i) const {
    return pimpl_->affects(i);
}

/**
 * Returns the name of the operation that is affected by this operation.
 *
 * @note This is from the point of view of the single operation's
 * description. It doesn't know the affects-properties globally
 * of all possible operations!
 *
 * @param i The index of the operation.
 * @exception OutOfRange If index is illegal.
 * @return The name of the operation. 
 */
TCEString
Operation::affectedBy(unsigned int i) const {
    return pimpl_->affectedBy(i);
}

/**
 * Returns true if Operation depends on the given operation.
 *
 * @param op The Operation being investigated.
 * @return True if Operation depends on the given operation, false otherwise.
 */
bool 
Operation::dependsOn(const Operation& op) const {
    return pimpl_->dependsOn(op);
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
    return pimpl_->canSwap(id1, id2);
}

/**
 * Loads the Operation from ObjectState object.
 *
 * @param state The state of the Operation.
 */
void
Operation::loadState(const ObjectState* state) {
    pimpl_->loadState(state);
}

/**
 * Saves the state of the Operation in ObjectState object.
 *
 * @return The state of the Operation.
 */
ObjectState*
Operation::saveState() const {
    return pimpl_->saveState();
}

/**
 * Returns the input Operand with the given index.
 *
 * This method can be used to traverse the list of input operands
 * (the max index is numberOfOutput() - 1).
 *
 * @param index The id of Operand.
 */
Operand&
Operation::input(int index) const {
    return pimpl_->input(index);
}

void
Operation::addInput(Operand* operand) {
    pimpl_->addInput(operand);
}

void
Operation::addOutput(Operand* operand) {
    pimpl_->addOutput(operand);
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
Operation::output(int index) const {
    return pimpl_->output(index);
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
Operation::operand(int id) const {
    return pimpl_->operand(id);
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
   
    return pimpl_->simulateTrigger(io, context);
}

/**
 * Returns true if the given inputs for valid and sensible for the operation.
 *
 * The Base implementation return always true unless number of inputs does
 * not match of the operands or the operation can not be simulated.
 *
 * @param inputs Input vector of SimValues for each input operand in order.
 *                     Note: Vector at index 0 is for Operand 1, index 1 is
 *                     for Operand 2 and so on.
 * @param context The context in which the validation is performed.
 */
bool
Operation::areValid(
    const InputOperandVector& inputs,
    const OperationContext& context) const {

    return canBeSimulated()
        && (inputs.size() == static_cast<size_t>(numberOfInputs()))
        && pimpl_->behavior().areValid(inputs, context);
}

/**
 * Returns set of test vectors for the operation.
 *
 * Base implementation returns random set of test vectors. Custom operations
 * may override this and return their own defined test vectors.
 *
 * @param testVectorsOut The container where new test vectors are added to.
 * @param seed The seed value for randomized test vector generation. Same seed
 *             used in subsequent calls produces same set of test vectors.
 * @param context The operation context for test vector generation.
 */
void
Operation::makeTestVectors(
    std::vector<InputOperandVector>& testVectorsOut,
    RandomNumberGenerator::SeedType seed,
    const OperationContext& context,
    unsigned numberOfVector) const {

    if (!canBeSimulated()) {
        return;
    }

    pimpl_->behavior().makeTestVectors(
        testVectorsOut, seed, context, numberOfVector);
}

/**
 * Returns set of random test vectors suitable for basic operations.
 *
 * Base implementation returns random set of test vectors. Custom operations
 * may override this and return their own defined test vectors.
 *
 * @param testVectorsOut The container where new test vectors are added to.
 * @param seed The seed value for randomized test vector generation.
 */
void
Operation::makeTestVectors(
    std::vector<InputOperandVector>& testVectorsOut,
    RandomNumberGenerator::SeedType seed,
    unsigned numberOfVector) const {

    OperationContext opContext;
    makeTestVectors(testVectorsOut, seed, opContext, numberOfVector);
}

/**
 * Creates an instance of operation state for this operation and adds it to
 * the operation context.
 *
 * @param context The operation context to add the state in.
 */
void
Operation::createState(OperationContext& context) const {
    pimpl_->createState(context);
}

/**
 * Deletes an instance of operation state for this operation from the
 * operation context.
 *
 * @param context The operation context to delete the state from.
 */
void
Operation::deleteState(OperationContext& context) const {
    pimpl_->deleteState(context);
}

/**
 * Returns true if this operation has behavior, or dag which is
 * simulateable (doesn't contain infinite recursion loop).
 *
 * @return True if this operation has behavior, or dag which is
 */
bool
Operation::canBeSimulated() const {    
    return pimpl_->canBeSimulated();
}

/**
 * Name of emulation function which is called if the operation is emulated
 * in program.
 *
 * @return Name of emulation function of the instruction.
 */
TCEString
Operation::emulationFunctionName() const {
    return pimpl_->emulationFunctionName();
}
    
/**
 * Specifies if operation reads memory.
 */    
void
Operation::setReadsMemory(bool setting){
    pimpl_->setReadsMemory(setting);
}

/**
 * Specifies if operation writes memory.
 */    
void
Operation::setWritesMemory(bool setting){
    pimpl_->setWritesMemory(setting);
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
TCEString
NullOperation::affects(unsigned int) const {
    
    abortWithError("affects()");
    return "";
}

/**
 * Aborts program with error log message.
 *
 * @exception Nothing.
 * @return Empty string.
 */
TCEString
NullOperation::affectedBy(unsigned int) const {
    
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
TCEString
NullOperation::name() const {
    abortWithError("name()");
    return "";
}

/**
 * Aborts program with error log message.
 *
 * @return An empty string.
 */
TCEString
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
NullOperation::isClocked() const {
    abortWithError("isClocked()");
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
 */
bool
NullOperation::isCall() const {
    abortWithError(__func__);
    return false;
}
/**
 * Aborts program with error log message.
 *
 * @return False.
 */
bool
NullOperation::isBranch() const {
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
    OperationContext&) const {
    
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
