/**
 * @file OperationPool.cc
 *
 * Definition of OperationPool class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel@cs.tut.fi)
 * @author Miakel Lepistö 2007 (mikael.lepisto@tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include <string>
#include <vector>

#include "OperationPool.hh"
#include "OperationModule.hh"
#include "Operation.hh"
#include "OperationBehaviorProxy.hh"
#include "OperationDAGBehavior.hh"
#include "OperationBehaviorLoader.hh"
#include "Environment.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "AssocTools.hh"
#include "SequenceTools.hh"
#include "StringTools.hh"
#include "Application.hh"
#include "OperationIndex.hh"

using std::string;
using std::vector;

OperationPool::OperationTable OperationPool::operationCache_;
std::vector<OperationBehaviorProxy*> OperationPool::proxies_;
OperationIndex* OperationPool::index_(NULL);
OperationBehaviorLoader* OperationPool::loader_(NULL);

/**
 * Constructor.
 *
 * Records the search paths of the operation modules.
 */
OperationPool::OperationPool() {

    // if this is a first created instance of OperationPool,
    // initialize the OperationIndex instance with the search paths
    if (index_ == NULL) {
        index_ = new OperationIndex();
        vector<string> paths = Environment::osalPaths();
        for (unsigned int i = 0; i < paths.size(); i++) {
            index_->addPath(paths[i]);
        }
        loader_ = new OperationBehaviorLoader(*index_);
    }
}

/**
 * Destructor.
 *
 * Cleans proxies and operations.
 */
OperationPool::~OperationPool() {
}

/**
 * Cleans up the static Operation cache.
 *
 * Deletes also the Operation instances, so be sure you are not using
 * them after calling this!
 */
void 
OperationPool::cleanupCache() {
    AssocTools::deleteAllValues(operationCache_);
    SequenceTools::deleteAllItems(proxies_);
    delete index_;
    index_ = NULL;
    delete loader_;
    loader_ = NULL;
}


/**
 * Looks up an operation identified by its name and returns a reference to it.
 *
 * The first found operation is returned. If operation is not found, a null 
 * operation is returned.
 * 
 * @param name The name of the operation.
 * @return The wanted operation.
 */
Operation&
OperationPool::operation(const std::string& name) {
  
    OperationTable::iterator it = 
        operationCache_.find(StringTools::stringToLower(name));
    if (it != operationCache_.end()) {
        return *((*it).second);
    }
    
    OperationModule& module = index_->moduleOf(name);
    if (&module == &NullOperationModule::instance()) {
        return NullOperation::instance();
    }
    
    Operation* found = NULL;

    serializer_.setSourceFile(module.propertiesModule());
    ObjectState* root = serializer_.readState();
    ObjectState* child = NULL;
    
    // load operations
    for (int i = 0; i < root->childCount(); i++) {
        child = root->child(i);
        const std::string operName = 
            root->child(i)->stringAttribute(Operation::OPRN_NAME);
      
        Operation* oper = 
            new Operation(operName, NullOperationBehavior::instance());
      
        oper->loadState(child);
        operationCache_[StringTools::stringToLower(operName)] = oper;
      
        if (StringTools::ciEqual(operName, name)) {
            found = oper;
        }            
    }
    
    // add behaviours
    for (std::map<std::string, Operation*>::iterator 
             iter = operationCache_.begin();
         iter != operationCache_.end(); iter++) {
      
        Operation* oper = iter->second;
      
        OperationBehaviorProxy* proxy = 
            new OperationBehaviorProxy(*oper, *loader_);
        proxies_.push_back(proxy);
        oper->setBehavior(*proxy);
    }
    
    delete root;
    root = NULL;

    if (found != NULL) {
        return *found;
    } else {
        return NullOperation::instance();            
    }
}
