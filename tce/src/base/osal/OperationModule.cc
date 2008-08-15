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
 * @file OperationModule.cc
 *
 * Definition of OperationModule class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#include <string>

#include "OperationModule.hh"
#include "FileSystem.hh"

using std::string;

/// Property file extension.
const string OperationModule::PROPERTY_FILE_EXTENSION = ".opp";
/// Behavior file extension.
const string OperationModule::BEHAVIOR_FILE_EXTENSION = ".opb";
/// Source file extension.
const string OperationModule::BEHAVIOR_SOURCE_FILE_EXTENSION = ".cc";

/**
 * Constructor.
 *
 * @param name The name of the module.
 * @param path The path of the module.
 */
OperationModule::OperationModule(
    const std::string& name, 
    const std::string& path) : path_(path), name_(name) {
}

/**
 * Copy constructor.
 *
 * @param om The module to be copied.
 */
OperationModule::OperationModule(const OperationModule& om) {
    name_ = om.name();
    path_ = FileSystem::directoryOfPath(om.propertiesModule());
}

/**
 * Destructor.
 */
OperationModule::~OperationModule() {
}

/**
 * Assignment operator.
 *
 * @param om Module to be assigned.
 */
OperationModule&
OperationModule::operator=(const OperationModule& om) {
    name_ = om.name();
    path_ = FileSystem::directoryOfPath(om.propertiesModule());
    return *this;
}

/**
 * Checks whether OperationModule has behavior definition file.
 *
 * @return True if OperationModule has behavior definition file.
 */
bool
OperationModule::definesBehavior() const {
    return (FileSystem::fileExists(
                path_ + FileSystem::DIRECTORY_SEPARATOR + behaviorFileName()));
}

/**
 * Returns the name of the behavior module.
 *
 * @return The name of the behavior module.
 * @exception FileNotFound If behavior module is not found.
 */
string
OperationModule::behaviorModule() const
    throw (FileNotFound) {
    
    if (!definesBehavior()) {
        std::string method = "OperationModule::behaviorModule()";
        std::string msg = "Behavior file not found.";
        throw FileNotFound(__FILE__, __LINE__, method, msg);
    }

    return path_ + FileSystem::DIRECTORY_SEPARATOR + behaviorFileName();
}

/**
 * Returns the name of the properties module.
 *
 * @return The name of the properties module.
 */
string
OperationModule::propertiesModule() const {
    return path_ + FileSystem::DIRECTORY_SEPARATOR + propertyFileName();
}

/**
 * Checks whether module has behavior source file.
 *
 * @return True if module has behavior source file.
 */
bool
OperationModule::hasBehaviorSource() const {
    return (FileSystem::fileExists(
                path_ + FileSystem::DIRECTORY_SEPARATOR + 
                behaviorSourceFileName()));
}

/**
 * Returns the name of the behavior source code file.
 *
 * @return The name of the behavior source code file.
 * @exception FileNotFound If behavior source file not found.
 */
string
OperationModule::behaviorSourceModule() const
    throw (FileNotFound) {

    if (!hasBehaviorSource()) {
        std::string method = "OperationModule::behaviorSourceModule()";
        std::string msg = "Behavior source file not found.";
        throw FileNotFound(__FILE__, __LINE__, method, msg);
    }

    return path_ + FileSystem::DIRECTORY_SEPARATOR + behaviorSourceFileName();
}

/**
 * Returns the name of the module.
 *
 * @return The name of the module.
 */
std::string
OperationModule::name() const {
    return name_;
}

/**
 * Returns the name of the behavior file name.
 *
 * @return The name of the behavior file name. 
 */
std::string
OperationModule::behaviorFileName() const {
    return name_ + BEHAVIOR_FILE_EXTENSION;
}

/**
 * Returns the name of the properties file.
 *
 * @return The name of the properties file.
 */
std::string
OperationModule::propertyFileName() const {
    return name_ + PROPERTY_FILE_EXTENSION;
}

/**
 * Returns the name of the code file.
 *
 * @return The name of the code file.
 */
std::string
OperationModule::behaviorSourceFileName() const {
    return name_ + BEHAVIOR_SOURCE_FILE_EXTENSION;
}

//////////////////////////////////////////////////////////////////////////////
// NullOperationModule
//////////////////////////////////////////////////////////////////////////////

NullOperationModule NullOperationModule::instance_;

/** 
 * Constructor.
 */
NullOperationModule::NullOperationModule() : OperationModule("<NULL>", "") {
}

/**
 * Destructor.
 */
NullOperationModule::~NullOperationModule() {
}

/**
 * Writes a message to error log and aborts the program.
 *
 * @return Never returns.
 */
string
NullOperationModule::name() const {
    Application::abortWithError("name()");
    return "";
}

/**
 * Writes a message to error log and aborts the program.
 *
 * @return Never returns.
 */
bool
NullOperationModule::definesBehavior() const {
    Application::abortWithError("definesBehavior()");
    return false;
}

/**
 * Writes a message to error log and aborts the program.
 *
 * @return Never returns.
 * @exception FileNotFound If behavior module is not found.
 */
string
NullOperationModule::behaviorModule() const
    throw (FileNotFound) {
    
    Application::abortWithError("behaviorModule()");
    return "";
}

/**
 * Writes a message to error log and aborts the program.
 *
 * @return Never returns.
 */
string
NullOperationModule::propertiesModule() const {
    
    Application::abortWithError("propertiesModule()");
    return "";
}

/**
 * Writes a message to error log and aborts the program. 
 *
 * @return Never returns.
 */
bool
NullOperationModule::hasBehaviorSource() const {
    Application::abortWithError("hasBehaviorSource()");
    return false;
}

/**
 * Writes a message to error log and aborts the program. 
 *
 * @return Never returns.
 * @exception FileNotFound Doesn't throw.
 */
string
NullOperationModule::behaviorSourceModule() const
    throw (FileNotFound) {
    
    Application::abortWithError("behaviorSourceModule()");
    return "";
}
