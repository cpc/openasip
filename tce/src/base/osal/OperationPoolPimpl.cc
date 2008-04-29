/**
 * @file OperationPoolPimpl.cc
 *
 * Definition of OperationPoolPimpl (private implementation) class.
 *
 * @author Viljami Korhonen 2008 (viljami.korhonen@tut.fi)
 * @note rating: red
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
#include "OperationPoolPimpl.hh"
#include "OperationSerializer.hh"
#include "TCEString.hh"

using std::vector;
using std::string;

OperationPoolPimpl::OperationTable OperationPoolPimpl::operationCache_;
std::vector<OperationBehaviorProxy*> OperationPoolPimpl::proxies_;
OperationIndex* OperationPoolPimpl::index_(NULL);
OperationBehaviorLoader* OperationPoolPimpl::loader_(NULL);


/**
 * The constructor
 */
OperationPoolPimpl::OperationPoolPimpl() 
{
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
 * The destructor
 */
OperationPoolPimpl::~OperationPoolPimpl() {
}

/**
 * Cleans up the static Operation cache.
 *
 * Deletes also the Operation instances, so be sure you are not using
 * them after calling this!
 */
void 
OperationPoolPimpl::cleanupCache() {
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
OperationPoolPimpl::operation(const char* name) {
  
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

/**
 * Returns the operation index of operation pool.
 *
 * @return The operation index.
 */
OperationIndex&
OperationPoolPimpl::index() {
    assert(index_ != NULL);
    return *index_;
}

