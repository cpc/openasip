/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file PluginTools.cc
 *
 * Definition of PluginTools class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 19 May 2004 by ml, jn, ao, am
 * @note rating: green
 */

#include <string>
#include <vector>
#include <dlfcn.h>

// this is not defined with all compilers
#ifndef RTLD_LOCAL
#define RTLD_LOCAL 0
#endif

#include <iostream>

#include "PluginTools.hh"
#include "FileSystem.hh"
#include "MapTools.hh"
#include "Exception.hh"
#include "ContainerTools.hh"
#include "StringTools.hh"

using std::string;
using std::vector;
using std::cout;
using std::endl;


/**
 * Constructor.
 *
 * The created instance has no search paths registered.
 *
 * @param modeLazyGlobal True if symbol resolution must be lazy and binding
 * global. Default is false: symbols are resolved immediately with local
 * binding.
 *
 * @note Throwing C++ exceptions from plugin to the loader might not work
 * with the lazy resolution! It's probably safest to set the argument to
 * "false".
 */
PluginTools::PluginTools(bool modeLazyGlobal):
    lazyResolution_(modeLazyGlobal) {
}


/**
 * Destructor.
 *
 * Unregisters all modules.
 */
PluginTools::~PluginTools() {
    unregisterAllModules();
}


/**
 * Adds a new search path in search path list.
 *
 * @param searchPath The search path to be added.
 * @exception FileNotFound If path doesn't exist.
 */
void
PluginTools::addSearchPath(const std::string& searchPath)
    throw (FileNotFound) {

    if (!(FileSystem::fileExists(searchPath))) {
        string method = "PluginTools::addSearchPath()";
        string message = "Path doesn't exist";
        throw FileNotFound(__FILE__, __LINE__, method, message);
    }
    searchPaths_.push_back(searchPath);
}


/**
 * Removes search path from search path list if it is found.
 *
 * @param searchPath the search path to be removed.
 */
void
PluginTools::removeSearchPath(const std::string& searchPath) {
    ContainerTools::removeValueIfExists(searchPaths_, searchPath);
}


/**
 * Clears all search paths from search path list.
 */
void
PluginTools::clearSearchPaths() {
    searchPaths_.clear();
}


/**
 * Registers a module in registry.
 *
 * If module is not an absolute path, then it is searched from search
 * paths added with addSearchPath() in the order of addition. First
 * module found is used. If module already exists in the registry,
 * it's not opened again. RTLD_LOCAL and RTLD_NOW flags are used by
 * default when opening the module.
 *
 * @param module The module to be registered.
 * @exception FileNotFound If module is not found.
 * @exception DynamicLibraryException if dlopen() fails.
 */
void
PluginTools::registerModule(const std::string& module)
    throw (FileNotFound, DynamicLibraryException) {

    if (module.empty()) {
        string method = "PluginTools::registerModule()";
        string message = "Empty module file name.";
        throw FileNotFound(__FILE__, __LINE__, method, message);
    }

    const string DIR_SEP = FileSystem::DIRECTORY_SEPARATOR;

    string path = module;                     
    if (!FileSystem::isAbsolutePath(module)) {

        bool moduleFound = false;

        // module is not an absolute path
        for (unsigned int i = 0; i < searchPaths_.size(); i++) {
            path = searchPaths_[i] + DIR_SEP + module;
            if (FileSystem::fileExists(path)) {
                moduleFound = true;                   
                break;
            }
        }

        if (!moduleFound) {
            string method = "PluginTools::registerModule()";
            string message = "Module not found";
            throw FileNotFound(__FILE__, __LINE__, method, message);
        }
    }

    // multiple registrations are ignored
    if (!MapTools::containsKey(modules_, path)) {

        void* handle = NULL;

        if (lazyResolution_) {
            handle = dlopen(path.c_str(), RTLD_GLOBAL | RTLD_LAZY);
        } else {
            handle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);
        }

        if (handle == NULL) {
            string method = "PluginTools::registerModule()";
            string message = dlerror();
            throw DynamicLibraryException(__FILE__, __LINE__, method, message);
        }

        modules_.insert(ValType(path, handle));
    }
}


/**
 * A module is closed using dlclose() and it is removed from registry.
 *
 * @param module The module to be unregistered.
 * @exception FileNotFound If module is not found.
 * @exception DynamicLibraryException If dlclose() fails.
 * @exception MultipleInstancesFound If multiple modules are found.
 */
void
PluginTools::unregisterModule(const std::string& module)
    throw (FileNotFound, DynamicLibraryException, MultipleInstancesFound) {

    string method = "PluginTools::unregisterModule()";
    string path = module;
    if (!FileSystem::isAbsolutePath(module)) {
        path = findModule(module);
    }

    MapIter mt = modules_.find(path);
    if (mt == modules_.end()) {
        string msg = "Module not found";
        throw FileNotFound(__FILE__, __LINE__, method, msg);
    }

    void* handle = (*mt).second;
    modules_.erase(mt);
    if (dlclose(handle) != 0) {
        string message = dlerror();
        throw DynamicLibraryException(__FILE__, __LINE__, method, message);
    }
}


/**
 * Unregisters all modules.
 *
 * All modules are closed by using dlclose() and registry is emptied.
 *
 * @exception DynamicLibraryException If dlclose() fails.
 */
void
PluginTools::unregisterAllModules()
    throw (DynamicLibraryException) {

    MapIter mt;
    for (mt = modules_.begin(); mt != modules_.end(); mt++) {
        void* handle = (*mt).second;
        std::string moduleName = (*mt).first;
        dlclose(handle);
	// Removing check. This is called on the global destructor
	// stage and in some systems (FreeBSD) libraries are already
	// unloaded by then which would cause this to fail unneccessarily.
//         if (dlclose(handle) != 0) {
//             string method = "PluginTools::unregisterAllModules()";
//             string message = dlerror();
//             throw DynamicLibraryException(__FILE__, __LINE__, method, message);
//         }
    }
    modules_.clear();
}


/**
 * Loads symbol from module.
 *
 * If module is not given, then all modules are searched in order to
 * find the symbol. Note that the search order of modules is not necessarily
 * the same as the registration order!
 *
 * @param symbolName The symbol being loaded.
 * @param module The module where symbol is loaded.
 * @return Pointer to loaded symbol.
 * @exception MultipleInstancesFound If multiple instances of module are
 *                                   found.
 * @exception DynamicLibraryException If dlsym() fails.
 * @exception FileNotFound If module is not found.
 * @exception SymbolNotFound If symbol to be loaded is not found.
 */
void*
PluginTools::loadSym(const std::string& symbolName, const std::string& module)
    throw (MultipleInstancesFound, DynamicLibraryException, FileNotFound,
           SymbolNotFound) {

    string path = module;
    if (module != "") {

        if (!FileSystem::isAbsolutePath(module)) {
            try {
                path = findModule(module);
            } catch (const FileNotFound& f) {
                // file was not found, so let's try to register it.
                registerModule(module);
                path = findModule(module);
            }
        } else {
            if (!MapTools::containsKey(modules_, path)) {
                registerModule(path);
            }
        }

        MapIter mt = modules_.find(path);
        void* handle = (*mt).second;
        const char* error = NULL;

        // clear possible old errors
        dlerror();

        void* sym = dlsym(handle, symbolName.c_str());
        if ((error = dlerror()) != NULL) {
            if (sym == NULL) {
                // it does not seem to be possible to separate the
                // symbol not found error from other errors, thus this will 
                // probably always throw SymbolNotFound in case the symbol
                // could not be loaded for any reason
                string message = "Symbol not found: ";
                message += symbolName;
                throw SymbolNotFound(__FILE__, __LINE__, __func__, message);
            } else {
                throw DynamicLibraryException(
                    __FILE__, __LINE__, __func__, error);
            }
        }

        return sym;

    } else {

        // seek all registered modules for the symbol and return the first
        // one found
        for (MapIter mt = modules_.begin(); mt != modules_.end(); mt++) {
            void* handle = (*mt).second;
            const char* error = NULL;
            dlerror();
            void* sym = dlsym(handle, symbolName.c_str());
            if ((error = dlerror()) == NULL) {
                return sym;
            }
        }

        // symbol was not found, exception is thrown
        string method = "PluginTools::loadSym()";
        string message = "Symbol not found";
        throw SymbolNotFound(__FILE__, __LINE__, method, message);
    }

    return NULL;
}


/**
 * Looks for the path for the module and returns it if it is found.
 *
 * @param module The module to be searched for.
 * @return The path for the module.
 * @exception MultipleInstacesFound If multiple instances of module are found.
 * @exception FileNotFOund If module is not found at all.
 */
string
PluginTools::findModule(const std::string& module)
    throw (MultipleInstancesFound, FileNotFound) {

    string path = "";
    bool moduleFound = false;
    for (MapIter mt = modules_.begin(); mt != modules_.end(); mt++) {

        const string fullPath = (*mt).first;
        const string fileName = FileSystem::fileOfPath(fullPath);
        if (module == fullPath ||
            module == fileName ||
            StringTools::endsWith(fullPath, std::string("/") + module)) {

            if (moduleFound) {
                string method = "PluginTools::findModule()";
                string message = "Multiple modules found";
                throw MultipleInstancesFound(
                    __FILE__, __LINE__, method, message);
            }

            path = (*mt).first;
            moduleFound = true;
        }
    }

    if (!moduleFound) {
        string method = "PluginTools::findModule()";
        string message = "Module not found";
        throw FileNotFound(__FILE__, __LINE__, method, message);
    }

    return path;
}
