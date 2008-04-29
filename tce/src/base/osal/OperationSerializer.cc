/**
 * @file OperationSerializer.cc
 *
 * Definition of OperationSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2007 (pekka.jaaskelainen@tut.fi)
 * @author Mikael Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include <string>

#include "OperationSerializer.hh"
#include "Conversion.hh"
#include "Operation.hh"
#include "FileSystem.hh"
#include "Environment.hh"
#include "StringTools.hh"
#include "Operand.hh"

using std::string;

/// Schema file name.
const string SCHEMA_FILE_NAME = "Operation_Schema.xsd";
/// Name of the library.
const string LIBRARY_NAME = "osal";

/// The version number.
const double VERSION_NUMBER = 0.1;

/// XML tag for the module.
const string OPSER_OSAL = "osal";
/// XML tag for version.
const string OPSER_VERSION = "version";
 
/**
 * Constructor.
 *
 * If schema file is found it is used.
 */
OperationSerializer::OperationSerializer() : Serializer() {

    string path = Environment::schemaDirPath(LIBRARY_NAME);
    if (path != "") {
        string schema = path + FileSystem::DIRECTORY_SEPARATOR + 
            SCHEMA_FILE_NAME;
        if (FileSystem::fileExists(schema)) {
            setSchemaFile(schema);
            setUseSchema(true);
        }
    }
}

/**
 * Destructor.
 */
OperationSerializer::~OperationSerializer() {
}

/**
 * Writes the state of operations to XML file.
 *
 * The states of operations are first converted to the format understood
 * by XMLSerializer and then written.
 *
 * @param state ObjectState to be written.
 * @exception SerializerException If error occurs.
 */
void
OperationSerializer::writeState(const ObjectState* state)
    throw (SerializerException) {

    try {
        ObjectState* converted = convertToXMLFormat(state);
        serializer_.writeState(converted);
        delete converted;
    } catch (const Exception& e) {
        string method = "OperationSerializer::writeState()";
        string msg = "Problems writing the state: " + e.errorMessage();
        SerializerException error(__FILE__, __LINE__, __func__, msg);
        error.setCause(e);
        throw error;
    }
}

/**
 * Reads the Operation property file and returns a corresponding ObjectState.
 *
 * ObjectState is converted to more easily parseable format before returning.
 *
 * @return The read ObjectState tree.
 */
ObjectState*
OperationSerializer::readState()
    throw (SerializerException) {

    ObjectState* operationState = NULL;
    try {
        ObjectState* xmlState = serializer_.readState();
        operationState = convertToOperationFormat(xmlState);
        delete xmlState;
    } catch (const Exception& e) {
        string method = "OperationSerializer::readState()";
        string msg = "Problems reading the state: " + e.errorMessage();
        SerializerException error(__FILE__, __LINE__, __func__, msg);
        error.setCause(e);
        throw error;
    }

    return operationState;
}

/**
 * Converts ObjectState to the format understood by XMLSerializer.
 *
 * XMLSerializer needs an ObjectState tree in which each attribute
 * is in its own node. ObjectState tree obtained form an operation
 * is not in that format. This is why conversion is needed.
 *
 * @param state ObjectState to be converted.
 * @exception ObjectStateLoadingException If ObjectState conversion fails.
 * @return The converted ObjectState tree.
 */
ObjectState* 
OperationSerializer::convertToXMLFormat(const ObjectState* state) 
    throw (ObjectStateLoadingException) {
	
    ObjectState* operation = new ObjectState(OPSER_OSAL);
    operation->setAttribute(OPSER_VERSION, VERSION_NUMBER);
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        operation->addChild(toXMLFormat(child));
    }
    return operation;
}

/**
 * Converts an individual operation to xml format.
 *
 * @param state ObjectState to be converted.
 * @exception ObjectStateLoadingException If ObjectState conversion fails.
 * @return The converted ObjectState tree.
 */
ObjectState*
OperationSerializer::toXMLFormat(const ObjectState* state) 
    throw (ObjectStateLoadingException) {

    ObjectState* oper = new ObjectState(Operation::OPRN_OPERATION);
    try {
        ObjectState* nameChild = new ObjectState(Operation::OPRN_NAME);
        nameChild->setValue(state->stringAttribute(Operation::OPRN_NAME));
        oper->addChild(nameChild);

        ObjectState* descriptionChild = new ObjectState(Operation::OPRN_DESCRIPTION);
        descriptionChild->setValue(state->stringAttribute(Operation::OPRN_DESCRIPTION));
        oper->addChild(descriptionChild); 
        
        ObjectState* inputChild = new ObjectState(Operation::OPRN_INPUTS);
        inputChild->setValue(state->intAttribute(Operation::OPRN_INPUTS));
        oper->addChild(inputChild);
        
        ObjectState* outputChild = new ObjectState(Operation::OPRN_OUTPUTS);
        outputChild->setValue(state->intAttribute(Operation::OPRN_OUTPUTS));
        oper->addChild(outputChild);
        
        if (state->boolAttribute(Operation::OPRN_READS_MEMORY)) {
            oper->addChild(new ObjectState(Operation::OPRN_READS_MEMORY));
        }
        
        if (state->boolAttribute(Operation::OPRN_WRITES_MEMORY)) {
            oper->addChild(new ObjectState(Operation::OPRN_WRITES_MEMORY));
        }
        
        if (state->boolAttribute(Operation::OPRN_TRAP)) {
            oper->addChild(new ObjectState(Operation::OPRN_TRAP));
        }
        
        if (state->boolAttribute(Operation::OPRN_SIDE_EFFECTS)) {
            oper->addChild(new ObjectState(Operation::OPRN_SIDE_EFFECTS));
        }

        if (state->boolAttribute(Operation::OPRN_CONTROL_FLOW)) {
            oper->addChild(new ObjectState(Operation::OPRN_CONTROL_FLOW));
        }
        
        for (int i = 0; i < state->childCount(); i++) {
            
            ObjectState* child = state->child(i);
            if (child->name() == Operation::OPRN_IN || 
                child->name() == Operation::OPRN_OUT) {
                
                ObjectState* operandChild = NULL;
                if (child->name() == Operation::OPRN_IN) {
                    operandChild = new ObjectState(Operation::OPRN_IN);
                } else {
                    operandChild = new ObjectState(Operation::OPRN_OUT);
                }
                
                operandChild->setAttribute(
                    Operand::OPRND_ID, 
                    child->intAttribute(Operand::OPRND_ID));
                
                operandChild->setAttribute(
                    Operand::OPRND_TYPE, 
                    child->stringAttribute(Operand::OPRND_TYPE));
                
                if (child->boolAttribute(Operand::OPRND_MEM_ADDRESS)) {
                    operandChild->
                        addChild(new ObjectState(Operand::OPRND_MEM_ADDRESS));
                }
                
                if (child->boolAttribute(Operand::OPRND_MEM_DATA)) {
                    operandChild->addChild(
                        new ObjectState(Operand::OPRND_MEM_DATA));
                }

                // can swap list
                if (child->childCount() > 0) {
                    ObjectState* swap = 
                        new ObjectState(Operand::OPRND_CAN_SWAP);
                    ObjectState* canSwap = child->child(0);

                    for (int j = 0; j < canSwap->childCount(); j++) {
                        ObjectState* swapChild = canSwap->child(j);
                        
                        ObjectState* newSwap = 
                            new ObjectState(Operand::OPRND_IN);

                        newSwap->setAttribute(
                            Operand::OPRND_ID, 
                            swapChild->intAttribute(Operand::OPRND_ID));
                        swap->addChild(newSwap);
                                              
                    }
                    operandChild->addChild(swap);
                }

                oper->addChild(operandChild);
            } else {
                
                ObjectState* newChild = new ObjectState(*child);
                oper->addChild(newChild);
            }
        }
            
    } catch (const Exception& e) {
        string msg = "Error while constructing ObjectState tree: " +
            e.errorMessage();
        ObjectStateLoadingException error(__FILE__, __LINE__, __func__, msg);
        error.setCause(e);
        throw error;
    }
    return oper;
}

/**
 * Converts ObjectState to more easily parseable format.
 *
 * More easily parseable format is such that there are less nodes.
 * Only children of the main node are the operands, affects, and
 * affected-by nodes.  
 *
 * @param state ObjectState to be converted.
 * @exception ObjectStateLoadingException If ObjectState conversion fails.
 * @return The converted ObjectState tree.
 */
ObjectState*
OperationSerializer::convertToOperationFormat(const ObjectState* state) 
    throw (ObjectStateLoadingException) {
    
    ObjectState* operation = new ObjectState(OPSER_OSAL);
    for (int i = 0; i < state->childCount(); i++) {
        ObjectState* child = state->child(i);
        operation->addChild(toOperation(child));
    }
    return operation;
}

/**
 * Converts Operation ObjectState tree to more easily parsed format.
 *
 * @param state ObjectState to be converted.
 * @exception ObjectStateLoadingException If ObjectState conversion fails.
 * @return The converted ObjectState tree.
 */
ObjectState*
OperationSerializer::toOperation(const ObjectState* state) 
    throw (ObjectStateLoadingException) {

    ObjectState* root = new ObjectState(Operation::OPRN_OPERATION);
    
    try {
        for (int i = 0; i < state->childCount(); i++) {
            
            ObjectState* child = state->child(i);
            
            if (child->name() == Operation::OPRN_NAME) {
                root->setAttribute(
                    Operation::OPRN_NAME, child->stringValue());
            } else if (child->name() == Operation::OPRN_DESCRIPTION) {
                root->setAttribute(Operation::OPRN_DESCRIPTION, child->stringValue());
            } else if (child->name() == Operation::OPRN_INPUTS) {
                root->setAttribute(Operation::OPRN_INPUTS, child->intValue());
            } else if (child->name() == Operation::OPRN_OUTPUTS) {
                root->setAttribute(Operation::OPRN_OUTPUTS, child->intValue());
            } else if (child->name() == Operation::OPRN_READS_MEMORY) {
                root->setAttribute(Operation::OPRN_READS_MEMORY, true);
            } else if (child->name() == Operation::OPRN_WRITES_MEMORY) {
                root->setAttribute(Operation::OPRN_WRITES_MEMORY, true);
            } else if (child->name() == Operation::OPRN_TRAP) {
                root->setAttribute(Operation::OPRN_TRAP, true);
            } else if (child->name() == Operation::OPRN_SIDE_EFFECTS) {
                root->setAttribute(Operation::OPRN_SIDE_EFFECTS, true);
            } else if (child->name() == Operation::OPRN_CONTROL_FLOW) {
                root->setAttribute(Operation::OPRN_CONTROL_FLOW, true);

//            } else if (child->name() == Operation::OPRN_AFFECTED_BY) {
//                ObjectState* affectedBy = new ObjectState(*child);
//                root->addChild(affectedBy);
                
//            } else if (child->name() == Operation::OPRN_AFFECTS) {
//                ObjectState* affects = new ObjectState(*child);
//                root->addChild(affects);
                
            } else if (child->name() == Operation::OPRN_IN) {
                ObjectState* inOperand = new ObjectState(Operation::OPRN_IN);

                inOperand->setAttribute(
                    Operand::OPRND_ID, 
                    child->intAttribute(Operand::OPRND_ID));

                inOperand->setAttribute(
                    Operand::OPRND_TYPE, 
                    child->stringAttribute(Operand::OPRND_TYPE));

                root->addChild(inOperand);
                
                setOperandProperties(inOperand, child);
                
                if (!inOperand->hasAttribute(Operand::OPRND_MEM_ADDRESS)) {
                    inOperand->setAttribute(Operand::OPRND_MEM_ADDRESS, false);
                }
                
                if (!inOperand->hasAttribute(Operand::OPRND_MEM_DATA)) {
                    inOperand->setAttribute(Operand::OPRND_MEM_DATA, false);
                }
                
            } else if (child->name() == Operation::OPRN_OUT) {
                
                ObjectState* outOperand = new ObjectState(Operation::OPRN_OUT);

                outOperand->setAttribute(
                    Operand::OPRND_ID, 
                    child->intAttribute(Operand::OPRND_ID));

                outOperand->setAttribute(
                    Operand::OPRND_TYPE, 
                    child->stringAttribute(Operand::OPRND_TYPE));

                root->addChild(outOperand);
                setOperandProperties(outOperand, child);
                
                outOperand->setAttribute(Operand::OPRND_MEM_ADDRESS, false);
                
                if (!outOperand->hasAttribute(Operand::OPRND_MEM_DATA)) {
                    outOperand->setAttribute(Operand::OPRND_MEM_DATA, false);
                }

            } else {
                // just copy other elements
                ObjectState* copyChild = new ObjectState(*child);
                root->addChild(copyChild);                 
            } 
        }

        root->setAttribute(
            Operation::OPRN_NAME, StringTools::stringToUpper(
                root->stringAttribute(Operation::OPRN_NAME)));
        
        if (!root->hasAttribute(Operation::OPRN_DESCRIPTION)) {
            root->setAttribute(Operation::OPRN_DESCRIPTION, std::string(""));
        }

        if (!root->hasAttribute(Operation::OPRN_READS_MEMORY)) {
            root->setAttribute(Operation::OPRN_READS_MEMORY, false);
        }
        
        if (!root->hasAttribute(Operation::OPRN_WRITES_MEMORY)) {
            root->setAttribute(Operation::OPRN_WRITES_MEMORY, false);
        }
        
        if (!root->hasAttribute(Operation::OPRN_TRAP)) {
            root->setAttribute(Operation::OPRN_TRAP, false);
        }
        
        if (!root->hasAttribute(Operation::OPRN_SIDE_EFFECTS)) {
            root->setAttribute(Operation::OPRN_SIDE_EFFECTS, false);
        }

        if (!root->hasAttribute(Operation::OPRN_CONTROL_FLOW)) {
            root->setAttribute(Operation::OPRN_CONTROL_FLOW, false);
        }

    } catch (const Exception& e) {
        string msg = "Error while constructing ObjectState tree" +
            e.errorMessage();
        ObjectStateLoadingException error(__FILE__, __LINE__, __func__, msg);
        error.setCause(e);
        throw error;
    }
    
    return root;
}

/**
 * Set operand properties to right values.
 *
 * @param operand Operand which properties are set.
 * @param source Original ObjectState tree.
 */
void
OperationSerializer::setOperandProperties(
    ObjectState* operand, 
    ObjectState* source) {

    for (int childIndex = 0; childIndex < source->childCount();
         childIndex++) {
        
        ObjectState* child = source->child(childIndex);
        if (child->name() == Operand::OPRND_MEM_ADDRESS) {
            operand->setAttribute(Operand::OPRND_MEM_ADDRESS, true);
        } else if (child->name() == Operand::OPRND_MEM_DATA) {
            operand->setAttribute(Operand::OPRND_MEM_DATA, true);
        } else if (child->name() == Operand::OPRND_CAN_SWAP) {            
            ObjectState* canSwap = new ObjectState(*child);
            operand->addChild(canSwap);
        }
    }
}

/**
 * Sets source file.
 *
 * @param filename The name of the file.
 */
void
OperationSerializer::setSourceFile(const std::string& filename) {
    serializer_.setSourceFile(filename);
}

/**
 * Sets destination file.
 *
 * @param filename The name of the file.
 */
void
OperationSerializer::setDestinationFile(const std::string& filename) {
    serializer_.setDestinationFile(filename);
}

/**
 * Sets schema file.
 *
 * @param fileName The name of the file.
 */
void
OperationSerializer::setSchemaFile(const std::string& filename) {
    serializer_.setSchemaFile(filename);
}

/**
 * Sets whether schema is used or not.
 *
 * @param useSchema True or false, depending on if schema is to be used or not.
 */
void
OperationSerializer::setUseSchema(bool useSchema) {
    serializer_.setUseSchema(useSchema);
}
