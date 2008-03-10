/**
 * @file SchedulingPlan.hh
 *
 * Declaration of SchedulingPlan class.
 *
 * @author Ari Metsähalme 2005 (ari.metsahalme@tut.fi)
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
    static SchedulingPlan* loadFromFile(const std::string& configurationFile)
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
