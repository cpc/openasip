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
 * @file SchedulingPlan.hh
 *
 * Declaration of SchedulingPlan class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Vladimír Guzma 2008 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_SCHEDULING_PLAN_HH
#define TTA_SCHEDULING_PLAN_HH

#include <vector>
#include "Exception.hh"

class SchedulerPluginLoader;
class ObjectState;
class StartableSchedulerModule;
class HelperSchedulerModule;

/**
 * Constructs the scheduling chain from an object tree representation
 * of the scheduler configuration file.
 */
class SchedulingPlan {
public:
    SchedulingPlan();
    virtual ~SchedulingPlan();

    void build(const ObjectState& conf)
        throw (ObjectStateLoadingException, DynamicLibraryException);
    int passCount() const;
    StartableSchedulerModule& pass(int index) const throw (OutOfRange);
    int helperCount(int index) const throw (OutOfRange);
    HelperSchedulerModule& helper(int passIndex, int helperIndex) const
        throw (OutOfRange);
    static SchedulingPlan* loadFromFile(
        const std::string& configurationFile)
        throw (Exception);

private:
    /// Copying forbidden.
    SchedulingPlan(const SchedulingPlan&);
    /// Assignment forbidden.
    SchedulingPlan& operator=(const SchedulingPlan&);

    /// List for scheduler passes.
    typedef std::vector<StartableSchedulerModule*> PassList;
    /// List for helper modules.
    typedef std::vector<HelperSchedulerModule*> HelperList;
    /// The schedler plugin loader.
    SchedulerPluginLoader* pluginLoader_;
    /// The scheduler passes.
    PassList passes_;
    /// Helper modules for each pass module.
    std::vector<HelperList> helpers_;
};

#endif
