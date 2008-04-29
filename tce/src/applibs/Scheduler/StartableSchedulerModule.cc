/**
 * @file StartableSchedulerModule.cc
 *
 * Implementation of StartableSchedulerModule class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme@tut.fi)
 * @note rating: red
 */

#include "StartableSchedulerModule.hh"

/**
 * Constructor.
 */
StartableSchedulerModule::StartableSchedulerModule(): BaseSchedulerModule() {
}

/**
 * Destructor.
 */
StartableSchedulerModule::~StartableSchedulerModule() {
}

/**
 * Return true if this module can be run independently.
 *
 * Startable does not mean that, if started, the module won't fail due
 * to some failure conditions.
 *
 * @return True always, because this type of module is supposed to be run
 * independently.
 */
bool
StartableSchedulerModule::isStartable() const {
    return true;
}

/**
 * A short description of the module.
 *
 * @return The description as a string.
 */   
std::string
StartableSchedulerModule::shortDescription() const {
    return "Generic startable module. Detailed description is missing.";
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
StartableSchedulerModule::longDescription() const {
    return "Generic startable module. Detailed description is missing.";
}
