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
 * @file GUICommand.cc
 *
 * Definition of GUICommand class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2004 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <string>

#include "GUICommand.hh"

using std::string;

/**
 * The Constructor.
 */
GUICommand::GUICommand(std::string name, wxWindow* parent = NULL) :
    parent_(parent), name_(name) {

}



/**
 * The Destructor.
 */
GUICommand::~GUICommand() {
}


/**
 * Sets the parent window of the command.
 *
 * Window will be used as parent for the dialogs created by this command.
 *
 * @param parent Parent window for the command dialogs.
 */
void
GUICommand::setParentWindow(wxWindow* parent) {
    parent_ = parent;
}


/**
 * Returns parent window of the command.
 *
 * @return Parent window of the command.
 */
wxWindow*
GUICommand::parentWindow() const {
    return parent_;
}


/**
 * Returns short version of the command name.
 *
 * Base class implementation returns the command normal name.
 *
 * @return Short version of the command name.
 */
std::string
GUICommand::shortName() const {
    return name_;
}


/**
 * Returns name of the command.
 *
 * @return Long name of the command.
 */
std::string
GUICommand::name() const {
    return name_;
}


/**
 * Returns true if a checkbox related to the command is checked.
 *
 * This is meaningful only for toggle-type commands.
 *
 * @return True, if the command feature is toggled on.
 */
bool
GUICommand::isChecked() const {
    return false;
}
