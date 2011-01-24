/**
 * @file PreOptimizerModule.cc
 *
 * Implementation of PreOptimizerModule class.
 *
 * DDG based pre-optimizer. gets rid of unnecessary xors before guarded 
 * operations by changing guard. Also changes some address registers.
 *
 * @author Heikki Kultala 2009 (hkultala@cs.tut.fi)
 * @note rating: red
 */

#include "ObjectState.hh"

#include "PreOptimizerModule.hh"
#include "PreOptimizer.hh"

/**
 * Schedules the assigned program.
 */
void
PreOptimizerModule::start()
    throw (Exception) {

    PreOptimizer gi(interPassData());
    gi.handleProgram(*program_, *target_);
}

/**
 * A short description of the module, usually the module name,
 *
 * @return The description as a string.
 */   
std::string
PreOptimizerModule::shortDescription() const {
    return "Startable: Guard inverter.";
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
PreOptimizerModule::longDescription() const {
    std::string answer = "Startable:  Guard Inverter\n";
    answer += "Optimized away some guard inversion operations";
    return answer;
}

SCHEDULER_PASS(PreOptimizerModule)
