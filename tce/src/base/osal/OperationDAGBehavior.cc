/**
 * @file OperationDAGBehavior.cc
 *
 * Implementations of OperationDAGBehavior class.
 *
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: red
 */

#include <string>
#include <set>

#include "OperationDAGBehavior.hh"
#include "OperationDAGNode.hh"
#include "OperationDAGEdge.hh"
#include "OperationDAG.hh"
#include "Operation.hh"
#include "SimValue.hh"
#include "OperationNode.hh"
#include "TerminalNode.hh"
#include "ConstantNode.hh"

/**
 * Constructor.
 */
OperationDAGBehavior::OperationDAGBehavior(
    OperationDAG& dag, int operandCount) : 
    OperationBehavior(), dag_(dag), operandCount_(operandCount),
    cycleFound_(false) {
  
    ios_ = new SimValue[operandCount_];
    
    std::vector<std::set<OperationDAGNode*, OperationDAGNode::Comparator> > 
        nodeLevels;
    std::set<OperationDAGNode*, OperationDAGNode::Comparator> addedNodes;
    
    // organize all the nodes calculation levels, in first level we calculate
    // nodes whose inputs are read straight from TerminalNodes or 
    // ConstantNodes, in the second are added nodes whose inputs are read 
    // from TerminalNodes or first level nodes etc.
    while (static_cast<int>(addedNodes.size()) < dag_.nodeCount()) {
        std::set<OperationDAGNode*, OperationDAGNode::Comparator> currSet;
                       
        for (int i = 0; i < dag_.nodeCount(); i++) {
            OperationDAGNode& currNode = dag_.node(i);
            
            // check if the node is already handled
            if (addedNodes.find(&currNode) != addedNodes.end()) {
                continue;
            }            
            
            // if node doesn't have in edges it can always be added
            bool canBeAddedToCurrentLevel = true;

            for (int j = 0; j < dag_.inDegree(currNode); j++) {
                OperationDAGNode& parentNode = 
                    dag_.tailNode(dag_.inEdge(currNode, j));
                                                              
                // find node from previous levels
                bool wasFound = false;
                for (unsigned int k = 0; k < nodeLevels.size(); k++) {
                    if (nodeLevels[k].find(&parentNode) != 
                        nodeLevels[k].end()) {
                        wasFound = true;
                        break;
                    }
                }
             
                // it was not found... too bad.
                if (!wasFound) {
                    canBeAddedToCurrentLevel = false;
                }
            }
            
            // current level is ok for currently checked node
            if (canBeAddedToCurrentLevel) {
                currSet.insert(&currNode);
                addedNodes.insert(&currNode);
            }
        }
                
        nodeLevels.push_back(currSet);
        currSet.clear();
    }
    
    // create execution steps for all operation nodes and add them to
    // executionStep table
    std::map<OperationNode*, int> stepOfNode;

    for (unsigned int i = 0; i < nodeLevels.size(); i++) {
        for (std::set<OperationDAGNode*,OperationDAGNode::Comparator>::
                 iterator nodeIter = 
                 nodeLevels[i].begin(); nodeIter != nodeLevels[i].end();
             nodeIter++) {
            
            OperationNode* opNode = dynamic_cast<OperationNode*>(*nodeIter);
            
            if (opNode != NULL) {
                SimulationStep step;
                step.op = &opNode->referencedOperation();

                step.params = 
                    new SimValue*[step.op->numberOfInputs() + 
                                  step.op->numberOfOutputs()];
                
                // set input variables
                for (int j = 0; j < dag_.inDegree(*opNode); j++) {
                    OperationDAGEdge* currEdge = &dag_.inEdge(*opNode, j);
                    OperationDAGNode* paramNode = &(dag_.tailNode(*currEdge));
                    
                    TerminalNode* termNode = 
                        dynamic_cast<TerminalNode*>(paramNode);
                    
                    OperationNode* operNode = 
                        dynamic_cast<OperationNode*>(paramNode);
                    
                    ConstantNode* constNode = 
                        dynamic_cast<ConstantNode*>(paramNode);
                    
                    if (termNode != NULL) {
                        // if terminal node, read stuff from ios_ table
                        step.params[currEdge->dstOperand() - 1] = 
                            &ios_[termNode->operandIndex() - 1];
                        
                    } else if (constNode != NULL) {
                        // if constant node, read constant to SimValue
                        // and give it to operation
                        SimValue* newVal = new SimValue();
                        *newVal = constNode->value();
                        cleanUpTable_.push_back(newVal);
                        step.params[currEdge->dstOperand() - 1] = newVal;
                        
                    } else if (operNode != NULL) {
                        // if normal operation, read stuff from parent operand
                        SimulationStep &refStep = 
                            simulationSteps_[stepOfNode[operNode]];
                        
                        step.params[currEdge->dstOperand() - 1] = 
                            refStep.params[currEdge->srcOperand() - 1];

                    } else {
                        assert(false && "Invalid node type");
                    }
                }                    
                
                // set output variables and also create them if needed.
                for (int j = 0; j < dag_.outDegree(*opNode); j++) {
                    OperationDAGEdge* currEdge = &dag_.outEdge(*opNode, j);
                    OperationDAGNode* paramNode = &(dag_.headNode(*currEdge));
                    
                    TerminalNode* termNode = 
                        dynamic_cast<TerminalNode*>(paramNode);
                    
                    OperationNode* operNode = 
                        dynamic_cast<OperationNode*>(paramNode);
                    
                    if (termNode != NULL) {
                        // if terminal node, write stuff to ios_ table
                        step.params[currEdge->srcOperand() - 1] = 
                            &ios_[termNode->operandIndex() - 1];

                    } else if (operNode != NULL) {
                        // if normal operation, write stuff to temp
                        SimValue* newVal = new SimValue();
                        cleanUpTable_.push_back(newVal);
                        step.params[currEdge->srcOperand() - 1] = newVal;
                    
                    } else {
                        assert(false && "Invalid node type");
                    }
                }
                
                simulationSteps_.push_back(step);
            }
        }
    }
}

/**
 * Destructor.
 */
OperationDAGBehavior::~OperationDAGBehavior() {
    delete[] ios_;
    
    for (unsigned int i = 0; i < cleanUpTable_.size(); i++) {
        delete cleanUpTable_[i];
    }
    
    for (unsigned int i = 0; i < simulationSteps_.size(); i++) {
        delete[] simulationSteps_[i].params;
    }
}

/**
 * Simulates the process of starting the execution of an operation.
 *
 * Clients should invoke isTriggerLocking() before any attempt to call 
 * simulateTrigger() in current clock cycle. By default, an operation 
 * invokations are successful.
 *
 *
 * @param io The input operands and the results of the operation.
 * @param context The operation context affecting the operation results.
 * @return bool True if all values could be computed, false otherwise.
 * @exception Exception Depends on the implementation.
 */
bool 
OperationDAGBehavior::simulateTrigger(
    SimValue** operands, OperationContext& context) const {    
  
    for (int i = 0; i < operandCount_; i++) {
        ios_[i] = *operands[i];
    }
    
    for (unsigned int i = 0; i < simulationSteps_.size(); i++) {
        simulationSteps_[i].op->simulateTrigger(
            simulationSteps_[i].params, context);
    }
    
    // this back copying should be able to be prevented by setting pointers 
    // to pointers in ios_ table
    for (int i = 0; i < operandCount_; i++) {
        *operands[i] = ios_[i];
    }
    
    return true;
}

/**
 * Checks whether any of the pending results of an operation initiated in
 * earlier cycle is ready.
 * 
 * @param io The results of the operation.
 * @param context The operation context affecting the operation results.
 * @return bool True if at least one new result of the operation could be
 * computed, false otherwise. Returns false when all results are already
 * computed. 
 */
bool 
OperationDAGBehavior::lateResult(
    SimValue**, OperationContext&) const {
    
    return false;
}

/**
 * Creates the instance of operation state for this operation and adds it to
 * its operation context.
 *
 * By default this function does nothing (assumes that the operation has no
 * state). If the operation context already contains the required operation
 * state instance, nothing is done.
 *
 * @param context The operation context to add the state to.
 */
void
OperationDAGBehavior::createState(OperationContext&) const {
}

/**
 * Deletes the instance of operation state for this operation from its
 * operation context.
 * 
 * By default this function does nothing (assumes that the operation has no
 * state). If the operation context does not contain the required operation
 * state instance, nothing is done.
 *
 * @param context The operation context to delete the state from.
 */
void
OperationDAGBehavior::deleteState(OperationContext&) const {
}

/**
 * Returns the name of the state of this operation behavior.
 *
 * By default returns an empty string which denotes that there is no state.
 *
 * @return The state name for this operation behavior.
 */
const char* 
OperationDAGBehavior::stateName() const {
    return "";
}

/**
 * If behavior can be simulated.
 *
 * Check that every node can be simulated and recognize 
 * cyclic depency.
 *
 * @return true If simulation of behavior is possible.
 */
bool 
OperationDAGBehavior::canBeSimulated() const {

    if (cycleFound_ || dag_.isNull()) {
        cycleFound_ = false;
        return false;
    }

    cycleFound_ = true;
    for (int i = 0; i < dag_.nodeCount(); i++) {
        OperationNode* node = dynamic_cast<OperationNode*>(&dag_.node(i));
        
        if (node != NULL) {
            if (!node->referencedOperation().canBeSimulated()) {
                return false;
            } 
        }
    }

    cycleFound_ = false;
    return true;
}

