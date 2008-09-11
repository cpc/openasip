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
 * @file SchedulerPluginLoader.hh
 *
 * Declaration of SchedulerPluginLoader class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma-no.spam-tut.fi)
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
