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
