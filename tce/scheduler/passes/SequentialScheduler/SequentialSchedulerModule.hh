/**
 * @file SequentialSchedulerModule.hh
 *
 * Declaration of SequentialSchedulerModule class.
 * 
 * Sequential scheduler which uses the new pass hierarchy,
 * resource manager and registercopyadder.
 * Does not use DDG.
 *
 * Runs quickly but creates very slow code, 
 * just for debugging/testing purposes.
 *
 * @author Heikki Kultala 2008 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_SEQUENTIAL_SCHEDULER_MODULE_HH
#define TTA_SEQUENTIAL_SCHEDULER_MODULE_HH

#include "StartableSchedulerModule.hh"

#include "SequentialScheduler.hh"

/**
 * Implementation of a simple sequential scheduling algorithm.
 */
class SequentialSchedulerModule : public StartableSchedulerModule {
public:
    SequentialSchedulerModule();
    virtual ~SequentialSchedulerModule();

    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;
};

SCHEDULER_PASS(SequentialSchedulerModule)

#endif
