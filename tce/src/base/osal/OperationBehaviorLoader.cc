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
#include "TCEString.hh"

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
