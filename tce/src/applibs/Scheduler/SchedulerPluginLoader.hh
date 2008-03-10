/**
 * @file SchedulerPluginLoader.hh
 *
 * Declaration of SchedulerPluginLoader class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULER_PLUGIN_LOADER_HH
#define TTA_SCHEDULER_PLUGIN_LOADER_HH

#include <string>
#include <vector>

#include "Exception.hh"
#include "PluginTools.hh"

class BaseSchedulerModule;
class ObjectState;

/**
 * Specialised abstraction layer to dynamically load pass modules of the
 * code transformation and scheduling chain.
 *
 * A loader is responsible for the plug-in modules it loads, and will
 * destroy them when it is destroyed. Can be used by pass modules to
 * internally load helper modules not explicitly declared in the chain.
 * Signleton class.
 */
class SchedulerPluginLoader {
public:
    static SchedulerPluginLoader& instance();
    virtual ~SchedulerPluginLoader();

    BaseSchedulerModule& loadModule(
        const std::string& modName, const std::string& fileName,
        const std::vector<ObjectState*>& options = emptyOptions_)
        throw (DynamicLibraryException);
protected:
    SchedulerPluginLoader();
private:
    /// Scheduler module constructor functions.
    typedef BaseSchedulerModule* (SchedulerPluginCreator)();
    /// Scheduler module destructor functions.
    typedef void (SchedulerPluginDestructor)(BaseSchedulerModule&);
    /// Map for loaded plugins and their destructor functions.
    typedef std::map<BaseSchedulerModule*,
                     SchedulerPluginDestructor*> PluginMap;

    /// Copying forbidden.
    SchedulerPluginLoader(const SchedulerPluginLoader&);
    /// Assignment forbidden.
    SchedulerPluginLoader& operator=(const SchedulerPluginLoader&);

    /// Plugin tool for loading scheduler pass modules.
    PluginTools pluginTool_;
    /// Loaded plugin modules.
    PluginMap loadedPlugins_;
    /// Unique instance of SchedulerPluginLoader.
    static SchedulerPluginLoader instance_;
    static const std::vector<ObjectState*> emptyOptions_;
};

#endif
