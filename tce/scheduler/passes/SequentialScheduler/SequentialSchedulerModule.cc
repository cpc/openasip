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
