/**
 * @file OperationBehaviorLoader.cc
 *
 * Definition of OperationBehaviorLoader class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include "OperationBehaviorLoader.hh"
#include "Operation.hh"
#include "OperationBehavior.hh"
#include "OperationIndex.hh"
#include "FileSystem.hh"
#include "StringTools.hh"
#include "OperationModule.hh"

using std::string;

const string OperationBehaviorLoader::CREATE_FUNC = "createOpBehavior_";
const string OperationBehaviorLoader::DELETE_FUNC = "deleteOpBehavior_";

/**
 * Constructor.
 *
 * OperationBehavior uses OperationIndex for getting the right modules.
 *
 * @param index OperationIndex.
 */
OperationBehaviorLoader::OperationBehaviorLoader(OperationIndex& index) :
    index_(index) {
}

/**
 * Destructor.
 *
 * Deletes all the loader operation behavior models.
 */
OperationBehaviorLoader::~OperationBehaviorLoader() {
    freeBehavior();
}

/**
 * Imports the behavior model of an operation.
 *
 * First, the module that contains the given operation is obtained from
 * OperationIndex. A pointer to appropriate create function is obtained and
 * used to create OperationBehavior. Also pointer to appropriate destruction
 * function is obtained and stored.
 *
 * @param parent The operation that owns the loaded behavior.
 * @return Operation behavior model of the operation.
 * @exception DynamicLibraryException If an error occurs while accessing the
 *                                    dynamic module.
 */
OperationBehavior&
OperationBehaviorLoader::importBehavior(const Operation& parent) 
    throw (DynamicLibraryException, InstanceNotFound) {
       
    string name = parent.name();
    // if behavior was already created, use it
    BehaviorMap::iterator iter = behaviors_.find(name);
    if (iter != behaviors_.end()) {
        return *((*iter).second);
    }
    
    try {
        OperationModule& module = index_.moduleOf(name);
        if (&module == &NullOperationModule::instance()) {
            string msg = "Module for operation " + name + " not found";
            throw InstanceNotFound(__FILE__, __LINE__, __func__, msg);
        }
       
        string creatorName = CREATE_FUNC + StringTools::stringToUpper(name);
        string destructorName = DELETE_FUNC + StringTools::stringToUpper(name);
        string modName = module.behaviorModule();
        OperationBehavior* (*behaviorCreator)(const Operation&);
        void (*behaviorDestructor)(OperationBehavior*);
        tools_.importSymbol(creatorName, behaviorCreator, modName);
        tools_.importSymbol(destructorName, behaviorDestructor, modName);
        OperationBehavior* behavior = behaviorCreator(parent);
        behaviors_[name] = behavior;
        destructors_[behavior] = behaviorDestructor;
        return *behavior;
    
    } catch (const Exception& e) {
        string msg = 
            std::string("Behavior definition for ") + parent.name() + 
            " not found.";        
        DynamicLibraryException error(__FILE__, __LINE__, __func__, msg);
        error.setCause(e);
        throw error;
    }
}

/**
 * Frees all the imported behavior models of the operations.
 */
void
OperationBehaviorLoader::freeBehavior() {
    DestructionMap::iterator iter = destructors_.begin();
    while (iter != destructors_.end()) {
        void (*behaviorDestructor)(OperationBehavior*) = (*iter).second;
        behaviorDestructor((*iter).first);
        iter++;
    }
}
