/**
 * @file BasicBlockSchedulerPass.hh
 *
 * Declaration of BasicBlockSchedulerPass class.
 * 
 * Basic block scheduler.
 *
 * @author Pekka J��skel�inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASIC_BLOCK_SCHEDULER_PASS_HH
#define TTA_BASIC_BLOCK_SCHEDULER_PASS_HH

#include <map>
#include <vector>
#include <set>
#include <list>

#include "StartableSchedulerModule.hh"

#include "BasicBlockScheduler.hh"

class BypasserModule;
class DSFillerModule;
/**
 * Implementation of a simple basic block scheduling algorithm.
 */
class BasicBlockSchedulerPass : public StartableSchedulerModule {
public:
    BasicBlockSchedulerPass();
    virtual ~BasicBlockSchedulerPass();

    virtual void start()        
        throw (Exception);
    bool needsProgram() const { return true; }
    bool needsTarget() const { return true; }

    void registerHelperModule(HelperSchedulerModule& module) 
        throw (IllegalRegistration);

    virtual std::string shortDescription() const;
    virtual std::string longDescription() const;
    
    BypasserModule* bypasserModule_;
    DSFillerModule* fillerModule_;
};

SCHEDULER_PASS(BasicBlockSchedulerPass)

#endif
