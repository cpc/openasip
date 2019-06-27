/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CommandRegistry.cc
 *
 * Definition of CommandRegistry class.
 *
 * @author Veli-Pekka J��skel�inen 2004 (vjaaskel-no.spam-cs.tut.fi)
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
CommandRegistry::commandIcon(const std::string name) const {
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
CommandRegistry::commandName(int id) const {
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
CommandRegistry::commandShortName(const std::string name) const {
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
CommandRegistry::isEnabled(const std::string command) {
    vector<GUICommand*>::iterator i = commands_.begin();
    for (; i != commands_.end(); i++) {
	if ((*i)->name() == command) {
	    return (*i)->isEnabled();
	}
    }
    throw InstanceNotFound(__FILE__, __LINE__, __func__, "");
    return false;
}
