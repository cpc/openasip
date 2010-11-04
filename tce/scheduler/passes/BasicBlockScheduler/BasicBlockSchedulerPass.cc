/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file BasicBlockSchedulerPass.hh
 *
 * Implementation of BasicBlockSchedulerPass class.
 *
 * Basic block scheduler.
 *
 * @author Pekka J��skel�inen 2007 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <typeinfo>


#include "BasicBlockSchedulerPass.hh"
#include "BBSchedulerController.hh"
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

    BBSchedulerController scheduler(interPassData(),bypasser, filler);
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
