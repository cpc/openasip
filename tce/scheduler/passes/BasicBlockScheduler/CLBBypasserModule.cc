/**
 *
 * @file CLBBypasserModule.cc
 * 
 * Implementation of CLBBypasserModule class
 *
 * Helper scheduler module for CycleLookBackSoftwareBypasser class.
 * Creates a CycleLookBackSoftwareBypasser which is the actual bypasser class.
 *
 * @author Heikki Kultala 2007 (heikki.kultala@tut.fi)
 * @note rating: red
 */
#include <cstdlib>

#include "ObjectState.hh"

#include "CLBBypasserModule.hh"
#include "Conversion.hh"
#include "CycleLookBackSoftwareBypasser.hh"

// default value when nothing given as parameter or environment variable.
#define BYPASS_DISTANCE 1

CLBBypasserModule::CLBBypasserModule() : clbs_(NULL) {
    bypassDistance_ = BYPASS_DISTANCE;
}

CLBBypasserModule::~CLBBypasserModule() {
    if (clbs_) {
        delete clbs_; clbs_ = NULL;
    }
}

/**
 * Sets options for the plugin.
 *
 * Currently this only recognizes the bypas lookup distance option.
 *
 * @param options all options given to the schdeduler pass.
 */ 
void CLBBypasserModule::setOptions(
    const std::vector<ObjectState*>& options) {

    for (std::vector<ObjectState*>::const_iterator iter = 
             options.begin(); iter != options.end(); iter++) {
        if ((*iter)->hasAttribute("name") && (*iter)->stringAttribute("name") == LOOKBACK_DISTANCE_OPTION) {
            bypassDistance_ = (*iter)->intValue();
        }
    }
}

SoftwareBypasser& 
CLBBypasserModule::bypasser() {

    const char* ENVIRONMENT_LBD = getenv("TCE_BYPASS_LBD");
    
    if (clbs_ == NULL) {
        if (ENVIRONMENT_LBD == NULL) {
            clbs_ = new CycleLookBackSoftwareBypasser(bypassDistance_);
        } else {
           const int lbd = Conversion::toInt(ENVIRONMENT_LBD);
           clbs_ = new CycleLookBackSoftwareBypasser(lbd);
        }
    }
    return *clbs_;
}

const std::string CLBBypasserModule::LOOKBACK_DISTANCE_OPTION = "swb-lookback-distance";
SCHEDULER_PASS(CLBBypasserModule)
