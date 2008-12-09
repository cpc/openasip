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
 * @file BaseSchedulerModule.hh
 *
 * Declaration of BaseSchedulerModule class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASE_SCHEDULER_MODULE_HH
#define TTA_BASE_SCHEDULER_MODULE_HH

#include <string>
#include <vector>

#include "Exception.hh"

namespace TTAMachine {
    class Machine;
}

namespace TTAProgram {
    class Program;
}

class ObjectState;
class SchedulerPluginLoader;
class HelperSchedulerModule;
class InterPassData;

/**
 * Base class for scheduler plugin modules.
 */
class BaseSchedulerModule {
public:
    virtual ~BaseSchedulerModule();

    /**
     * Return true if this module can be run independently.
     *
     * Startable does not mean that, if started, the module won't fail due
     * to some failure conditions.
     *
     * @return True if this module can be run independently by the means
     *         of the start method.
     */
    virtual bool isStartable() const = 0;

    /**
     * Run the module.
     *
     * The module will complete all its operations and return control to
     * the caller.
     */
    virtual void start()
        throw (Exception) = 0;

    virtual bool needsTarget() const;
    void setTarget(const TTAMachine::Machine& target);
    virtual bool needsProgram() const;
    void setProgram(TTAProgram::Program& program);
    virtual bool needsProgramRepresentation() const;

    virtual bool needsPluginLoader() const;
    void setPluginLoader(const SchedulerPluginLoader& loader);

    virtual void registerHelperModule(HelperSchedulerModule& module)
        throw (IllegalRegistration);

    void setInterPassData(InterPassData& data);
    InterPassData& interPassData();

    virtual void setOptions(
        const std::vector<ObjectState*>& options);

    virtual std::string shortDescription() const = 0;
    virtual std::string longDescription() const;
    
protected:
    BaseSchedulerModule();

    /// Target machine.
    const TTAMachine::Machine* target_;
    /// Source program.
    TTAProgram::Program* program_;
    /// Plugin loader.
    const SchedulerPluginLoader* loader_;
    /// The inter pass data.
    InterPassData* interPassData_;

private:
    /// Copying forbidden.
    BaseSchedulerModule(const BaseSchedulerModule&);
    /// Assignment forbidden.
    BaseSchedulerModule& operator=(const BaseSchedulerModule&);
};

/// Base name of the function loaded at run time to instantiate new
/// scheduler module classes. The underscore is followed with the
/// name of the plugin.
const std::string SCHEDULER_MODULE_CREATOR = "createScheduler_";
/// Base name of the function loaded at run time to deallocate
/// existing instances of scheduler module classes.
const std::string SCHEDULER_MODULE_DESTRUCTOR = "deleteScheduler_";


#define SCHEDULER_PASS(PLUGIN_NAME__)                                 \
    extern "C" {                                                      \
        BaseSchedulerModule* createScheduler_##PLUGIN_NAME__() {      \
            return new PLUGIN_NAME__();                               \
        }                                                             \
        void deleteScheduler_##PLUGIN_NAME__(                         \
            BaseSchedulerModule* target) {                            \
            delete target;                                            \
        }                                                             \
    }

#endif
