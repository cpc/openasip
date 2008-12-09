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
