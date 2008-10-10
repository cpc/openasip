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
 *
 * @file CLBBypasserModule.cc
 * 
 * Implementation of CLBBypasserModule class
 *
 * Helper scheduler module for CycleLookBackSoftwareBypasser class.
 * Creates a CycleLookBackSoftwareBypasser which is the actual bypasser class.
 *
 * @author Heikki Kultala 2007 (heikki.kultala-no.spam-tut.fi)
 * @note rating: red
 */
#include <cstdlib>

#include "ObjectState.hh"

#include "CLBBypasserModule.hh"
#include "Conversion.hh"
#include "CycleLookBackSoftwareBypasser.hh"

// default value when nothing given as parameter or environment variable.
#define BYPASS_DISTANCE 1

CLBBypasserModule::CLBBypasserModule() : bypassDistance_(BYPASS_DISTANCE), 
                                         killDeadResults_(true),
                                         clbs_(NULL) {
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
        if ((*iter)->hasAttribute("name") && 
            (*iter)->stringAttribute("name") == LOOKBACK_DISTANCE_OPTION) {
            bypassDistance_ = (*iter)->intValue();
        }
        if ((*iter)->hasAttribute("name") && 
            (*iter)->stringAttribute("name") == KILL_DEAD_RESULTS_OPTION) {
            if ( (*iter)->stringValue() == "true" ) {
                killDeadResults_ = true;
                continue;
            }
            if ( (*iter)->stringValue() == "false" ) {
                killDeadResults_ = false;
                continue;
            }
            throw IllegalParameters(
                __FILE__,__LINE__,__func__, 
                "only true or false value allowed for " +
                KILL_DEAD_RESULTS_OPTION);
        }
    }
}

SoftwareBypasser& 
CLBBypasserModule::bypasser() {

    const char* ENVIRONMENT_LBD = getenv("TCE_BYPASS_LBD");
    const char* ENVIRONMENT_KDR = getenv("TCE_BYPASS_DRE");
    
    if (clbs_ == NULL) {
        if (ENVIRONMENT_LBD != NULL && *ENVIRONMENT_LBD != 0) {
            bypassDistance_ = Conversion::toInt(ENVIRONMENT_LBD); 
        }
        if (ENVIRONMENT_KDR != NULL && *ENVIRONMENT_KDR != 0) {
            if (!strcmp(ENVIRONMENT_KDR,"true")) {
                killDeadResults_ = true;
            } else if (!strcmp(ENVIRONMENT_KDR,"false")) {
                killDeadResults_ = false;
            } else {
                throw IllegalParameters(
                    __FILE__,__LINE__,__func__,"Illegal value in "
                    "TCE_BYPASS_DRE environment variable. "
                    "Only true/false allowed");
            }
        }

        clbs_ = new CycleLookBackSoftwareBypasser(
            bypassDistance_,killDeadResults_);
    }
    return *clbs_;
}
/**
 * A short description of the module, usually the module name,
 * in this case "CLBBypasserModule".
 *
 * @return The description as a string.
 */   
std::string
CLBBypasserModule::shortDescription() const {
    return "Helper: CLBBypasserModule";
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
CLBBypasserModule::longDescription() const {
    std::string answer ="Helper: CLBBypasserModule. ";
    answer += "Implements Cycle look back software bypassing.";
    answer += " Accepts parameter \"swb-lookback-distance\"=<uint>, default is 1.";    
    return answer;
}

const std::string CLBBypasserModule::LOOKBACK_DISTANCE_OPTION = "swb-lookback-distance";
const std::string CLBBypasserModule::KILL_DEAD_RESULTS_OPTION = "kill-dead-results";


SCHEDULER_PASS(CLBBypasserModule)
