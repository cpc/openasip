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
 * @file SchedulerPluginLoader.cc
 *
 * Implementation of SchedulerPluginLoader class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "SchedulerPluginLoader.hh"
#include "BaseSchedulerModule.hh"
#include "Environment.hh"
#include "FileSystem.hh"

using std::string;
using std::pair;

SchedulerPluginLoader SchedulerPluginLoader::instance_;

/**
 * Constructor.
 */
SchedulerPluginLoader::SchedulerPluginLoader() :
    pluginTool_(false) {
}

/**
 * Destructor.
 *
 * Manages deallocation of all loaded plug-in modules.
 */
SchedulerPluginLoader::~SchedulerPluginLoader() {

    for (PluginMap::iterator iter = loadedPlugins_.begin();
         iter != loadedPlugins_.end(); iter++) {

        BaseSchedulerModule* module = (*iter).first;
        SchedulerPluginDestructor* pluginDestructor = (*iter).second;
        pluginDestructor(*module);
    }

    loadedPlugins_.clear();
}

/**
 * Returns an instance of SchedulerPluginLoader class (singleton).
 *
 * @return Singleton instance of SchedulerPluginLoader class.
 */
SchedulerPluginLoader&
SchedulerPluginLoader::instance() {
    return instance_;
}

/**
 * Load a pass module of the code transformation and scheduling chain from
 * the given file and return a handle to it.
 *
 * Deallocation of the loaded plugin module will be handled by the plugin
 * loader instance.
 *
 * @param modName Name of the plug-in pass module.
 * @param fileName Name of the file that contains the plug-in module.
 * @return A handle to the loaded plug-in module.
 * @exception DynamicLibraryException If an error occurs loading the
 * requested plug-in module.
 */
BaseSchedulerModule&
SchedulerPluginLoader::loadModule(
    const std::string& modName, const std::string& fileName,
    const std::vector<ObjectState*>& options)
    throw (DynamicLibraryException) {

    try {

        string moduleName = FileSystem::findFileInSearchPaths(
            Environment::schedulerPluginPaths(), fileName);

        SchedulerPluginCreator* pluginCreator;
        pluginTool_.importSymbol(
            SCHEDULER_MODULE_CREATOR + modName, pluginCreator,
            moduleName);
        BaseSchedulerModule* module = pluginCreator();
        if (!options.empty()) {
            module->setOptions(options);
        }
        SchedulerPluginDestructor* pluginDestructor;
        pluginTool_.importSymbol(
            SCHEDULER_MODULE_DESTRUCTOR + modName, pluginDestructor,
            moduleName);

        if (Application::verboseLevel() >=
            Application::VERBOSE_LEVEL_INCREASED) {
            Application::logStream() << "Loading module: " <<
                module->shortDescription() << std::endl;
        }

        loadedPlugins_.insert(
            pair<BaseSchedulerModule*, SchedulerPluginDestructor*>(
                module, pluginDestructor));

        return *module;

    } catch (const FileNotFound& e) {
        const string method = "SchedulerPluginLoader::loadModule()";
        const string msg = "Plugin '" + fileName + "' not found.";
        throw DynamicLibraryException(__FILE__, __LINE__, method, msg);

    } catch (const MultipleInstancesFound& e) {
        const string method = "SchedulerPluginLoader::loadModule()";
        const string msg =
            "Multiple instances of plugin '" + fileName + "' found.";
        throw DynamicLibraryException(__FILE__, __LINE__, method, msg);

    } catch (const DynamicLibraryException& e) {
        const string method = "SchedulerPluginLoader::loadModule()";
        const string msg = "Error loading plugin '" + modName + "' "
            "from file '" + fileName + "': " + e.errorMessage();
        throw DynamicLibraryException(__FILE__, __LINE__, method, msg);

    } catch (const Exception& e) {
        const string method = "SchedulerPluginLoader::loadModule()";
        const string msg = "Error loading plugin '" + modName + "' "
            "from file '" + fileName + "'.";
        throw DynamicLibraryException(__FILE__, __LINE__, method, msg);
    }
}

const std::vector<ObjectState*> SchedulerPluginLoader::emptyOptions_;
