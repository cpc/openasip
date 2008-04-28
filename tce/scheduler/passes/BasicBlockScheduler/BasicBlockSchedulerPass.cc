/**
 * @file BasicBlockSchedulerPass.hh
 *
 * Implementation of BasicBlockSchedulerPass class.
 *
 * Basic block scheduler.
 *
 * @author Pekka J��skel�inen 2007 (pekka.jaaskelainen@tut.fi)
 * @note rating: red
 */

#include <typeinfo>


#include "BasicBlockSchedulerPass.hh"
#include "BasicBlockScheduler.hh"
#include "BypasserModule.hh"
#include "DSFillerModule.hh"

/**
 * Constructor.
 */
BasicBlockSchedulerPass::BasicBlockSchedulerPass() :
    StartableSchedulerModule(), bypasserModule_(NULL), fillerModule_(NULL) {
}

/**
 * Destructor.
 */
BasicBlockSchedulerPass::~BasicBlockSchedulerPass() {
}

/**
 * Schedules the assigned program.
 */
void
BasicBlockSchedulerPass::start()
    throw (Exception) {

    SoftwareBypasser* bypasser = NULL;
    CopyingDelaySlotFiller* filler = NULL;

    if (bypasserModule_ != NULL) {
        bypasser = &bypasserModule_->bypasser();
    }

    if (fillerModule_ != NULL) {
        filler = &fillerModule_->filler();
    }

    BasicBlockScheduler scheduler(interPassData(),bypasser, filler);
    scheduler.handleProgram(*program_, *target_);
}

/**
 * Registers a helper module to this module. 
 *
 * This can accept a software bypasser or a delay slot filler.
 * 
 * Later the delay slot filelr will become independent pass,
 * not a helper module to this.
 */
void
BasicBlockSchedulerPass::registerHelperModule(HelperSchedulerModule& module)
    throw (IllegalRegistration) {

    BypasserModule* swbModule = 
        dynamic_cast<BypasserModule*>(&module);
    if (swbModule != NULL) {
        bypasserModule_ = swbModule;
    }

    DSFillerModule* dsfModule = dynamic_cast<DSFillerModule*>(
        &module);
    if (dsfModule != NULL) {
        fillerModule_ = dsfModule;
    }
}

/**
 * A short description of the module, usually the module name,
 * in this case "BasicBlockSchedulerPass".
 *
 * @return The description as a string.
 */   
std::string
BasicBlockSchedulerPass::shortDescription() const {
    return "Startable: BasicBlockSchedulerPass";
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
BasicBlockSchedulerPass::longDescription() const {
    std::string answer = "Startable:  BasicBlockSchedulerPass\n";
    answer += "accepts: BypasserModule and DSFillerModule as parameters.";
    return answer;
}
