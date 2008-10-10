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
 * @file CommandRegistry.cc
 *
 * Definition of CommandRegistry class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 * @note reviewed Jun 23 2004 by ml, jn, jm, vpj
 */

#include "CommandRegistry.hh"
#include "GUICommand.hh"

using std::vector;
using std::string;


/**
 * The Constructor.
 */
CommandRegistry::CommandRegistry() {
}


/**
 * The Destructor.
 */
CommandRegistry::~CommandRegistry() {
    // delete commands in registry
    vector<GUICommand*>::iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	delete *i;
    }
    commands_.clear();
}


/**
 * Creates a new command corresponding to the id.
 *
 * @param id Enumerated ID of the GUICommand.
 * @return Created GUICommand, or NULL, if no GUICommand was found
 *         with the ID.
 */
GUICommand*
CommandRegistry::createCommand(const int id) {
    vector<GUICommand*>::iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->id() == id) {
	    return (*i)->create();
	}
    }
    return NULL;
}

/**
 * Creates a new command corresponding to the command name.
 *
 * @param name Name of the GUICommand.
 * @return Created GUICommand, or NULL, if no GUICommand was found
 *         with the name.
 */
GUICommand*
CommandRegistry::createCommand(const std::string name) {
    vector<GUICommand*>::iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->name() == name) {
	    return (*i)->create();
	}
    }
    return NULL;
}


/**
 * Returns first command in the registry.
 *
 * @return Pointer to the first command in the registry.
 */
GUICommand*
CommandRegistry::firstCommand() {
    iterator_ = commands_.begin();
    if (iterator_ == commands_.end()) {
	return NULL;
    }
    return (*iterator_);
}



/**
 * Returns next command in the registry.
 *
 * @return Pointer to the next command in the registry.
 */
GUICommand*
CommandRegistry::nextCommand() {
    iterator_++;
    if (iterator_ == commands_.end()) {
	return NULL;
    }
    return (*iterator_);
}


/**
 * Returns enumerated ID of the command.
 *
 * @param name Name of the GUICommand.
 * @return Enumerated ID of the GUICommand, or -1 if no command was
 *         found.
 */
int
CommandRegistry::commandId(const std::string name) const {

    vector<GUICommand*>::const_iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->name() == name) {
	    return (*i)->id();
	}
    }
    return -1;
}

/**
 * Returns icon for the GUICommand.
 *
 * @param name Name of the GUICommand.
 * @return Icon for the GUICommand.
 */
std::string
CommandRegistry::commandIcon(const std::string name) const
    throw (InstanceNotFound) {

    vector<GUICommand*>::const_iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->name() == name) {
	    return (*i)->icon();
	}
    }
    throw InstanceNotFound(__FILE__, __LINE__, __func__, "");
    return "";
}

/**
 * Returns name of the GUICommand.
 *
 * @param id ID of the GUICommand.
 * @return Command name.
 */
std::string
CommandRegistry::commandName(int id) const
    throw(InstanceNotFound) {

    vector<GUICommand*>::const_iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->id() == id) {
	    return (*i)->name();
	}
    }
    throw InstanceNotFound(__FILE__, __LINE__, __func__, "");
    return "";
}

/**
 * Returns short name of the GUICommand.
 *
 * @param name Name of the GUICommand.
 * @return Short version of the command name.
 */
std::string
CommandRegistry::commandShortName(const std::string name) const
    throw(InstanceNotFound) {

    vector<GUICommand*>::const_iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->name() == name) {
	    return (*i)->shortName();
	}
    }
    throw InstanceNotFound(__FILE__, __LINE__, __func__, "");
    return "";
}


/**
 * Returns true, if command is executable and should be enabled,
 * false if not.
 *
 * @param command The command which is checked for enabled/disabled state.
 * @return true, if the command is executable.
 */
bool
CommandRegistry::isEnabled(const std::string command)
throw(InstanceNotFound) {

    vector<GUICommand*>::iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->name() == command) {
	    return (*i)->isEnabled();
	}
    }
    throw InstanceNotFound(__FILE__, __LINE__, __func__, "");
    return false;
}
