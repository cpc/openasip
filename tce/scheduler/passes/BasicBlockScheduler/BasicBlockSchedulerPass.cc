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
