/**
 * @file SequentialSchedulerModule.hh
 *
 * Implementation of SequentialSchedulerModule class.
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

#include <typeinfo>


#include "SequentialSchedulerModule.hh"
#include "BasicBlockScheduler.hh"
#include "BypasserModule.hh"
#include "DSFillerModule.hh"

/**
 * Constructor.
 */
SequentialSchedulerModule::SequentialSchedulerModule() :
    StartableSchedulerModule() {
}

/**
 * Destructor.
 */
SequentialSchedulerModule::~SequentialSchedulerModule() {
}

/**
 * Schedules the assigned program.
 */
void
SequentialSchedulerModule::start()
    throw (Exception) {

    SequentialScheduler scheduler(interPassData());
    scheduler.handleProgram(*program_, *target_);
}


/**
 * A short description of the module, usually the module name,
 * in this case "SequentialSchedulerModule".
 *
 * @return The description as a string.
 */   
std::string
SequentialSchedulerModule::shortDescription() const {
    return "Startable: SequentialSchedulerModule";
}

/**
 * Optional longer description of the Module.
 *
 * This description can include usage instructions, details of choice of
 * helper modules, etc.
 *
 * @return The description as a string.
 */
std::string
SequentialSchedulerModule::longDescription() const {
    std::string answer = "Startable:  SequentialSchedulerModule\n";
    return answer;
}
