/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file OperationIndex.cc
 *
 * Definition of OperationIndex class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2015
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
#include "OperationBuilder.hh"
#include "OperationBehaviorProxy.hh"
#include "OperationBehaviorLoader.hh"

using std::map;
using std::string;
using std::vector;

const string OperationIndex::PROPERTY_FILE_EXTENSION = ".opp";

/**
 * Constructor.
 */
OperationIndex::OperationIndex() : loader_(*this) {
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
    SequenceTools::deleteAllItems(proxies_);
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
        string behaviourFile = modules[i];
        string behaviourSourceFile = 
            behaviourFile.substr(0, behaviourFile.length() - 3) + "cc";
        
        // behaviour is .opb , change last letter from p to b
        *(behaviourFile.rbegin()) = 'b';
        
        // load only modules which have behaviour file.
        if (!FileSystem::fileExists(behaviourFile) &&
            FileSystem::fileExists(behaviourSourceFile)) {
            OperationBuilder& opBuilder = OperationBuilder::instance();
            std::vector<std::string> output;
            bool buildOk = opBuilder.buildObject(
                file.substr(0, file.length()-4), behaviourSourceFile, 
                path, output);
            if (!buildOk || !FileSystem::fileExists(behaviourFile)) {
                std::cerr << "Warning: Found operation module specification "
                          << "file " << modules[i] << " and operation "
                          << "behavious source file " << behaviourSourceFile
                          << " without compiled behaviour "
                          << "implementation file "
                          << behaviourFile << "." << std::endl;
                std::cerr << "Tried to compile behaviour impelementaton "
                          << "file, but the compilation failed to error: "
                          << std::endl;
                for (unsigned int j = 0; j < output.size(); j++) {
                    std::cerr << output[j] << std::endl;
                }
                std::cerr << "This may cause program to hang if operation "
                          << "in this module is attempted to be simulated."
                          << std::endl;
            }
        }
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
OperationIndex::module(int i, const std::string& path) {
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
OperationIndex::moduleCount(const std::string& path) const {
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
OperationIndex::addModule(OperationModule* module, const std::string& path) {
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
    const std::string& path, const std::string& modName) {
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
    const std::string& path, const std::string& modName) {
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
        delete (*it).second;
        opDefinitions_.erase(it);
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
OperationIndex::operationName(int i, const OperationModule& om) {
    DefinitionTable::iterator it = opDefinitions_.find(om.propertiesModule());
    if (it == opDefinitions_.end()) {
        try {
            readOperations(om);
            it = opDefinitions_.find(om.propertiesModule());
        } catch (const SerializerException& s) {
            brokenModules_.insert(&om);
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
OperationIndex::operationCount(const OperationModule& om) {
    DefinitionTable::iterator it = opDefinitions_.find(om.propertiesModule());
    if (it == opDefinitions_.end()) {
        try {
            readOperations(om);
            it = opDefinitions_.find(om.propertiesModule());
        } catch (const SerializerException& s) {
            brokenModules_.insert(&om);
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
OperationIndex::readOperations(const OperationModule& module) {
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

        if (brokenModules_.count(mods[j]))
            continue;
        DefinitionTable::const_iterator dt = 
            opDefinitions_.find(mods[j]->propertiesModule());
        if (dt == opDefinitions_.end()) {
        
            // operations for this module are not yet read from XML file
            // let's read them now
            try {
                readOperations(*mods[j]);
                dt = opDefinitions_.find(mods[j]->propertiesModule());
            } catch (const SerializerException& s) {
                brokenModules_.insert(mods[j]);
                // error occurred in reading, let's keep searching
                continue;
            }
        }
        
        // let's go through all operations and try to find a specific 
        // one
        ObjectState* op = (*dt).second;
        for (int i = 0; i < op->childCount(); i++) {    
            ObjectState* child = op->child(i);
            TCEString childName = child->stringAttribute(Operation::OPRN_NAME);
            if (childName.ciEqual(operName)) {
                return *(mods[j]);
            }
        }
    }
    return NullOperationModule::instance();
}

/**
 * Returns a new instance of the Operation with the given
 * name that is 'effective' based on the search path priorities.
 */
Operation*
OperationIndex::effectiveOperation(const TCEString& name) {

    OperationModule& mod = moduleOf(name);

    DefinitionTable::const_iterator dt = 
        opDefinitions_.find(mod.propertiesModule());

    assert (dt != opDefinitions_.end());

    ObjectState* root = (*dt).second;
    ObjectState* child = NULL;
    
    // load operations
    for (int i = 0; i < root->childCount(); i++) {
        child = root->child(i);
        const TCEString operName = 
            root->child(i)->stringAttribute(Operation::OPRN_NAME);
      
        /* Do not load all operations in the module because the user
           might have overridden some of the operation (properties)
           in a local search path with higher order. Just load the one 
           requested. */
        if (!operName.ciEqual(name))
            continue;
        
        Operation* oper = 
            new Operation(operName, NullOperationBehavior::instance());
      
        oper->loadState(child);
        // add the behavior loader proxy
        OperationBehaviorProxy* proxy = 
            new OperationBehaviorProxy(*oper, loader_);
        proxies_.push_back(proxy);
        oper->setBehavior(*proxy);
        return oper;
    }
    return NULL;
}
