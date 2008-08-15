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
 * @file OperationIndex.cc
 *
 * Definition of OperationIndex class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 19 August 2004 by pj, jn, ao, ac
 */

#include "OperationIndex.hh"
#include "Operation.hh"
#include "OperationModule.hh"
#include "ObjectState.hh"
#include "FileSystem.hh"
#include "Application.hh"
#include "AssocTools.hh"
#include "StringTools.hh"
#include "SequenceTools.hh"
#include "MapTools.hh"

using std::map;
using std::string;
using std::vector;

const string OperationIndex::PROPERTY_FILE_EXTENSION = ".opp";

/**
 * Constructor.
 */
OperationIndex::OperationIndex() {
}

/**
 * Destructor.
 *
 * Deletes all object state trees modeling module properties. Deletes all
 * modules.
 */
OperationIndex::~OperationIndex() {
    AssocTools::deleteAllValues(opDefinitions_);
    SequenceTools::deleteAllItems(modules_);
}

/**
 * Adds a path to search path list.
 *
 * When the path is added, all modules in that path are automatically scanned 
 * and recorded.
 *
 * @param path The path to be added.
 */
void
OperationIndex::addPath(const std::string& path) {
    
    ModuleTable::iterator iter = modulesInPath_.find(path);
    if (iter != modulesInPath_.end()) {
        return;
    }

    paths_.push_back(path);
    vector<string> modules;
    string pattern = path + FileSystem::DIRECTORY_SEPARATOR + 
        FileSystem::STRING_WILD_CARD + PROPERTY_FILE_EXTENSION;
 
    FileSystem::globPath(pattern, modules);
    
    vector<OperationModule*> opModules;
    for (unsigned int i = 0; i < modules.size(); i++) {
        string file = FileSystem::fileOfPath(modules[i]);
        OperationModule*  module = 
            new OperationModule(FileSystem::fileNameBody(file), path);
        modules_.push_back(module);
        opModules.push_back(module);
    }

    modulesInPath_[path] = opModules;
}

/**
 * Returns the module by the given index in a given path.
 *
 * @param i The index of wanted module.
 * @param path The path of the module
 * @return The module by the given index in a given path, or null module if 
 *         path is not found.
 * @exception OutOfRange If index i is out of range.
 * @exception PathNotFound If path is not found.
 */
OperationModule&
OperationIndex::module(int i, const std::string& path) 
    throw (OutOfRange, PathNotFound) {

    ModuleTable::iterator it = modulesInPath_.find(path);
    if (it == modulesInPath_.end()) {
        string msg = "Path for the module not found.";
        throw PathNotFound(__FILE__, __LINE__, __func__, msg, path);
    }
    
    if (i < 0 || i > static_cast<int>((*it).second.size()) - 1) {
        string msg = "Index out of range.";
        throw OutOfRange(__FILE__, __LINE__, __func__, msg);
    }

    OperationModule* module = ((*it).second)[i];
    return *module;
}

/**
 * Returns the number of modules in a path.
 *
 * @param path The path of the modules.
 * @return The number of modules in a path.
 * @exception PathNotFound If path is not found.
 */
int
OperationIndex::moduleCount(const std::string& path) const 
    throw (PathNotFound) {
    
    ModuleTable::const_iterator it = 
        modulesInPath_.find(path);
    if (it == modulesInPath_.end()) {
        string msg = "Path for the modules not found.";
        throw PathNotFound(__FILE__, __LINE__, __func__, msg, path);
    }
    return (*it).second.size();
}

/**
 * Adds a new module to OperationIndex.
 *
 * @param module Module to be added.
 * @param path Path in which module is located.
 * @exception PathNotFound If path is not found.
 */
void
OperationIndex::addModule(OperationModule* module, const std::string& path) 
    throw (PathNotFound) {
	
    ModuleTable::iterator it = modulesInPath_.find(path);
    if (it == modulesInPath_.end()) {
        string method = "OperationIndex::addModule()";
        string msg = "Path not found.";
        throw PathNotFound(__FILE__, __LINE__, method, msg, path);
    }
    modules_.push_back(module);
    (*it).second.push_back(module);
}

/**
 * Removes the module in a given path.
 *
 * @param path The name of the path.
 * @param modName The name of the module.
 * @exception PathNotFound If path is not found.
 */
void
OperationIndex::removeModule(
    const std::string& path, 
    const std::string& modName) 
    throw (PathNotFound, InstanceNotFound) {

    ModuleTable::iterator iter = modulesInPath_.find(path);
    if (iter == modulesInPath_.end()) {
        string msg = "Paths of the module not found.";
        throw PathNotFound(__FILE__, __LINE__, __func__, msg, path);
    }
    
    OperationModule* toBeErased = NULL;
    
    vector<OperationModule*>::iterator modIter = (*iter).second.begin();
    while (modIter != (*iter).second.end()) {
        if ((*modIter)->name() == modName) {
            toBeErased = *modIter;
            (*iter).second.erase(modIter);
            break;
        }
        modIter++;
    }

    if (toBeErased == NULL) {
	throw InstanceNotFound(
	    __FILE__, __LINE__, __func__, 
	    "Operation module " + path + ":" + modName + " not found.");
    }

    modIter = modules_.begin();
    while (modIter != modules_.end()) {
        if (*modIter == toBeErased) {
            modules_.erase(modIter);
            break;
        }
        modIter++;
    }
    
    // erase module from DefinitionTable
    DefinitionTable::iterator dIter = opDefinitions_.begin();
    while (dIter != opDefinitions_.end()) {
        if ((*dIter).first == toBeErased->propertiesModule()) {
            opDefinitions_.erase(dIter);
            delete (*dIter).second;
            break;
        }
        dIter++;
    }

    delete toBeErased;
}

/**
 * Refreshes module (usually when new operation is added to it).
 *
 * Refreshing is done by erasing the ObjectState tree of the module. That
 * way it has to be read again.
 *
 * @param path The name of the path.
 * @param modName The name of the module.
 * @exception PathNotFound If path is not found.
 */
void
OperationIndex::refreshModule(
    const std::string& path, 
    const std::string& modName) 
    throw (PathNotFound, InstanceNotFound) {
    
    ModuleTable::iterator modIter = modulesInPath_.find(path);
    if (modIter == modulesInPath_.end()) {
        string msg = "Path for the module not found.";
        throw PathNotFound(__FILE__, __LINE__, __func__, msg, path);
    }

    vector<OperationModule*>::iterator iter = (*modIter).second.begin();
    OperationModule* module = NULL;
    while (iter != (*modIter).second.end()) {
        if ((*iter)->name() == modName) {
            module = (*iter);
            break;
        }
        iter++;
    }

    if (module == NULL) {
	throw InstanceNotFound(
	    __FILE__, __LINE__, __func__, 
	    "Operation module " + path + ":" + modName + " not found.");
    }

    DefinitionTable::iterator it = 
        opDefinitions_.find(module->propertiesModule());
    if (it == opDefinitions_.end()) {
        return;
    } else {
        opDefinitions_.erase(it);
        delete (*it).second;
    }
}

/**
 * Returns the module in which a given operation is defined.
 *
 * If operation module is not found, a null operation module is returned.
 *
 * @param name The name of the operation.
 * @return The OperationModule in which operation is defined.
 */
OperationModule&
OperationIndex::moduleOf(const std::string& name) {
    
    // let's iterate through every module to search an operation
    for (unsigned int i = 0; i < paths_.size(); i++) {
        OperationModule& module = moduleOf(paths_[i], name);
        if (&module != &NullOperationModule::instance()) {
            return module;
        }
    }
    return NullOperationModule::instance();
}

/**
 * Returns the name of the operation by the given index in a given module.
 *
 * @param i The index of the operation.
 * @param om The OperationModule.
 * @return The name of the operation by the given index in a given module.
 * @exception OutOfRange If index i is out of range.
 * @exception BadOperationModule When module is invalid.
 */
string
OperationIndex::operationName(int i, const OperationModule& om) 
    throw (OutOfRange, BadOperationModule) {
    
    DefinitionTable::iterator it = opDefinitions_.find(om.propertiesModule());
    if (it == opDefinitions_.end()) {
        try {
            readOperations(om);
            it = opDefinitions_.find(om.propertiesModule());
        } catch (const SerializerException& s) {
            string msg = "Error when reading module: " + s.errorMessage();
            throw BadOperationModule(__FILE__, __LINE__, __func__, msg);
        }
    }
    
    ObjectState* op = (*it).second;
    ObjectState* child = op->child(i);
    return child->stringAttribute(Operation::OPRN_NAME);
}

/**
 * Returns the number of operations in a particular module.
 * 
 * @param om The OperationModule.
 * @return The number of operations in a module.
 * @exception BadOperationModule When module is invalid.
 */
int
OperationIndex::operationCount(const OperationModule& om) 
    throw (BadOperationModule) {
    
    DefinitionTable::iterator it = opDefinitions_.find(om.propertiesModule());
    if (it == opDefinitions_.end()) {
        try {
            readOperations(om);
            it = opDefinitions_.find(om.propertiesModule());
        } catch (const SerializerException& s) {
            string msg = "Error when reading module: " + s.errorMessage();
            throw BadOperationModule(__FILE__, __LINE__, __func__, msg);
        }
    }
    ObjectState* op = (*it).second;
    return op->childCount();
}

/**
 * Read all operation definitions of a module.
 *
 * @param module The operation module to be read operations from.
 * @exception SerializerException If reading fails.
 */
void
OperationIndex::readOperations(const OperationModule& module) 
    throw (SerializerException) {
    
    serializer_.setSourceFile(module.propertiesModule());
    ObjectState* tree = serializer_.readState();
    opDefinitions_[module.propertiesModule()] = tree;
}

/**
 * Searches for a module in which a given operation is defined all in a 
 * given path.
 *
 * If no module is found, NullOperationModule is returned. 
 *
 * @param path The name of the path.
 * @param operName The name of the operation.
 * @return The module in which operation is defined or NullOperationModule.
 */
OperationModule&
OperationIndex::moduleOf(
    const std::string& path, 
    const std::string& operName) {

    ModuleTable::const_iterator mt = modulesInPath_.find(path);
    if (mt == modulesInPath_.end()) {
        return NullOperationModule::instance();
    }
    
    vector<OperationModule*> mods = (*mt).second;
    
    // let's iterate through all modules in this path
    for (unsigned int j = 0; j < mods.size(); j++) {
        
        DefinitionTable::const_iterator dt = 
            opDefinitions_.find(mods[j]->propertiesModule());
        if (dt == opDefinitions_.end()) {
        
            // operations for this module are not yet read from XML file
            // let's read them now
            try {
                readOperations(*mods[j]);
                dt = opDefinitions_.find(mods[j]->propertiesModule());
            } catch (const SerializerException& s) {
                // error occurred in reading, let's keep searching
                continue;
            }
        }
        
        // let's go through all operations and try to find a specific 
        // one
        ObjectState* op = (*dt).second;
        for (int i = 0; i < op->childCount(); i++) {    
            ObjectState* child = op->child(i);
            string childName = child->stringAttribute(Operation::OPRN_NAME);
            string upName = StringTools::stringToUpper(childName);
            if (upName == StringTools::stringToUpper(operName)) {
                return *(mods[j]);
            }
        }
    }
    return NullOperationModule::instance();
}
