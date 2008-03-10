/**
 * @file OperationPropertyLoader.cc
 *
 * Definition of OperationPropertyLoader class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include <string>
#include <vector>

#include "OperationPropertyLoader.hh"
#include "SequenceTools.hh"
#include "OperationModule.hh"
#include "Operation.hh"

using std::string;
using std::vector;

/**
 * Constructor.
 */
OperationPropertyLoader::OperationPropertyLoader() {
}

/**
 * Destructor.
 */
OperationPropertyLoader::~OperationPropertyLoader() {
    MapIter it = operations_.begin();
    while (it != operations_.end()) {
        SequenceTools::deleteAllItems((*it).second);
        it++;
    }
}

/**
 * Loads the properties of an operation.
 *
 * If operation properties has not yet been loaded, it is done first.
 *
 * @param operation Operation which properties are loaded and updated.
 * @param module Module where properties are loaded from.
 * @exception InstanceNotFound If reading of XML fails, if Operation fails to
 *                             load its state, or if properties are not found 
 *                             at all.
 */
void
OperationPropertyLoader::loadOperationProperties(
    Operation& operation, 
    const OperationModule& module)
    throw (InstanceNotFound) {

    MapIter it = operations_.find(module.propertiesModule());
    if (it == operations_.end()) {
        // properties are not yet read, let's do it first
        loadModule(module);
        it = operations_.find(module.propertiesModule());
    }

    const vector<ObjectState*> ops = (*it).second;
    bool operationFound = false;
    
    for (unsigned int i = 0; i < ops.size(); i++) {
        if (ops[i]->stringAttribute(Operation::OPRN_NAME) == 
            operation.name()) {
            
            operationFound = true;
            try {
                operation.loadState(ops[i]);
            } catch (const ObjectStateLoadingException& o) {
                string msg = "Problems when loading the state of the Object: "
                    + o.errorMessage();
                throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
            }
            break;
        }
    }

    if (!operationFound) {
        string msg = "Properties for operation " + operation.name() + 
            " not found";
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
}

/**
 * Loads the module.
 *
 * Saves its operations as an ObjectState objects.
 * 
 * @param module The OperationModule.
 * @exception InstanceNotFound If loading the module fails.
 */
void
OperationPropertyLoader::loadModule(const OperationModule& module) 
    throw (InstanceNotFound) {
    
    serializer_.setSourceFile(module.propertiesModule());
    ObjectState* root = NULL;
    try {
        root = serializer_.readState();
    } catch (const SerializerException& s) {
        string msg = "Problems when reading the XML file: " + s.errorMessage();
        throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
    }
    vector<ObjectState*> ops;
    for (int i = 0; i < root->childCount(); i++) {
        ObjectState* child = new ObjectState(*root->child(i));
        ops.push_back(child);
    }
    delete root;
    operations_.insert(ValueType(module.propertiesModule(), ops));
}
