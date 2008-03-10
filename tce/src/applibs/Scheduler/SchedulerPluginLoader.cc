/**
 * @file SchedulerPluginLoader.cc
 *
 * Implementation of SchedulerPluginLoader class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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


