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
