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
 * @file HelperSchedulerModule.cc
 *
 * Implementation of HelperSchedulerModule class.
 *
 * @author Ari Mets�halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "Application.hh"
#include "HelperSchedulerModule.hh"

using std::string;

/**
 * Constructor.
 */
HelperSchedulerModule::HelperSchedulerModule():
    BaseSchedulerModule(), parent_(NULL) {
}

/**
 * Destructor.
 */
HelperSchedulerModule::~HelperSchedulerModule() {
}

/**
 * Return true if this module can be run independently.
 *
 * @return False always, because this type of module is not supposed
 * to be run independently by means of the start method.
 */
bool
HelperSchedulerModule::isStartable() const {
    return false;
}

/**
 * A helper module cannot be run independently using the start method.
 *
 * @exception ObjectNotInitialized Never.
 * @exception WrongSubclass Always.
 * @exception ModuleRunTimeError Never.
 */
void
HelperSchedulerModule::start()
    throw (ObjectNotInitialized, WrongSubclass, ModuleRunTimeError) {
    string method = "HelperSchedulerModule::start()";
    string msg = "Not startable module!";
    throw WrongSubclass(__FILE__, __LINE__, method, msg);
}

/**
 * Return true if the module has been registered to a parent module.
 *
 * @return True if the module has been registered to a parent module.
 */
bool
HelperSchedulerModule::isRegistered() const {
    return (parent_ != NULL);
}

/**
 * Set the parent module.
 */
void
HelperSchedulerModule::setParent(BaseSchedulerModule& parent) {
    parent_ = &parent;
}

/**
 * Return the parent module.
 *
 * @return The parent module (to which this module has been registered).
 * @exception IllegalRegistration If the module has not been registered
 *            to a parent module.
 */
BaseSchedulerModule&
HelperSchedulerModule::parent() const throw (IllegalRegistration) {
    if (parent_ == NULL) {
        string method = "HelperSchedulerModule::parent()";
        string msg = "Module not registered to a parent module.";
        throw IllegalRegistration(__FILE__, __LINE__, method, msg);
    } else {
        return *parent_;
    }
}

/**
 * A short description of the module
 *
 * @return The description as a string.
 */   
std::string
HelperSchedulerModule::shortDescription() const {
    return "Generic helper module. Detailed description is missing.";
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
HelperSchedulerModule::longDescription() const {
    return "Generic helper module. Detailed description is missing.";
}
